from ctypes import *

class Quadruped:

    def __init__(self, libpath):
        self.lib = CDLL(libpath)
        #restypes
        self.lib.QuadrupedAlloc.restype = c_void_p
        self.lib.QuadrupedGetHMatrix.restype = POINTER(c_double)
        #startup
        self.q = self.lib.QuadrupedAlloc();
        
    def __del__(self):
        self.lib.QuadrupedFree(self.q)
        
    def test(self):
        a =  self.lib.QuadrupedGetHMatrix(self.q, 0)
        for i in range(16):
            print a[i]
        return a