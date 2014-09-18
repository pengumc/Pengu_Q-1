from ctypes import *

class Quadruped:

    def __init__(self, libpath):
        self.lib = CDLL(libpath)
        #restypes
        self.lib.QuadrupedAlloc.restype = c_void_p
        self.lib.QuadrupedGetHMatrix.restype = POINTER(c_double)
        self.lib.QuadrupedGetRelativeHMatrix.restype = POINTER(c_double)
        self.lib.QuadrupedGetCoM.restype = POINTER(c_double)
        self.lib.QuadrupedGetTargetFoothold.restype = POINTER(c_double)
        self.lib.QuadrupedChangePivotAngle.restype = c_bool
        self.lib.QuadrupedChangeFootPos.restype = c_bool
        self.lib.QuadrupedSetFootPos.restype = c_bool
        self.lib.QuadrupedSyncToDev.restype = c_bool
        self.lib.QuadrupedSyncFromDev.restype = c_bool
        self.lib.QuadrupedGetKM.restype = c_double
        self.lib.QuadrupedGetLASMB.restype = c_double
        self.lib.QuadrupedGetLASMF.restype = c_double
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

    def get_com(self):
        return self.lib.QuadrupedGetCoM(self.q)

    def configure_pivot_rot(self, leg, pivot, axis, angle):
        self.lib.QuadrupedConfigurePivotRot(self.q, int(leg), int(pivot),
            int(axis), c_double(angle))

    def set_pivot_config(self, leg, pivot, offset, abs_max):
        self.lib.QuadrupedSetPivotConfig(self.q, int(leg), int(pivot),
            c_double(offset), c_double(abs_max))

    def change_pivot_angle(self, leg, pivot, angle):
        return self.lib.QuadrupedChangePivotAngle(self.q, int(leg), int(pivot),
            c_double(angle))

    def change_foot_pos(self, leg, dx, dy, dz, mode):
        return self.lib.QuadrupedChangeFootPos(self.q, int(leg), c_double(dx),
            c_double(dy), c_double(dz), int(mode))
    def set_foot_pos(self, leg, x, y, z):
        return self.lib.QuadrupedSetFootPos(self.q, int(leg), c_double(x),
            c_double(y), c_double(z))

    def connect(self, vid, pid):
        return self.lib.QuadrupedConnectDev(self.q, c_uint16(vid),
            c_uint16(pid))

    def sync_from_device(self):
        return self.lib.QuadrupedSyncFromDev(self.q)

    def sync_to_device(self):
        return self.lib.QuadrupedSyncToDev(self.q)

    def get_KM(self, leg):
        return self.lib.QuadrupedGetKM(self.q, leg)
        
    def set_gg_config(self, HLlength, speed1, speed2, speed3, ASM_min,
                      ground_clearance, search_width, L_min):
        self.lib.QuadrupedSetGGConfig(self.q, c_double(HLlength),
                                      c_double(speed1), c_double(speed2),
                                      c_double(speed3),
                                      c_double(ASM_min),
                                      c_double(ground_clearance),
                                      c_double(search_width),
                                      c_double(L_min))
        print "HLlength : {}".format(HLlength)
        print "transfer speeds : {}, {}, {}".format(speed1, speed2, speed3)
        print "ASM_min: {}".format(ASM_min)
        print "ground_clearance: {}".format(ground_clearance)
        print "search_width: {}".format(search_width)
        print "L_min: {}".format(L_min)
        
    def set_gg_velocity(self, x, y, z):
        self.lib.QuadrupedSetGGVelocity(self.q, c_double(x), c_double(y),
                                        c_double(z))
    
    def get_LASMB(self, leg):
        return self.lib.QuadrupedGetLASMB(self.q, leg)

    def get_LASMF(self, leg):
        return self.lib.QuadrupedGetLASMF(self.q, leg)
        
    def gg_step(self):
        r = self.lib.QuadrupedGGStep(self.q)
        if r == 0:
            print "stepresult: transferring"
        elif r == 1:
            print "stepresult: no liftable legs"
        elif r == 2:
            print "stepresult: new foothold for leg {}".format(self.get_LT())
            #self.print_hmatrix(self.get_target_foothold())
        elif r == 3:
            print "stepresult: calculate failed"
        elif r == 4:
            print "stepresult: AAAARGH!"
        return r
        
    def get_target_foothold(self):
        return self.lib.QuadrupedGetTargetFoothold(self.q)
        
    def get_LT(self):
        return self.lib.QuadrupedGetLT(self.q)
        
    def set_pivot_pulsewidth_config(self, leg, pivot, pw_0, pw_60):
        self.lib.QuadrupedSetPivotPulsewidthConfig(self.q, leg, pivot,
            c_double(pw_0), c_double(pw_60))
    
    def set_all_angles_to_0(self):
        self.lib.QuadrupedSetAllAnglesTo0(self.q);