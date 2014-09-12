#not actual main thread but almost everything happens here since
#pyplot needs the actual main thread

import threading
import Queue
import math
import quadruped
import configuration
import time
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
        self.xyq = Queue.Queue()
        #quadruped
        if system() == 'Linux':
            libpath = "../quadruped/bin/libquadruped.so"
        else: #there's only linux and windows :p
          libpath = "../quadruped/bin/quadruped.dll"
        print("Using library: {}".format(libpath))
        self.Q = quadruped.Quadruped(libpath)
        #configfile
        print("configuring...")
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
        self.leg = 0
        self.pivot = 0
        self.cob_selected = True
        self.put_on_queue()
        while True:
            c = keyboard.getch()
            if c == 'q':
                #if the keyboard thread dies, we all die
                print("Waiting for plot to close...")
                self.queue.put(None)
                self.queue.put(None)
                break
            #selection
            elif c == '!':
                self.pivot = 0
                self.cob_selected = False
                print "pivot 0 selected"
            elif c == '@':
                self.pivot = 1
                self.cob_selected = False
                print "pivot 1 selected"
            elif c == '#':
                self.pivot = 2
                self.cob_selected = False
                print "pivot 2 selected"
            elif c == '$':
                self.pivot = 3
                self.cob_selected = False
                print "pivot 3 selected"
            elif c == '1':
                self.leg = 0
                self.cob_selected = False
                print "leg 0 selected"
            elif c == '2':
                self.leg = 1
                self.cob_selected = False
                print "leg 1 selected"
            elif c == '3':
                self.leg = 2
                self.cob_selected = False
                print "leg 2 selected"
            elif c == '4':
                self.leg = 3
                self.cob_selected = False
                print "leg 3 selected"
            elif c == '0':
                self.cob_selected = True
                print "CoB selected"
            #wasd
            elif c == 'w':
                if self.cob_selected:
                    # - y
                    self.safe_change_all_feet(0, -0.1, 0)
                else:
                    self.safe_change_single_foot(self.leg, 0, 0.1, 0)
            elif c == 'a':
                if self.cob_selected:
                    # + x
                    self.safe_change_all_feet(0.1, 0, 0)
                else:
                    self.safe_change_single_foot(self.leg, -0.1, 0, 0)
            elif c == 's':
                if self.cob_selected:
                    # + y
                    self.safe_change_all_feet(0, 0.1, 0)
                else:
                    self.safe_change_single_foot(self.leg, 0, -0.1, 0)
            elif c == 'd':
                if self.cob_selected:
                    #- x
                    self.safe_change_all_feet(-0.1, 0, 0)
                else:
                    self.safe_change_single_foot(self.leg, 0.1, 0, 0)
            elif c == '-':
                if self.cob_selected:
                    self.safe_change_all_feet(0, 0, -0.1)
                else:
                    self.safe_change_single_foot(self.leg, 0, 0, 0.1)
            elif c == '+':
                if self.cob_selected:
                    self.safe_change_all_feet(0, 0, +0.1)
                else:
                    self.safe_change_single_foot(self.leg, 0, 0, -0.1)
            elif c == 'z':
                self.Q.change_pivot_angle(self.leg, self.pivot, -0.1)
                self.commit()
            elif c == 'x':
                self.Q.change_pivot_angle(self.leg, self.pivot, 0.1)
                self.commit()
            elif c == '`':
                res = self.Q.sync_from_device()
                print "sync from dev: {}".format(res)
            elif c == '~':
                self.commit()
            elif c == 'r':
                d = 10#11.35#math.cos(math.pi/4) * (3.6+6.8) +4
                h = 11
                print "all feet at {}, -11".format(d)
                self.Q.set_foot_pos(0, d, d+2, -h)
                self.Q.set_foot_pos(1, -d, d+2, -h)
                self.Q.set_foot_pos(2, -d, -d-2, -h)
                self.Q.set_foot_pos(3, d, -d-2, -h)
            elif c == 'f':
                if self.Q.gg_step() == 2:
                    T = self.Q.get_target_foothold()
                    l = self.Q.get_LT()
                    #cob is at 0
                    H = self.Q.get_relative_hmatrix(l, 3)
                    dx = T[3] - H[3]
                    dy = T[7] - H[7]
                    self.transfer_leg(l, dx, dy)
            elif c == 'p':
                print "LASMBF 0: {}, {}".format(
                    self.Q.get_LASMB(0), self.Q.get_LASMF(0))
                print "LASMBF 1: {}, {}".format(
                    self.Q.get_LASMB(1), self.Q.get_LASMF(1))
                print "LASMBF 2: {}, {}".format(
                    self.Q.get_LASMB(2), self.Q.get_LASMF(2))
                print "LASMBF 3: {}, {}".format(
                    self.Q.get_LASMB(3), self.Q.get_LASMF(3))
            elif c == 't':
                try:
                    x, y = self.xyq.get(False)
                except:
                    print "failed to grab data from xyq"
                self.safe_change_all_feet(-x, -y, 0)
            self.put_on_queue()

    def put_on_queue(self):
        raw_pivots = []
        for i in range(4):
            for pivot in range(4):
                raw_pivots.append(
                    self.Q.get_relative_hmatrix(i, pivot))
        raw_pivots.append(self.Q.get_com());
        raw_pivots.append(self.Q.get_KM(0))
        raw_pivots.append(self.Q.get_KM(1))
        raw_pivots.append(self.Q.get_KM(2))
        raw_pivots.append(self.Q.get_KM(3))
        raw_pivots.append(self.Q.get_LASMB(self.leg))
        raw_pivots.append(self.Q.get_LASMF(self.leg))
        #empty queue
        try:
            while True:
                self.queue.get(False)
        except:
            self.queue.put(raw_pivots)


    def commit(self):
        res = self.Q.sync_to_device()
        print "sync to dev: {}".format(res)

    def transfer_leg(self, leg, x, y):
        print "leg {}: {}, {}".format(leg,x,y)
        t = 0.2
        #first check each point
        if self.Q.change_foot_pos(leg, 0, 0, 2, 0): #up 2
            if self.Q.change_foot_pos(leg, x, y, 0, 0): #move xy
                if self.Q.change_foot_pos(leg, 0, 0, -2, 0): #down 2
                    #back to start
                    self.Q.change_foot_pos(leg, -x, -y, 0, 0);
                    self.commit()
                    #now start transfer
                    self.Q.change_foot_pos(leg, 0, 0, 2, 0)
                    self.commit()
                    time.sleep(t)
                    self.Q.change_foot_pos(leg, x, y, 0, 0)
                    self.commit()
                    time.sleep(t)
                    self.Q.change_foot_pos(leg, 0, 0, -2, 0)
                    self.commit()
                else:
                    print("{} down failed".format(leg))
                    self.Q.change_foot_pos(leg, -x, -y, -2, 0) #revert
            else:
                print("{} forward failed".format(leg))
                self.Q.change_foot_pos(leg, 0, 0, -2, 0); #revert
        else:
            print("{} up failed".format(leg))

    def safe_change_single_foot(self, leg, x, y, z):
        if self.Q.change_foot_pos(leg, x, y, z, 0):
            self.commit()
        else:
            print "leg {} change {}, {}, {} failed".format(leg, x, y, z)

    def safe_change_all_feet(self, x, y, z):
        print("change all: x({}) y({}) z({})".format(x,y,z))
        if self.Q.change_foot_pos(0, x, y, z, 0):
            if self.Q.change_foot_pos(1, x, y, z, 0):
                if self.Q.change_foot_pos(2, x, y, z, 0):
                    if self.Q.change_foot_pos(3, x, y, z, 0):
                        self.commit();
                        return True
                    else:
                        print("leg 3 failed")
                        self.Q.change_foot_pos(2, -x, -y, -z, 0)
                        self.Q.change_foot_pos(1, -x, -y, -z, 0)
                        self.Q.change_foot_pos(0, -x, -y, -z, 0)
                        return False
                else:
                    print("leg 2 failed")
                    self.Q.change_foot_pos(1, -x, -y, -z, 0)
                    self.Q.change_foot_pos(0, -x, -y, -z, 0)
                    return False
            else:
                print("leg 1 failed")
                self.Q.change_foot_pos(0, -x, -y, -z, 0)
                return False
        else:
            print("leg 0 failed")
            return False

    def apply_config(self):
        #gaitgenerator
        self.Q.set_gg_config(
            self.config.reachable_sector_radius,
            self.config.transfer_speeds[0],
            self.config.transfer_speeds[1],
            self.config.transfer_speeds[2],
            self.config.ASM_min,
            self.config.ground_clearance,
            self.config.search_width,
            self.config.L_min)
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
        #move y dir
        self.Q.set_gg_velocity(0.0000, 0.0001, 0)