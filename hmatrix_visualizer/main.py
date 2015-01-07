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
        print("\tz servo -0.1\n\tx servo +0.1")
        print("\tx all legs z -0.1\n\t+ all legs z +0.1")
        print("\t` sync from dev")
        print("\t~ sync to dev")
        self.leg = 0
        self.pivot = 0
        self.sgg_leg = -1
        self.sgg_target = None
        self.sgg_liftable = False
        self.sgg_last = -1
        self.cob_selected = True
        self.put_on_queue()
        self.input_mode = 0
        self.speed  = 0.3
        self.cob_moved = [0.0, 0.0, 0.0]
        self.high_force = True
        while True:
            if self.input_mode == 0:
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
                        if self.Q.change_all_feet_pos(0, -self.speed, 0):
                            self.cob_moved[1] += self.speed
                    else:
                        self.Q.change_foot_pos(self.leg, 0, self.speed, 0, 0)
                    self.commit()
                elif c == 'W':
                    print "high_force = ", self.high_force
                    if self.high_force:
                        if self.cycle(math.pi/2, -11.5):
                            self.high_force = False
                        else:
                            if self.Q.change_all_feet_pos(0, -self.speed, 0):
                                self.cob_moved[1] += self.speed
                    else:
                        if self.cycle(math.pi/2, -7):
                            self.high_force = True
                        else:
                            if self.Q.change_all_feet_pos(0, -self.speed, 0):
                                self.cob_moved[1] += self.speed
                    self.commit()
                elif c == 'a':
                    if self.cob_selected:
                        # + x
                        if self.Q.change_all_feet_pos(self.speed, 0, 0):
                            self.cob_moved[0] -= self.speed
                    else:
                        self.Q.change_foot_pos(self.leg, -self.speed, 0, 0, 0)
                            
                    self.commit()
                elif c == 'A':
                    print "high_force = ", self.high_force
                    if self.high_force:
                        if self.cycle(math.pi, -10):
                            self.high_force = False
                        else:
                            if self.Q.change_all_feet_pos(self.speed, 0, 0):
                                self.cob_moved[0] -= self.speed
                    else:
                        if self.cycle(math.pi, -7):
                            self.high_force = True
                        else:
                            if self.Q.change_all_feet_pos(self.speed, 0, 0):
                                self.cob_moved[0] -= self.speed
                    self.commit()
                elif c == 's':
                    if self.cob_selected:
                        # + y
                        if self.Q.change_all_feet_pos(0, self.speed, 0):
                            self.cob_moved[1] -= self.speed 
                    else:
                        self.Q.change_foot_pos(self.leg, 0, -self.speed, 0, 0)
                    self.commit()
                elif c == 'S':
                    print "high_force = ", self.high_force
                    if self.high_force:
                        if self.cycle(-math.pi/2, -11.5):
                            self.high_force = False
                        else:
                            if self.Q.change_all_feet_pos(0, self.speed, 0):
                                self.cob_moved[1] -= self.speed
                    else:
                        if self.cycle(-math.pi/2, -7):
                            self.high_force = True
                        else:
                            if self.Q.change_all_feet_pos(0, self.speed, 0):
                                self.cob_moved[1] -= self.speed
                    self.commit()
                elif c == 'd':
                    if self.cob_selected:
                        #- x
                        if self.Q.change_all_feet_pos(-self.speed, 0, 0):
                            self.cob_moved[0] += self.speed
                    else:
                        self.Q.change_foot_pos(self.leg, self.speed, 0, 0, 0)
                    self.commit()
                elif c == 'D':
                    print "high_force = ", self.high_force
                    if self.high_force:
                        if self.cycle(0, -10):
                            self.high_force = False
                        else:
                            if self.Q.change_all_feet_pos(-self.speed, 0, 0):
                                self.cob_moved[0] += self.speed
                    else:
                        if self.cycle(0, -7):
                            self.high_force = True
                        else:
                            if self.Q.change_all_feet_pos(-self.speed, 0, 0):
                                self.cob_moved[0] += self.speed
                    self.commit()

                #OTHERS
                elif c == '-':
                    if self.cob_selected:
                        self.Q.change_all_feet_pos(0, 0, -0.1)
                    else:
                        self.Q.change_foot_pos(self.leg, 0, 0, 0.1, 0)
                    self.commit()
                elif c == '+':
                    if self.cob_selected:
                        self.Q.change_all_feet_pos(0, 0, +0.1)
                    else:
                        self.Q.change_foot_pos(self.leg, 0, 0, -0.1, 0)
                    self.commit()
                elif c == 'n':
                    if not self.cob_selected:
                        self.stable_up(self.leg)
                elif c == 'm':
                    if not self.cob_selected:
                        self.stable_down(self.leg)
                elif c == 'z':
                    self.Q.change_pivot_angle(self.leg, self.pivot, -0.01)
                    self.commit()
                elif c == 'x':
                    self.Q.change_pivot_angle(self.leg, self.pivot, 0.01)
                    self.commit()
                elif c == 'Z':
                    self.Q.change_pivot_angle(self.leg, self.pivot, -0.1)
                    self.commit()
                elif c == 'X':
                    self.Q.change_pivot_angle(self.leg, self.pivot, 0.1)
                    self.commit()
                elif c == '`':
                    res = self.Q.sync_from_device()
                    print "sync from dev: {}".format(res)
                elif c == '~':
                    self.commit()
                elif c == 'r':
                    self.startpos()
                elif c == 'R':
                    self.Q.set_all_angles_to_0()
                elif c == 't':
                    try:
                        x, y = self.xyq.get(False)
                    except:
                        print "failed to grab data from xyq"
                    self.Q.change_all_feet_pos(-x, -y, 0)
                elif c == 'k':
                    for i in range(5): self.stable_turn(0)
                # TEST BUTTON Y
                elif c == 'y':
                    for i in range(5): self.stable_turn(0.3)
                elif c == 'Y':
                    for i in range(5): self.stable_turn(-0.3)
                #Body rotation [ ] { } ; '
                elif c == '[':
                    print "body z + ", self.Q.change_body_rotation(2, 0.1*self.speed)
                    self.commit()
                elif c == ']':
                    print "body z - ", self.Q.change_body_rotation(2, -0.1*self.speed)
                    self.commit()
                elif c == '{':
                    print "body y +", self.Q.change_body_rotation(1, 0.1*self.speed)
                    self.commit()
                elif c == '}':
                    print "body y -", self.Q.change_body_rotation(1, -0.1*self.speed)
                    self.commit()
                elif c == ';':
                    print "body x +", self.Q.change_body_rotation(0, 0.1*self.speed)
                    self.commit()
                elif c == "'":
                    print "body x -", self.Q.change_body_rotation(0, -0.1*self.speed)
                    self.commit()
                elif c == '?':
                    print "set rot 0, 0.1, 0 " , self.Q.set_body_rotation(0, 0.1, 0);
                    self.commit()
                # U I O P springgg commands
                elif c == 'u':
                    self.Q.update_spring_gg()
                    self.sgg_leg = self.Q.get_leg_with_highest_force(math.pi/2)
                    print "HF leg (0-based): {}".format(self.sgg_leg)
                elif c == 'i':
                    if 0 <= self.sgg_leg <= 3:
                        self.sgg_liftable = self.Q.can_lift_leg(self.sgg_leg, 1.0)
                        print "leg {} can be lifted? {}".format(
                            self.sgg_leg, self.sgg_liftable)
                    else:
                        print "sgg_leg = {}".format(self.sgg_leg)
                elif c == 'o':
                    if 0 <= self.sgg_leg <= 3:
                        F = 11
                        if self.sgg_leg == 0 or self.sgg_leg == 1: F = 7
                        self.sgg_target = self.Q.get_last_spring_gg_vector(
                            self.sgg_leg, math.pi/2, F)
                        print "sgg target {}, {}, {}".format(
                            self.sgg_target[0], self.sgg_target[1],
                            self.sgg_target[2])
                        if self.sgg_target[2] <> 0.0:
                            self.sgg_target = None
                            print "sgg failed to find target"
                    else:
                        print "sgg_leg = {}".format(self.sgg_leg)
                elif c == 'p':
                    if (0 <= self.sgg_leg <= 3 and self.sgg_liftable and 
                    self.sgg_target[2] == 0.0):
                        print "changing foot {} by {}, {}".format(
                            self.sgg_leg, self.sgg_target[0], self.sgg_target[1])
                        self.transfer_leg(self.sgg_leg, self.sgg_target[0],
                            self.sgg_target[1])
                    else:
                        print "leg {} {}".format(self.sgg_leg, self.sgg_liftable)
                elif c == '[':
                    self.cycle()
                # end keyboard character cases
                self.put_on_queue()
            else: 
                #TODO (michiel): something breaks usb packet order
                #input mode 1
                time.sleep(0.2)
                misc_data = self.Q.get_misc_data()
                if misc_data:
                  print misc_data[0:8]
                  if (misc_data[0] & (1<<3)) == 0:
                      #start pressed
                      self.input_mode = 0
                      print "switch back to keyboard"
                  xrot = 0 # (float(misc_data[5])-128)/128.0 * 0.1
                  yrot = (float(misc_data[4])-128)/128.0 * 0.1
                  if self.Q.set_body_rotation(xrot, yrot, 0):
                    self.commit()
                    time.sleep(0.2)
                  #self.put_on_queue()
                  else:
                    print "rotate failed"
                else:
                  print "data was none"
                  self.input_mode = 0
                  

    def put_on_queue(self):
        raw_pivots = []
        for i in range(4):
            for pivot in range(4):
                raw_pivots.append(
                    self.Q.get_relative_hmatrix(i, pivot))
        raw_pivots.append(self.Q.get_com());
        try:
            while True:
                self.queue.get(False)
        except:
            self.queue.put(raw_pivots)

    def startpos(self):
        print "revert to rest vectors"
        self.Q.reset_body()
        for i in range(4):
            v = self.Q.get_foot_rest_vector(i, 0, 0)
            print v[0], v[1], v[2]
            print self.Q.set_foot_pos(i, v[0], v[1], v[2]);
        self.Q.update_spring_gg()
        self.Q.zero_spring_gg()
        self.cob_moved = [0.0, 0.0, 0.0]
        
    def commit(self):
        res = self.Q.sync_to_device()
        print "sync to dev: {}".format(res)

    def move_to_lift(self, index, margin, stepcount):
        # move to a support pattern that allows lifting of index
        l1 = (index - 1)%4
        l2 = (index + 1)%4
        xyz = self.Q.find_vector_to_diagonal(l1, l2)
        distance = xyz[2]
        print "distance to diagonal: ", distance
        stable = xyz[3]
        if stable == index:
            if distance >= margin*0.9:
                # selected leg is stable
                print "move to lift: already there"
                return True
            else:
                print "moving to lift leg ", index
                angle = math.atan2(xyz[1], xyz[0])
                if not self.move_body_in_steps(
                    math.cos(angle) * (margin - distance),
                    math.sin(angle) * (margin - distance),
                    0, stepcount):
                    print "leg ", index, " can't be lifted now"
                    return False
                else:
                    return True
        else:
            print "moving to lift leg ", index, " (opposite dir)"
            angle = math.atan2(xyz[1], xyz[0]) + math.pi
            if not self.move_body_in_steps(
                math.cos(angle) * (distance + margin),
                math.sin(angle) * (distance + margin),
                0, stepcount):
                print "leg ", index, " can't be lifted now"
                return False
            else:
                return True
          
    def move_body_in_steps(self, x, y, z, stepcount):
        if stepcount > 20: stepcount = 20
        dx = float(x) / stepcount
        dy = float(y) / stepcount
        dz = float(z) / stepcount
        if self.Q.change_all_feet_pos(x, y, z):
            self.Q.change_all_feet_pos(-x, -y, -z)
        else:
          print "move body in steps: end position unreachable"
          return False
        for i in range(stepcount):
            self.Q.change_all_feet_pos(dx, dy, dz)
            self.commit()
            time.sleep(0.05)
        self.cob_moved[0] -= x;
        self.cob_moved[1] -= y;
        self.cob_moved[2] -= z;
        return True
    
    ###############
    # stable_turn #
    ###############
    def stable_turn(self, angle):
        # 'turn' a leg = move leg to rest pos for a rotated body
        turning_leg = -1
        try:
            a = angle * len(self.to_turn)/4.0
            print "a ", a
            # if to_turn is empty, refill and move body to center
            if len(self.to_turn) == 0:
                self.move_body_in_steps(self.cob_moved[0], self.cob_moved[1],
                    self.cob_moved[2], 1)
                del self.to_turn
                return True
            # try to move to support any leg in to_turn
            for i in self.to_turn:
                if self.move_to_lift(i, 2, 5):
                    self.to_turn.remove(i) # remove from que
                    turning_leg = i
                    self.put_on_queue()
                    break
            if turning_leg == -1:
                print "failed to move any leg in", self.to_turn
                return False
        except AttributeError:
            self.to_turn = range(4)
            self.cob_moved[0] = 0.0
            self.cob_moved[1] = 0.0
            self.cob_moved[2] = 0.0
            if angle < 0: self.to_turn.reverse()
            return self.stable_turn(angle)
        # we're in a position to lift turning_leg
        time.sleep(0.1)
        rest_vector = self.Q.get_foot_rest_vector_delta(turning_leg, 2, a)
        if not self.transfer_leg(turning_leg,
            rest_vector[0] - self.cob_moved[0],
            rest_vector[1] - self.cob_moved[1]):
            # failed to transfer leg
            print "failed to transfer leg ", turning_leg
            self.to_turn.append(turning_leg) # re add to turning que
            return False
        # leg has been moved, time to rotate the body
        if not self.Q.change_body_rotation(2, angle / 4.0):
            # body rotation failed, but leg was moved
            print "body rotation failed, but leg ", turning_leg, " was moved"
            return False
        return True
        
    def old_cycle(self):
        self.Q.update_spring_gg()
        self.sgg_leg = self.Q.get_leg_with_highest_force(math.pi/2)
        print "HF leg (0-based): {}".format(self.sgg_leg)
        if 0 <= self.sgg_leg <= 3:
            #don't bother with contralateral front
            if (self.sgg_leg == 1 and self.sgg_last == 0) or (
            self.sgg_leg == 0 and self.sgg_last == 1): return
            self.sgg_liftable = self.Q.can_lift_leg(self.sgg_leg, 1.0)
            print "leg {} can be lifted? {}".format(
                self.sgg_leg, self.sgg_liftable)
        else:
            print "sgg_leg = {}".format(self.sgg_leg)
            return
        if not self.sgg_liftable: return
        F = 10
        if self.sgg_leg == 0 or self.sgg_leg == 1: F = 13
        transferred = False
        while not transferred:
            self.sgg_target = self.Q.get_last_spring_gg_vector(
                self.sgg_leg, math.pi/2, F)
            print "sgg target {}, {}, {}".format(
                self.sgg_target[0], self.sgg_target[1],
                self.sgg_target[2])
            if self.sgg_target[2] <> 0.0:
                self.sgg_target = None
                print "sgg failed to find target"
                return
            if self.sgg_target[2] == 0.0:
                print "changing foot {} by {}, {}".format(
                    self.sgg_leg, self.sgg_target[0], self.sgg_target[1])
                if self.transfer_leg(self.sgg_leg, self.sgg_target[0],
                self.sgg_target[1]): 
                    self.sgg_last = self.sgg_leg
                    return
                else:
                    F = F - 1
                    
        else:
            print "leg {} {}".format(self.sgg_leg, self.sgg_liftable)
  
    #############
    # NEW CYCLE #
    #############
    def cycle(self, angle, F):
        # try to transfer the leg with the highest force in specified
        # direction, to a position where it finds force F
        # (this means that to move a leg forward, you want negative F)
        self.Q.update_spring_gg()
        A = self.Q.get_leg_with_highest_force(angle)
        if self.Q.can_lift_leg(A, 1.0):
            v = self.Q.get_last_spring_gg_vector(A, angle, F)
            if v[2] <> 0.0:
                # failed
                print "cycle: couldn't find target for leg ", A
                return False
            else:
                print "cycle: transfer leg ", A, " by ", v[0], v[1]
                return self.transfer_leg(A, v[0], v[1]);
        else:
            print "cycle: couldn't lift leg ", A
            
    def stable_up(self, leg):
        self.Q.change_foot_pos((leg+2)%4, 0, 0, 0.60, 0)
        self.Q.change_foot_pos((leg+3)%4, 0, 0, -0.20, 0)
        self.Q.change_foot_pos((leg+1)%4, 0, 0, -0.20, 0)
        self.commit();
        self.Q.change_foot_pos(leg, 0, 0, 1.7, 0)
        self.commit();

    def stable_down(self, leg):
        self.Q.change_foot_pos((leg+2)%4, 0, 0, -0.60, 0)
        self.Q.change_foot_pos((leg+3)%4, 0, 0, 0.20, 0)
        self.Q.change_foot_pos((leg+1)%4, 0, 0, 0.20, 0)
        self.commit();
        self.Q.change_foot_pos(leg, 0, 0, -1.7, 0)
        self.commit();

        
    def transfer_leg(self, leg, x, y):
        print "leg {}: {}, {}".format(leg,x,y)
        t = 0.3
        #first check each point
        L1 = self.Q.change_foot_pos(leg, 0, 0 ,1.7, 0)
        print L1
        L2 = self.Q.change_foot_pos(leg, x, y, 0, 0)
        print L2
        if not L2:
            self.Q.change_foot_pos(leg, 0, 0, -1.7, 0)
            return False
        L3 = self.Q.change_foot_pos(leg, 0, 0, -1.7, 0)
        print L3
        if not L3:
            self.Q.change_foot_pos(leg, -x, -y, -1.7, 0)
            return False
        if L1 and L2 and L3:
            self.Q.change_foot_pos(leg, -x, -y, 0, 0)
            self.stable_up(leg)
            time.sleep(t)
            self.Q.change_foot_pos(leg, x, y, 0, 0)
            self.commit()
            time.sleep(t)
            self.stable_down(leg)
            time.sleep(t)
            return True
        else:
            print "transfer leg {} failed: {}, {}".format(leg, x, y)
            return False

    def transfer_leg_to_rest(self, leg):
      H = self.Q.get_relative_hmatrix(leg, 3)
      dx = self.rest_distance # or something


      
    def body_rotation_seq(self, t, inverted):
        self.startpos()
        sign = 1.0
        if inverted:
            sign = -1.0
        for i in range(10):
            xrot = math.sin(i/10.0 * 0.5 * math.pi) * 0.15 * sign
            self.Q.set_body_rotation(xrot, 0, 0)
            self.commit()
            time.sleep(t)
        count = 25 #don't increase above 45
        for i in range(count):
            xrot = math.cos(i/float(count) * 2 * math.pi) * 0.15 * sign
            yrot = math.sin(i/float(count) * 2 * math.pi) * 0.15 * sign
            print "start rot ", i
            if self.Q.set_body_rotation(xrot, yrot, 0):
                self.commit()
                #self.put_on_queue()
                time.sleep(t)
                if i % 10 == 0:
                    self.put_on_queue()
            else: 
                self.commit()
                #break;
        for i in range(10):
            xrot = math.cos(i/10.0 * 0.5 * math.pi) * 0.15 * sign
            self.Q.set_body_rotation(xrot, 0, 0)
            self.commit()
            time.sleep(t)
        self.startpos()
        self.commit()

    def body_circle(self, t, R):
        self.startpos()
        for i in range(10):
            print self.Q.change_all_feet_pos(R/10.0, 0, 0)
            self.commit()
            time.sleep(t)
        for i in range(45):
            dx = -math.sin(i/44.0 * 2 * math.pi)* R * (2*math.pi)/45
            dy = math.cos(i/44.0 * 2 * math.pi) * R * (2*math.pi)/45
            print self.Q.change_all_feet_pos(dx, dy, 0)
            self.commit()
            time.sleep(t)
            if i % 5: self.put_on_queue()

    def apply_config(self):
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
                try:
                    self.Q.set_pivot_pulsewidth_config(leg.id, pivot.id, 
                        pivot.pw_0, pivot.pw_60)
                except AttributeError:
                    pass
        self.Q.update_spring_gg()
        # rest vector are not updated at any time
        d = 3.8 + 7.35 - 1
        h = 11
        self.Q.set_foot_rest_vector(0, d, d+2, -h);
        self.Q.set_foot_rest_vector(1, -d, d+2, -h);
        self.Q.set_foot_rest_vector(2, -d, -d-2, -h);
        self.Q.set_foot_rest_vector(3, d, -d-2, -h);
        
