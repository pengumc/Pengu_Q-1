# actions to perform for each keypress
import math
import time

connected = False
leg = 0
pivot = 0
cob_selected = True
cob_moved = [0.0, 0.0, 0.0]
speed = 0.3
high_force = True

def act_on_key(char, libthread):
    global connected, leg, pivot, cob_selected, cob_moved, speed, high_force
    if char == 'q': # ---------------------------------------------------- QUIT
        print "'q': quit"
    elif char == 'c': # ----------------------------------------------- CONNECT
        libthread.thread_call(libthread.Q.connect, 0x16c0, 0x5df)
        result = libthread.qout.get()
        if result == 0:
            print "'c': connected to robot\n"
            connected = True
            sync_from_device(libthread)
        else:
            connected = False
            print "'c': could not connect to device\n"
    elif char == '1': # ----------------------------------------- LEG SELECTION
        leg = 0
        cob_selected = False
        print "'1': leg 0 selected\n"
    elif char == '2':
        leg = 1
        cob_selected = False
        print "'2': leg 1 selected\n"
    elif char == '3':
        leg = 2
        cob_selected = False
        print "'3': leg 2 selected\n"
    elif char == '4':
        leg = 3
        cob_selected = False
        print "'4': leg 3 selected\n"
    elif char == '2':
        leg = 1
        cob_selected = False
        print "'2': leg 1 selected\n"
    elif char == '!': # --------------------------------------- PIVOT SELECTION
        pivot = 0
        cob_selected = False
        print "'!': pivot 0 selected\n"
    elif char == '@':
        pivot = 1
        cob_selected = False
        print "'@': pivot 1 selected\n"
    elif char == '#':
        pivot = 2
        cob_selected = False
        print "'#': pivot 2 selected\n"
    elif char == '$':
        pivot = 3
        cob_selected = False
        print "'$': pivot 3 selected\n"
    elif char == '0': # ------------------------------------------COB SELECTION
        cob_selected = True
        print "'0': CoB selected\n"
    elif char == 'r': # ----------------------------------------- REST POSITION
        print "'r': rest position\n"
        restpos(libthread)
        print ""
    elif char == '`': # -------------------------------------- SYNC FROM DEVICE
        print "'`': sync from device"
        sync_from_device(libthread)
        print ""
    elif char == '~': # ---------------------------------------- SYNC TO DEVICE
        print "'~': sync to device"
        commit(libthread)
        print ""
    elif char == 'm': # --------------------------------------------- FOOT DOWN
        print "'m': selected foot down"
        stable_down(libthread, leg)
        print ""
    elif char == 'n': # ----------------------------------------------- FOOT UP
        print "'n': selected foot up"
        stable_up(libthread, leg)
        print ""
    elif char == 'w': # ----------------------------------------- FORWARDS (-Y)
        # move selection
        if cob_selected:
            print "'w': body Y+", speed 
            libthread.thread_call(libthread.Q.change_all_feet_pos, 0, -speed, 0)
            result = libthread.qout.get()
            if result:
                cob_moved[1] += speed
            else:
                print " movement failed"
        else:
            print "'w': foot ", leg, ", Y+", speed 
            libthread.thread_call(libthread.Q.change_foot_pos,
                                  leg, speed, 0, 0, 0)
        commit(libthread)
        print ""
    elif char == 'W':
        # capital W, walk
        print "'W': walk in Y dir"
        F = -7
        if high_force: F = -11.5
        if cycle(libthread, math.pi/2, -11.5):
            # if we could move a leg, we're done, toggle high_force
            high_force = not high_force
        else:
            # no leg transfer, just move forwards then
            libthread.thread_call(libthread.Q.change_all_feet_pos, 
                                  0, -speed, 0)
            result = libthread.qout.get()
            if result:
                cob_moved[1] += speed
        commit(libthread)
        print ""
    elif char == 's': # ---------------------------------------- BACKWARDS (+Y)
        # move selection
        if cob_selected:
            print "'s': body Y-", speed 
            libthread.thread_call(libthread.Q.change_all_feet_pos, 0, speed, 0)
            result = libthread.qout.get()
            if result:
                cob_moved[1] -= speed
            else:
                print " movement failed"
        else:
            print "'s': foot ", leg, ", Y-", speed 
            libthread.thread_call(libthread.Q.change_foot_pos,
                                  leg, -speed, 0, 0, 0)
        commit(libthread)
        print ""
    elif char == 'S':
        # capital S, walk backwards
        print "'S': walk in Y dir (backwards)"
        F = -7
        if high_force: F = -11.5
        if cycle(libthread, -math.pi/2, -11.5):
            # if we could move a leg, we're done, toggle high_force
            high_force = not high_force
        else:
            # no leg transfer, just move forwards then
            libthread.thread_call(libthread.Q.change_all_feet_pos, 
                                  0, speed, 0)
            result = libthread.qout.get()
            if result:
                cob_moved[1] -= speed
        commit(libthread)
        print ""
        
        
