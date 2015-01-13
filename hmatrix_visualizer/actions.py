# actions to perform for each keypress
import math
import time
import plotthread

connected = False
leg = 0
pivot = 0
cob_selected = True
cob_moved = [0.0, 0.0, 0.0]
speed = 0.3
to_turn = range(4)
walk_counter = 0
high_force = True
plot_thread = None

def act_on_key(char, libthread):
    global connected, leg, pivot, cob_selected, cob_moved, speed, high_force
    global plot_thread, walk_counter, to_turn
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
    elif char == 'p': # ---------------------------------------- START PLOTTING
        # connect plot thread to the visualization queue
        try:
            if plot_thread.is_alive():
                print "'p': plot thread is already running"
            else:
                print "'p': plot thread is no longer running"
                raise AttributeError
        except AttributeError:
            print "'p': starting plot thread"
            plot_thread = plotthread.PlotThread(libthread.qvis)
            plot_thread.start()
            libthread.thread_call(libthread.queue_visual_data)
            libthread.qout.get()
        print ""
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
        high_force = True
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
            libthread.thread_call(libthread.Q.change_all_feet_pos,
                                  0, -speed, 0)
            result = libthread.qout.get()
            if result:
                cob_moved[1] += speed
            else:
                print " movement failed"
        else:
            print "'w': foot ", leg, ", Y+", speed 
            libthread.thread_call(libthread.Q.change_foot_pos,
                                  leg, 0, speed, 0, 0)
            libthread.qout.get()
        commit(libthread)
        print ""
    elif char == 'W':
        # capital W, walk
        print "'W': walk in Y dir"
        F = -7
        if high_force: F = -11.5
        print "F = ", F
        if cycle(libthread, math.pi/2, F):
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
        print "walk counter ", walk_counter
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
                                  leg, 0, -speed, 0, 0)
            libthread.qout.get()
        commit(libthread)
        print ""
    elif char == 'S':
        # capital S, walk backwards
        print "'S': walk in Y dir (backwards)"
        F = -7
        if high_force: F = -11.5
        if cycle(libthread, -math.pi/2, F):
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
    elif char == 'd': # ----------------------------------------- SIDEWAYS (+X)
        # move selection
        if cob_selected:
            print "'d': body X+", speed 
            libthread.thread_call(libthread.Q.change_all_feet_pos,
                                  -speed, 0, 0)
            result = libthread.qout.get()
            if result:
                cob_moved[0] += speed
            else:
                print " movement failed"
        else:
            print "'w': foot ", leg, ", X+", speed 
            libthread.thread_call(libthread.Q.change_foot_pos,
                                  leg, speed, 0, 0, 0)
            libthread.qout.get()
        commit(libthread)
        print ""
    elif char == 'D':
        # capital D, walk
        print "'D': walk in X dir"
        F = -7
        if high_force: F = -10
        if cycle(libthread, 0, F):
            # if we could move a leg, we're done, toggle high_force
            high_force = not high_force
        else:
            # no leg transfer, just move forwards then
            libthread.thread_call(libthread.Q.change_all_feet_pos, 
                                  -speed, 0, 0)
            result = libthread.qout.get()
            if result:
                cob_moved[0] += speed
        commit(libthread)
        print ""
    elif char == 'a': # ----------------------------------------- SIDEWAYS (-X)
        # move selection
        if cob_selected:
            print "'a': body X-", speed 
            libthread.thread_call(libthread.Q.change_all_feet_pos, speed, 0, 0)
            result = libthread.qout.get()
            if result:
                cob_moved[0] -= speed
            else:
                print " movement failed"
        else:
            print "'a': foot ", leg, ", X-", speed 
            libthread.thread_call(libthread.Q.change_foot_pos,
                                  leg, -speed, 0, 0, 0)
            libthread.qout.get()
        commit(libthread)
        print ""
    elif char == 'A':
        # capital A, walk
        print "'A': walk in X dir (backwards)"
        F = -7
        if high_force: F = -10
        if cycle(libthread, math.pi, F):
            # if we could move a leg, we're done, toggle high_force
            high_force = not high_force
        else:
            # no leg transfer, just move forwards then
            libthread.thread_call(libthread.Q.change_all_feet_pos, 
                                  speed, 0, 0)
            result = libthread.qout.get()
            if result:
                cob_moved[0] -= speed
        commit(libthread)
        print ""
    elif char == 'k': # ---------------------------------------------- TURNING
        print "'k': rebalance"
        walk_counter = 0
        to_turn = None
        high_force = True
        for i in range(5): stable_turn(libthread, 0)
        print ""
    elif char == 'y':
        print "'y': turning 0.3 radians"
        to_turn = None
        high_force = True
        for i in range(5): stable_turn(libthread, 0.3)
        print ""
    elif char == 'Y':
        print "'Y': turning -0.3 radians"
        to_turn = None
        high_force = True
        for i in range(5): stable_turn(libthread, -0.3)
        print ""
    elif char == '{': # ----------------------------------------- BODY ROTATION
        print "'{': rotate body around Y axis (+)"
        libthread.thread_call(libthread.Q.change_body_rotation, 1, 0.1*speed)
        if not libthread.qout.get():
            print " rotation failed"
        print ""
    elif char == '}': # -Y
        print "'}': rotate body around Y axis (-)"
        libthread.thread_call(libthread.Q.change_body_rotation, 1, -0.1*speed)
        if not libthread.qout.get():
            print " rotation failed"
        print ""
    elif char == '[': # +Z
        print "'[': rotate body around Z axis (+)"
        libthread.thread_call(libthread.Q.change_body_rotation, 2, 0.1*speed)
        if not libthread.qout.get():
            print " rotation failed"
        print ""
    elif char == ']': # -Z
        print "']': rotate body around Z axis (-)"
        libthread.thread_call(libthread.Q.change_body_rotation, 2, -0.1*speed)
        if not libthread.qout.get():
            print " rotation failed"
        print ""
    elif char == ';': # +X
        print "';': rotate body around X axis (+)"
        libthread.thread_call(libthread.Q.change_body_rotation, 0, 0.1*speed)
        if not libthread.qout.get():
            print " rotation failed"
        print ""
    elif char == "'": # -X
        print "''': rotate body around X axis (-)"
        libthread.thread_call(libthread.Q.change_body_rotation, 0, -0.1*speed)
        if not libthread.qout.get():
            print " rotation failed"
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
    global walk_counter
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
            if transfer_leg(libthread, A, v[0], v[1]):
                walk_counter = walk_counter + 1
                libthread.thread_call(libthread.Q.equalize_feet_levels, -11)
                libthread.qout.get()
                return True
            else:
                return False
    else:
        print " cycle couldn't lift leg ", A
        return False
    
