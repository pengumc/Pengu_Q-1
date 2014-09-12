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
# setup 2d axis
ax_flat = fig.add_subplot(1, 2, 2)
ax_flat.set_xlim([-17,17])
ax_flat.set_ylim([-17,17])
ax_flat.set_xlabel("X")
ax_flat.set_ylabel("Y")
ax_flat.grid()
ax_flat_scatters = [ax_flat.scatter(0,0) for i in range(2)]
ax_flat_lines = [ax_flat.plot([],[],c="b")[0] for i in range(5)]

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

def on_click(event):
    global ax_flat
    if event.inaxes == ax_flat:
        print "x: {}, y: {}".format(event.xdata, event.ydata)
        #clear kbt xyq
        try:
            while True:
                kbt.xyq.get(False)
        except:
            #put (x,y) on there
            kbt.xyq.put((event.xdata, event.ydata))

fig.canvas.mpl_connect('button_press_event', on_click)

def np_conv_hmatrix16(ar16):
    return np.array([ar16[0:4], ar16[4:8], ar16[8:12], ar16[12:16]])

def data_gen():
    #get all data from queue, use last one as data
    try:
        queue_data = kbt.queue.get(False) # don't block, [throw empty exception]
        if queue_data:
            yield queue_data[:17], queue_data[17:]
        else:
            yield [], []
    except:
        yield [], []
    
def run_ani(data):
    #draw stuff
    i = 0
    raw_hmatrices, km_and_lasmbf = data
    for raw_H in raw_hmatrices:
        hmatrix = np_conv_hmatrix16(raw_H)
        #multiply each point with the hmatrix
        transformed_points = np.dot(hmatrix, BASE_CUBE_POINTS)
        transformed_points = np.transpose(transformed_points)
        #now delete our 4th column
        transformed_points = np.round(np.delete(transformed_points, 3, 1),2)
        add_cube(transformed_points, i)
        i = i + 1
    if len(raw_hmatrices) == 17 and len(km_and_lasmbf) == 6:
        plot_topdown(raw_hmatrices, km_and_lasmbf)

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
            
def plot_topdown(hmatrices, km_and_lasmbf):
    #plot xy of com
    global ax_flat_scatters
    ax_flat_scatters[0].remove()
    ax_flat_scatters[0] = ax_flat.scatter(
        hmatrices[16][3], hmatrices[16][7], s=200, c='r')
    #plot xy of cob (aka 0,0)
    ax_flat_scatters[1].remove()
    ax_flat_scatters[1] = ax_flat.scatter(0,0, s=100, c='g')
    #plot crosslines
    ax_flat_lines[0].set_data(
        [hmatrices[3][3], hmatrices[11][3]],
        [hmatrices[3][7], hmatrices[11][7]])
    ax_flat_lines[1].set_data(
        [hmatrices[7][3], hmatrices[15][3]],
        [hmatrices[7][7], hmatrices[15][7]])
    ax_flat_lines[2].set_data(
        [hmatrices[3][3], hmatrices[7][3],
        hmatrices[11][3], hmatrices[15][3],
        hmatrices[3][3]],
        [hmatrices[3][7], hmatrices[7][7],
        hmatrices[11][7], hmatrices[15][7],
        hmatrices[3][7]])
    #lasmbf lines (assuming v in x dir)
    ax_flat_lines[3].set_data([0, km_and_lasmbf[4]], [0,0])
    print km_and_lasmbf[4]
    ax_flat_lines[3].set_color("r")
    ax_flat_lines[4].set_data([0, km_and_lasmbf[5]], [0,0])
    ax_flat_lines[4].set_color("g")

            
if __name__ == "__main__":
    # start keyboard thread
    # TODO(michiel): give a more appropriate name to this thread
    kbt = KeyboardThread()
    kbt.start()
    a = ani.FuncAnimation(fig, run_ani, data_gen, blit=False, interval=300)
    plt.show()