def sync_from_device(libthread):
    libthread.thread_call(libthread.Q.sync_from_device)
    result = libthread.qout.get()
    if result == False:
        print "sync from device failed"

def commit(libthread):
    libthread.thread_call(libthread.Q.sync_to_device)
    result = libthread.qout.get()
    if result == False:
        print "sync to device failed"
        
def restpos(libthread):
    libthread.thread_call(libthread.Q.reset_body)
    libthread.qout.get()
    for i in range(4):
        libthread.thread_call(libthread.Q.get_foot_rest_vector, i, 0, 0)
        v = libthread.qout.get()
        print v[0], v[1], v[2]
        libthread.thread_call(libthread.Q.set_foot_pos, i, v[0], v[1], v[2])
        result = libthread.qout.get()
        print "set_foot_pos: ", result
    libthread.thread_call(libthread.Q.update_spring_gg)
    libthread.qout.get()
    libthread.thread_call(libthread.Q.zero_spring_gg)
    libthread.qout.get()
    cob_moved = [0.0, 0.0, 0.0]

def cycle(libthread, angle, force):
    libthread.thread_call(libthread.Q.update_spring_gg)
    libthread.qout.get()
    libthread.thread_call(libthread.Q.get_leg_with_highest_force, angle)
    A = libthread.qout.get()
    libthread.thread_call(libthread.Q.can_lift_leg, A, 1.0)
    liftable = libthread.qout.get()
    if liftable:
        libthread.thread_call(libthread.Q.get_last_spring_gg_vector, 
                              A, angle, force)
        v = libthread.qout.get()
        if v[2] <> 0.0:
            # failed
            print " cycle couldn't find target for leg ", A
            return False
        else:
            print " transferring leg ", A, "by ", v[0], v[1]
            return transfer_leg(libthread, A, v[0], v[1])
    else:
        print " cycle couldn't lift leg ", A
        return False
    
def transfer_leg(libthread, leg, x, y):
    t = 0.3
    dz = 1.7
    # lift
    libthread.thread_call(libthread.Q.change_foot_pos, leg, 0, 0, dz, 0)
    L1 = libthread.qout.get()
    # move
    libthread.thread_call(libthread.Q.change_foot_pos, leg, x, y, 0, 0)
    L2 = libthread.qout.get()
    if not L2:
        # rollback
        libthread.thread_call(libthread.Q.change_foot_pos, leg, 0, 0, -dz, 0)
        libthread.qout.get()
        print "  transfer L2 failed for leg ", leg, "(", x, y, ")"
        return False
    # land
    libthread.thread_call(libthread.Q.change_foot_pos, leg, 0, 0, -dz, 0)
    L3 = libthread.qout.get()
    if not L3:
        # rollback
        libthread.thread_call(libthread.Q.change_foot_pos, leg, -x, -y, -dz, 0)
        libthread.qout.get()
        print "  transfer L3 failed for leg ", leg, "(", x, y, ")"
    if L1 and L2 and L3:
        libthread.thread_call(libthread.Q.change_foot_pos, leg, -x, -y, 0, 0)
        libthread.qout.get()
        stable_up(libthread, leg)
        time.sleep(t)
        libthread.thread_call(libthread.Q.change_foot_pos, leg, x, y, 0, 0)
        libthread.qout.get()
        commit(libthread)
        time.sleep(t)
        stable_down(libthread, leg)
        time.sleep(t)
        return True

def stable_up(libthread, leg):
        libthread.thread_call(libthread.Q.change_foot_pos, 
                              (leg+2)%4, 0, 0, 0.6, 0)
        libthread.thread_call(libthread.Q.change_foot_pos, 
                              (leg+3)%4, 0, 0, -0.2, 0)
        libthread.thread_call(libthread.Q.change_foot_pos, 
                              (leg+1)%4, 0, 0, -0.2, 0)
        for i in range(3): libthread.qout.get()
        commit(libthread)
        libthread.thread_call(libthread.Q.change_foot_pos, 
                              leg, 0, 0, 1.7, 0)
        libthread.qout.get()
        commit(libthread)

def stable_down(libthread, leg):
        libthread.thread_call(libthread.Q.change_foot_pos, 
                              (leg+2)%4, 0, 0, -0.6, 0)
        libthread.thread_call(libthread.Q.change_foot_pos, 
                              (leg+3)%4, 0, 0, 0.2, 0)
        libthread.thread_call(libthread.Q.change_foot_pos, 
                              (leg+1)%4, 0, 0, 0.2, 0)
        for i in range(3): libthread.qout.get()
        commit(libthread)
        libthread.thread_call(libthread.Q.change_foot_pos, 
                              leg, 0, 0, -1.7, 0)
        libthread.qout.get()
        commit(libthread)
                