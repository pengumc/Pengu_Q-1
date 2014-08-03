#not actual main thread but almost everything happens here since
#pyplot needs the actual main thread

import threading
import Queue
import math
import quadruped
import configuration
from platform import system

class KeyboardThread (threading.Thread):

    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'

    def __init__(self):
        threading.Thread.__init__(self)
        self.queue = Queue.Queue()
        #quadruped
        if system() == 'Linux':
            libpath = "../quadruped/bin/libquadruped.so"
        else: #there's only linux and windows :p
          libpath = "../quadruped/bin/quadruped.dll"
        print("Using library: {}".format(libpath))
        self.Q = quadruped.Quadruped(libpath)
        #configfile
        self.config = configuration.Configuration("config.xml")
        self.connected = False

    def run(self):
        import keyboard
        self.apply_config()
        c = self.Q.connect(0x16c0, 0x05df) 
        if c == 0:
          print self.OKGREEN + "connected " + self.ENDC
          self.connected = True
          res = self.Q.sync_from_device()
          print "sync from dev: {}".format(res)
        else:
          print (self.FAIL + "could not connect {}" + self.ENDC).format(c)
          self.connected = False
        x = 5.0
        print("usage:\n\tq quit\n\tp plot")
        #print("\t1..4 select leg")
        print("\t1..4 select leg 1..4")
        print("\t!..$ select pivot 0..3")
        # wasd move body on xyplane
        # +- raise lower body
        # rt move foot 5 cm forward in nice transfer
        print("\tz servo +0.1\n\tx servo -0.1")
        print("\tx all legs z -0.1\n\t+ all legs z +0.1")
        print("\t` sync from dev")
        print("\t~ sync to dev")
        leg = 0
        pivot = 0
        while True:
            c = keyboard.getch()
            if c == 'q':
                #if the keyboard thread dies, we all die
                print("Waiting for plot to close...")
                self.queue.put(None)
                self.queue.put(None)
                break
            elif c == 'p':
                #raw_pivot00 = self.Q.get_hmatrix(0)
                raw_pivots = []
                for leg in range(4):
                    for pivot in range(4):
                        raw_pivots.append(
                            self.Q.get_relative_hmatrix(leg, pivot))
                raw_pivots.append(self.Q.get_com());
                self.Q.print_hmatrix(raw_pivots[16])
                self.queue.put(raw_pivots[0:17])
                print("plot requested")
            #selection
            elif c == '!':
              pivot = 0
              print "pivot 0 selected"
            elif c == '@':
              pivot = 1
              print "pivot 1 selected"
            elif c == '#':
              pivot = 2
              print "pivot 2 selected"
            elif c == '$':
              pivot = 3
              print "pivot 3 selected"
            elif c == '1':
              leg = 0
              print "leg 0 selected"
            elif c == '2':
              leg = 1
              print "leg 1 selected"
            elif c == '3':
              leg = 2
              print "leg 2 selected"
            elif c == '4':
              leg = 3
              print "leg 3 selected"
            #wasd
            elif c == 'w':
              # + y
                self.Q.change_foot_pos(0, 0, 0.1, 0, 0)
                self.Q.change_foot_pos(1, 0, 0.1, 0, 0)
                self.Q.change_foot_pos(2, 0, 0.1, 0, 0)
                self.Q.change_foot_pos(3, 0, 0.1, 0, 0)
                self.commit()
            elif c == 'a':
                # - x
                self.Q.change_foot_pos(0, -0.1, 0, 0, 0)
                self.Q.change_foot_pos(1, -0.1, 0, 0, 0)
                self.Q.change_foot_pos(2, -0.1, 0, 0, 0)
                self.Q.change_foot_pos(3, -0.1, 0, 0, 0)
                self.commit()
            elif c == 's':
                # - y
                self.Q.change_foot_pos(0, 0, -0.1, 0, 0)
                self.Q.change_foot_pos(1, 0, -0.1, 0, 0)
                self.Q.change_foot_pos(2, 0, -0.1, 0, 0)
                self.Q.change_foot_pos(3, 0, -0.1, 0, 0)
                self.commit()
            elif c == 'd':
                + x
                self.Q.change_foot_pos(0, 0.1, 0, 0, 0)
                self.Q.change_foot_pos(1, 0.1, 0, 0, 0)
                self.Q.change_foot_pos(2, 0.1, 0, 0, 0)
                self.Q.change_foot_pos(3, 0.1, 0, 0, 0)
                self.commit()
            elif c == 'z':
                self.Q.change_pivot_angle(leg, pivot, -0.1)
                self.commit()
            elif c == 'x':
                self.Q.change_pivot_angle(leg, pivot, 0.1)
                self.commit()
            elif c == '`':
                res = self.Q.sync_from_device()
                print "sync from dev: {}".format(res)
            elif c == '~':
                self.commit()
            elif c == 'r':
                d = math.cos(math.pi/4) * (3.6+6.8) +5.37
                print "all feet at {}, -10".format(d) 
                self.Q.set_foot_pos(0, d, d, -10) 
                self.Q.set_foot_pos(1, -d, d, -10) 
                self.Q.set_foot_pos(2, -d, -d, -10) 
                self.Q.set_foot_pos(3, d, -d, -10) 
            elif c == '-':
                self.Q.change_foot_pos(0, 0, 0, -0.1, 0)
                self.Q.change_foot_pos(1, 0, 0, -0.1, 0)
                self.Q.change_foot_pos(2, 0, 0, -0.1, 0)
                self.Q.change_foot_pos(3, 0, 0, -0.1, 0)
                self.commit()
            elif c == '+':
                self.Q.change_foot_pos(0, 0, 0, +0.1, 0)
                self.Q.change_foot_pos(1, 0, 0, +0.1, 0)
                self.Q.change_foot_pos(2, 0, 0, +0.1, 0)
                self.Q.change_foot_pos(3, 0, 0, +0.1, 0)
                self.commit()
                
    def commit(self):
        res = self.Q.sync_to_device()
        print "sync to dev: {}".format(res)

    def apply_config(self):
        for leg in self.config.legs:
            for pivot in leg.pivots:
                self.Q.set_pivot_pos(leg.id, pivot.id, pivot.x, pivot.y,
                    pivot.z)
                self.Q.configure_pivot_rot(leg.id, pivot.id, 0, pivot.rotx)
                self.Q.configure_pivot_rot(leg.id, pivot.id, 1, pivot.roty)
                self.Q.configure_pivot_rot(leg.id, pivot.id, 2, pivot.rotz)
                self.Q.set_pivot_config(leg.id, pivot.id, pivot.offset, 
                    pivot.abs_max)
