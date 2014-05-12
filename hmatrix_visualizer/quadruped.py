from ctypes import *

class Quadruped:

    def __init__(self, libpath):
        self.lib = CDLL(libpath)
        #restypes
        self.lib.QuadrupedAlloc.restype = c_void_p
        self.lib.QuadrupedGetHMatrix.restype = POINTER(c_double)
        self.lib.QuadrupedGetRelativeHMatrix.restype = POINTER(c_double)
        #startup
        self.q = self.lib.QuadrupedAlloc();

    def __del__(self):
        self.lib.QuadrupedFree(self.q)

    def set_pivot_pos(self, leg, pivot, x, y, z):
        self.lib.QuadrupedSetPivotPos(
            self.q,
            int(leg),
            int(pivot),
            c_double(x),
            c_double(y),
            c_double(z))

    def test(self):
        self.set_pivot_pos(0, 0, 5.125689, 5.0, 0.0)
        a =  self.lib.QuadrupedGetHMatrix(self.q, 0)
        self.print_hmatrix(a)
        return a

    def print_hmatrix(self, H):
        print("{} {} {} {}\n{} {} {} {}\n{} {} {} {}\n{} {} {} {}".format(
            H[0], H[1], H[2], H[3], H[4], H[5], H[6], H[7], H[8], H[9], H[10],
            H[11], H[12], H[13], H[14], H[15]).rjust(4))

    def get_hmatrix(self, index):
        return self.lib.QuadrupedGetHMatrix(self.q, int(index))

    def get_relative_hmatrix(self, leg, pivot):
        return self.lib.QuadrupedGetRelativeHMatrix(self.q, int(leg),
            int(pivot))

    def configure_pivot_rot(self, leg, pivot, axis, angle):
        self.lib.QuadrupedConfigurePivotRot(self.q, int(leg), int(pivot),
            int(axis), c_double(angle))

    def set_pivot_config(self, leg, pivot, offset, abs_max):
        self.lib.QuadrupedSetPivotConfig(self.q, int(leg), int(pivot), c_double(offset),
            c_double(abs_max))
        