def transfer_leg(libthread, leg, x, y):
    print "legtransfer ", leg, x, y
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
    # z_main = 1.7
    # z_side = 0.2
    # z_opposite = 0.6
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
                
def stable_turn(libthread, angle):
    global to_turn, cob_moved
    # 'turn' a leg = move leg to rest pos for a rotated body
    turning_leg = -1
    if to_turn <> None:
        a = angle * len(to_turn)/4.0
        print "a ", a
        # if to_turn is empty, refill and move body to center
        if len(to_turn) == 0:
            move_body_in_steps(libthread, cob_moved[0], cob_moved[1],
                               cob_moved[2], 1)
            to_turn = None
            return True
        # try to move to support any leg in to_turn
        for i in to_turn:
            if move_to_lift(libthread, i, 2, 5):
                to_turn.remove(i) # remove from que
                turning_leg = i
                break
        if turning_leg == -1:
            print "failed to move any leg in", to_turn
            return False
    else:
        to_turn = range(4)
        cob_moved[0] = 0.0
        cob_moved[1] = 0.0
        cob_moved[2] = 0.0
        if angle < 0: to_turn.reverse()
        return stable_turn(libthread, angle)
    # we're in a position to lift turning_leg
    time.sleep(0.1)
    libthread.thread_call(libthread.Q.get_foot_rest_vector_delta,
                          turning_leg, 2, a)
    rest_vector = libthread.qout.get()
    if not transfer_leg(libthread, turning_leg,
        rest_vector[0] - cob_moved[0],
        rest_vector[1] - cob_moved[1]):
        # failed to transfer leg
        print "failed to transfer leg ", turning_leg
        to_turn.append(turning_leg) # re add to turning que
        return False
    # leg has been moved, time to rotate the body
    libthread.thread_call(libthread.Q.change_body_rotation, 2, angle/4.0)
    if not libthread.qout.get():
        # body rotation failed, but leg was moved
        print "body rotation failed, but leg ", turning_leg, " was moved"
        return False
    return True

def move_body_in_steps(libthread, x, y, z, stepcount):
    global cob_moved
    dx = float(x) / stepcount
    dy = float(y) / stepcount
    dz = float(z) / stepcount
    # check if final position is reachable
    libthread.thread_call(libthread.Q.change_all_feet_pos, x, y, z)
    if libthread.qout.get():
        libthread.thread_call(libthread.Q.change_all_feet_pos, -x, -y, -z)
        libthread.qout.get()
    else:
        print "move body in steps: end position unreachable"
        return False
    for i in range(stepcount):
        libthread.thread_call(libthread.Q.change_all_feet_pos, dx, dy, dz)
        libthread.qout.get()
        commit(libthread)
        time.sleep(0.05)
    cob_moved[0] -= x;
    cob_moved[1] -= y;
    cob_moved[2] -= z;
    return True

def move_to_lift(libthread, index, margin, stepcount):
    # move to a support pattern that allows lifting of index
    L1 = (index - 1)%4
    L2 = (index + 1)%4
    libthread.thread_call(libthread.Q.find_vector_to_diagonal, L1, L2)
    xyz = libthread.qout.get()
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
            if not move_body_in_steps(libthread, 
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
        if not move_body_in_steps(libthread,
            math.cos(angle) * (distance + margin),
            math.sin(angle) * (distance + margin),
            0, stepcount):
            print "leg ", index, " can't be lifted now"
            return False
        else:
            return True
