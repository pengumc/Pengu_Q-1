from main import *
import numpy as np
import time
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import matplotlib.animation as ani
from itertools import product, combinations

fig = plt.figure(figsize=plt.figaspect(.5))
# setup 3d axis
ax3d = fig.add_subplot(1, 2, 1, projection='3d')
ax3d.set_xlabel("X")
ax3d.set_ylabel("Y")
ax3d.set_zlabel("Z")
ax3d.set_xlim((-20,20))
ax3d.set_ylim((-20,20))
ax3d.set_zlim((-20,20))
ax3d_lines = [ax3d.plot([1,1], [2,2], [3,3], c="b")[0] for i in range(17*12)]
ax3d_scatters = [ax3d.scatter(0,0) for i in range(17)]

BASE_CUBE_POINTS = np.array(
    [[-1, -1, -1],
    [-1, -1,  1],
    [-1,  1, -1],
    [-1,  1,  1],
    [ 1, -1, -1],
    [ 1, -1,  1],
    [ 1,  1, -1],
    [ 1,  1,  1]])
BASE_CUBE_POINTS = np.transpose(np.insert(BASE_CUBE_POINTS, 3, 1, axis=1))

def np_conv_hmatrix16(ar16):
    return np.array([ar16[0:4], ar16[4:8], ar16[8:12], ar16[12:16]])

def data_gen():
    #get all data from queue, use last one as data
    try:
        queue_data = kbt.queue.get(False) # don't block, [throw empty exception]
        if queue_data:
            yield queue_data
        else:
            yield []
    except:
        yield []
    
def run_ani(data):
    #draw stuff
    i = 0
    for raw_H in data:
        hmatrix = np_conv_hmatrix16(raw_H)
        #multiply each point with the hmatrix
        transformed_points = np.dot(hmatrix, BASE_CUBE_POINTS)
        transformed_points = np.transpose(transformed_points)
        #now delete our 4th column
        transformed_points = np.round(np.delete(transformed_points, 3, 1),2)
        add_cube(transformed_points, i)
        i = i + 1
    time.sleep(0.5)

def add_cube(points, i):
    global ax3d_scatters, ax3d
    ax3d_scatters[i].remove()
    ax3d_scatters[i] = ax3d.scatter(*points[7], s=10, color="r")
    index = 12*i
    for s, e in combinations(points, 2):
        # combinations('ABCD', 2) --> AB AC AD BC BD CD
        #if np.sum(np.abs(s-e)) == r[1]-r[0]: #only take lines with length 2
        delta = s-e
        #magic 2.1
        if np.round(np.sqrt(delta[0]**2 + delta[1]**2 + delta[2]**2),2) <= 2.1:
            #ax3d.plot3D(*zip(s,e), color="b")
            ax3d_lines[index].set_data(*zip(s[:2],e[:2]))
            ax3d_lines[index].set_3d_properties([s[2], e[2]])
            index = index + 1
            
    
if __name__ == "__main__":
    # start keyboard thread
    # TODO(michiel): give a more appropriate name to this thread
    kbt = KeyboardThread()
    kbt.start()
    #main thread has pyplot
    #plot everything at 500ms intervals
    #data_gen can grab data from queue if there's any
    a = ani.FuncAnimation(fig, run_ani, data_gen, blit=False, interval=200)
    plt.show()