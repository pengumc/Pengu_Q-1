from ctypes import *

class Quadruped:

    def __init__(self, libpath):
        self.lib = CDLL(libpath)
        #restypes
        self.lib.QuadrupedAlloc.restype = c_void_p
        self.lib.QuadrupedGetHMatrix.restype = POINTER(c_double)
        self.lib.QuadrupedGetRelativeHMatrix.restype = POINTER(c_double)
        self.lib.QuadrupedGetCoM.restype = POINTER(c_double)
        self.lib.QuadrupedChangePivotAngle.restype = c_bool
        self.lib.QuadrupedChangeFootPos.restype = c_bool
        self.lib.QuadrupedSetFootPos.restype = c_bool
        self.lib.QuadrupedSyncToDev.restype = c_bool
        self.lib.QuadrupedSyncFromDev.restype = c_bool
        self.lib.QuadrupedCanLiftLeg.restype = c_bool
        self.lib.QuadrupedGetLastSpringGGVector.restype = POINTER(c_double)
        self.lib.QuadrupedEqualizeFeetLevels.restype = c_bool
        self.lib.QuadrupedSetBodyRotation.restype = c_bool
        self.lib.QuadrupedChangeBodyRotation.restype = c_bool
        self.lib.QuadrupedGetMiscData.restype = POINTER(c_uint8)
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
 
    def set_all_angles_to_0(self):
        self.lib.QuadrupedSetAllAnglesTo0(self.q);
        
    def update_spring_gg(self):
        self.lib.QuadrupedUpdateSpringGG(self.q)
        
    def zero_spring_gg(self):
        self.lib.QuadrupedZeroSpringGG(self.q)
        
    def get_leg_with_highest_force(self, angle):
        return self.lib.QuadrupedGetLegWithHighestForce(self.q, c_double(angle))
        
    def can_lift_leg(self, index, margin):
        return self.lib.QuadrupedCanLiftLeg(self.q, index, c_double(margin))
        
    def get_last_spring_gg_vector(self, index, angle, F):
        return self.lib.QuadrupedGetLastSpringGGVector(self.q, index, 
            c_double(angle), c_double(F))

    def equalize_feet_levels(self, z):
        return self.lib.QuadrupedEqualizeFeetLevels(self.q, c_double(z))
        
    def change_body_rotation(self, axis, angle):
        return self.lib.QuadrupedChangeBodyRotation(self.q, int(axis),
            c_double(angle))
        
    def set_body_rotation(self, xrot, yrot, zrot):
        return self.lib.QuadrupedSetBodyRotation(self.q, c_double(xrot),
            c_double(yrot), c_double(zrot))
        
    def reset_body(self):
        self.lib.QuadrupedResetBody(self.q)
        
    def get_misc_data(self):
        r = self.lib.QuadrupedGetMiscData(self.q)
        if r:
            return r
        else:
            return None
