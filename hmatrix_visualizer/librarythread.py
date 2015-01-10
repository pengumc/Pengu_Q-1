import threading
import Queue
import quadruped
from platform import system
import time
import configuration

class LibraryThread(threading.Thread):

    def __init__(self, qin, qout, qvis, configfile):
        threading.Thread.__init__(self)
        self.qin = qin
        self.qout = qout
        self.qvis = qvis
        self.running = False
        self.configfile = configfile

    def thread_call(self, function, *args, **kwargs):
        self.qin.put((function, args, kwargs))
        
    def run(self):
        # load quadruped
        if system() == 'Linux':
            libpath = "../quadruped/bin/libquadruped.so"
        else: #there's only linux and windows :p
          libpath = "../quadruped/bin/quadruped.dll"
        print("Using library: {}".format(libpath))
        self.Q = quadruped.Quadruped(libpath)
        # configure
        print("configuring robot...")
        self.config = configuration.Configuration(self.configfile)
        self.apply_config()
        # start loop
        self.empty_queue(self.qout)
        self.running = True
        self.qout.put("ready")
        self.should_update_visuals = True
        while self.running:
            try:
                function, args, kwargs = self.qin.get(timeout=0.2)
                val = function(*args, **kwargs)
                self.qout.put(val)
                self.should_update_visuals = True
            except Queue.Empty:
                # nothing to do when call queue is empty
                if self.should_update_visuals:
                    self.queue_visual_data()
                    self.should_update_visuals = False
        print "library thread dying..."
        
    def die(self):
        self.running = False

    def empty_queue(self, queue):
        while True:
            try:
                queue.get(block=False)
            except Queue.Empty:
                break

    def queue_visual_data(self):
        raw_pivots = []
        for i in range(4):
            for pivot in range(4):
                raw_pivots.append(
                    self.Q.get_relative_hmatrix(i, pivot))
        raw_pivots.append(self.Q.get_com());
        self.empty_queue(self.qvis)
        self.qvis.put(raw_pivots)
        print "new data on qvis"
        
    def apply_config(self):
        #mechanical
        for leg in self.config.legs:
            for pivot in leg.pivots:
                self.Q.set_pivot_pos(leg.id, pivot.id, pivot.x, pivot.y,
                    pivot.z)
                self.Q.configure_pivot_rot(leg.id, pivot.id, 0, pivot.rotx)
                self.Q.configure_pivot_rot(leg.id, pivot.id, 1, pivot.roty)
                self.Q.configure_pivot_rot(leg.id, pivot.id, 2, pivot.rotz)
                self.Q.set_pivot_config(leg.id, pivot.id, pivot.offset,
                    pivot.abs_max)
                try:
                    self.Q.set_pivot_pulsewidth_config(leg.id, pivot.id, 
                        pivot.pw_0, pivot.pw_60)
                except AttributeError:
                    pass
        self.Q.update_spring_gg()
        # rest vector are not updated at any time
        d = 3.8 + 7.35 - 1
        h = 11
        self.Q.set_foot_rest_vector(0, d, d+2, -h);
        self.Q.set_foot_rest_vector(1, -d, d+2, -h);
        self.Q.set_foot_rest_vector(2, -d, -d-2, -h);
        self.Q.set_foot_rest_vector(3, d, -d-2, -h);
