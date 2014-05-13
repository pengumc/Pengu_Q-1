import numpy as np
from itertools import product, combinations
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm

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

def draw_hmatrix(hmatrix):
    #multiply each point with the hmatrix
    transformed_points = np.dot(hmatrix, BASE_CUBE_POINTS)
    transformed_points = np.transpose(transformed_points)
    #now delete our 4th column
    transformed_points = np.round(np.delete(transformed_points, 3, 1),2)
    add_cube(transformed_points)

def add_cube(points):
    ax.scatter(*points[7], s=10, color="r")
    for s, e in combinations(points, 2):
        # combinations('ABCD', 2) --> AB AC AD BC BD CD
        #if np.sum(np.abs(s-e)) == r[1]-r[0]: #only take lines with length 2
        delta = s-e
        #magic 2.1
        if np.round(np.sqrt(delta[0]**2 + delta[1]**2 + delta[2]**2),2) <= 2.1:
            ax.plot3D(*zip(s,e), color="b")

def loadcsv(filename):
    a = np.genfromtxt(filename, delimiter=',', skip_header=1)
    X = []
    Y = []
    Z = [];
    for row in a:
       X.append(row[0]-0.05)
       Y.append(row[1]-0.05)
       Z.append(row[3])
    return np.array(X), np.array(Y), np.array(Z)

if __name__ == "__main__":
    fig = plt.figure(1)
    ax = fig.gca(projection='3d')
    ax.set_aspect("equal")
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Z")
    
    X,Y,Z = loadcsv("../quadruped/ikdata.csv")
    # ax.bar3d(X, Y, np.zeros(len(X)), 0.1, 0.1, Z)
    ax.scatter(X, Y, Z, c='r')
    # A = np.array([[1.0, 0.0, 0.0, 0.0],[0,1,0,0],[0,0,1,0],[0,0,0,1]]);
    # draw_hmatrix(A)
    plt.show()
    
