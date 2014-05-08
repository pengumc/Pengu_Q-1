from ctypes import *

class Quadruped:

    def __init__(self, libpath):
        self.lib = CDLL(libpath)
        #restypes
        self.lib.quadruped_alloc.restype = c_void_p
        #startup
        self.q = self.lib.quadruped_alloc();
        
    def __del__(self):
        self.lib.quadruped_free(self.q)
        
        