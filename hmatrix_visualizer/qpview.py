from quadruped import *
import keyboard
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np
from itertools import product, combinations

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

def draw_hmatrix(ax, hmatrix):
    #multiply each point with the hmatrix
    transformed_points = np.dot(hmatrix, BASE_CUBE_POINTS)
    transformed_points = np.transpose(transformed_points)
    #now delete our 4th column
    transformed_points = np.round(np.delete(transformed_points, 3, 1),2)
    add_cube(ax, transformed_points)

def add_cube(ax, points):
    ax.scatter(*points[7], s=30, color="g")
    for s, e in combinations(points, 2):
        # combinations('ABCD', 2) --> AB AC AD BC BD CD
        #if np.sum(np.abs(s-e)) == r[1]-r[0]: #only take the lines with length 2
        delta = s-e
        if np.round(np.sqrt(delta[0]**2 + delta[1]**2 + delta[2]**2),2) == 2:
            ax.plot3D(*zip(s,e), color="b")

def np_conv_hmatrix16(ar16):
    return np.array([ar16[0:4], ar16[4:8], ar16[8:12], ar16[12:16]])
            
if __name__ == "__main__":
    fig = plt.figure()
    ax = fig.gca(projection='3d')
    ax.set_aspect("equal")
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Z")

    Q = Quadruped("../quadruped/bin/quadruped.dll")
    # move x 5
    Q.set_pivot_pos(0, 0, 5, 0, 0)
    #grab pivot00
    raw_pivot00 = Q.get_hmatrix(0)
    pivot00 = np_conv_hmatrix16(raw_pivot00)
    #draw it
    draw_hmatrix(ax, pivot00)
    plt.show()
    x = 5.0
    while keyboard.getch() != 'q':
        x = x + 1
        Q.set_pivot_pos(0, 0, x, 0, 0)
        plt.cla()
        raw_pivot00 = Q.get_hmatrix(0)
        pivot00 = np_conv_hmatrix16(raw_pivot00)
        draw_hmatrix(ax, pivot00)
        plt.show()
#proposed:
#cmd prompt
# p to launch another thread with a plt.show of the current data
# q to quit
# a/d inc dec x        