import hmatrixplot
from main import *

if __name__ == "__main__":
    kbt = KeyboardThread()
    kbt.start()
    while True:
        if kbt.is_alive():
            hmatrices = kbt.queue.get(True, None)
            while not kbt.queue.empty():
                kbt.queue.get(True, None)
            if hmatrices:
                pt = hmatrixplot.PlotThread(hmatrices)
                pt.run()
        else:
            break
        
    # Q = Quadruped("../quadruped/bin/quadruped.dll")
    # move x 5
    # Q.set_pivot_pos(0, 0, 5, 0, 0)
    #grab pivot00
    # raw_pivot00 = Q.get_hmatrix(0)
    # raw_pivot01 = Q.get_hmatrix(1)
    # pt = hmatrixplot.threading.Thread()
    # x = 1.0
    # while 1:

        # c = keyboard.getch()
        # if c == 'q':
            # if pt.is_alive():
                # print "Waiting for plot thread to die..."
                # pt.join()
            # break
        # elif c == 'p':
            # if pt.is_alive():
                # print("There's already a plot showing, close that one first")
            # else:
                # print("creating new thread")
                # pt = hmatrixplot.PlotThread([raw_pivot00, raw_pivot01])
                # print("running new thread")
                # pt.start()
        # elif c == 'a':
            # Q.set_pivot_pos(0, 0, x, 0, 0)
            # print("set x of pivot00 to {}".format(x))
#proposed:
#cmd prompt
# p to launch another thread with a plt.show of the current data
# q to quit
# a/d inc dec x        