#not actual main thread but almost everything happens here since
#pyplot needs the actual main thread

import threading
import Queue
import math
import quadruped
import configuration
from platform import system

class KeyboardThread (threading.Thread):

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

    def run(self):
        import keyboard
        self.apply_config()
        x = 5.0
        print("usage:\n\t'q' quit\n\t'p' plot")
        print("\t'a' foot0 z + 0.3\n\t'z' foot0 z - 0.3\n")
        while True:
            c = keyboard.getch()
            if c == 'q':
                #if the keyboard thread dies, we all die
                print("Waiting for plot to close...")
                self.queue.put(None)
                break
            elif c == 'p':
                #raw_pivot00 = self.Q.get_hmatrix(0)
                raw_pivots = []
                for leg in range(4):
                    for pivot in range(4):
                        raw_pivots.append(
                            self.Q.get_relative_hmatrix(leg, pivot))
                self.queue.put(raw_pivots[0:16])
                print("plot requested")
            elif c == 'a':
              self.Q.change_foot_pos(0, 0, 0, 0.3, 0)
              print("foot0 z + 0.3")
            elif c == 'z':
              self.Q.change_foot_pos(0, 0, 0, -0.3, 0)
              print("foot0 z - 0.3")
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
