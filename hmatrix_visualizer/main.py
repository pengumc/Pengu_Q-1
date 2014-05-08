#not actual main thread but almost everything happens here since
#pyplot needs the actual main thread

import threading
import Queue
from quadruped import *
import math

class KeyboardThread (threading.Thread):
    
    def __init__(self):
        threading.Thread.__init__(self)
        self.queue = Queue.Queue()
        
    def run(self):
        import keyboard
        self.Q = Quadruped("../quadruped/bin/quadruped.dll")
        self.setup_pivot0s()
        x = 5.0
        angle = 0.0
        print("usage:\n\t'q' quit\n\t'p' plot")
        print("\t'a' pivot00 x+1\n\t'z' pivot00 angle -0.1\n")
        while True:
            c = keyboard.getch()
            if c == 'q':
                #if the keyboard thread dies, we all die
                print("Waiting for plot to close...")
                self.queue.put(None)
                break
            elif c == 'p':
                #raw_pivot00 = self.Q.get_hmatrix(0)
                raw_pivots = [self.Q.get_hmatrix(i) for i in range(16)]
                self.queue.put(raw_pivots)
                print("plot requested")
            elif c == 'a':
                x = x + 1.0
                self.Q.set_pivot_pos(0, 0, x, 0, 0)
                print("pivot00 x = {}".format(x))
            elif c == 'z':
                angle = angle - 0.1
                self.Q.configure_pivot_rot(0, 0, 2, angle)
                print("pivot00 angle = {}".format(angle))
                
    def setup_pivot0s(self):
        self.Q.set_pivot_pos(0, 0, 5, 5, 0)
        self.Q.configure_pivot_rot(0, 0, 2, math.pi/4) 
        self.Q.set_pivot_pos(1, 0, -5, 5, 0)
        self.Q.configure_pivot_rot(1, 0, 2, 3*math.pi/4) 
        self.Q.set_pivot_pos(2, 0, -5, -5, 0)
        self.Q.configure_pivot_rot(2, 0, 2, 5*math.pi/4) 
        self.Q.set_pivot_pos(3, 0, 5, -5, 0)
        self.Q.configure_pivot_rot(3, 0, 2, 7*math.pi/4) 
        