#linux test for threaded ctypes
import threading
import Queue
from quadruped import *


class A (threading.Thread):
    
    def __init__(self, Q):
        threading.Thread.__init__(self)
        self.Q = Q
        
    def run(self):
        self.Q.set_pivot_pos(0, 0, 5, 5, 0)

if __name__ == "__main__":
    libpath = "../quadruped/bin/libquadruped.so"
    Q = Quadruped(libpath)
    t1 = A(Q)
    t1.start()
    t1.join()
