import numpy as np
from itertools import product, combinations
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
import sys

SCALE = .1
BASE_CUBE_POINTS = SCALE * np.array(
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
        if np.round(np.sqrt(delta[0]**2 + delta[1]**2 + delta[2]**2),2) <= SCALE *2.1:
            ax.plot3D(*zip(s,e), color="b")

def draw_line(hs):
    x = []
    y = []
    z = []
    for h in hs:
        x.append(h[0][3])
        y.append(h[1][3])
        z.append(h[2][3])
    ax.plot(x, y, z)

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

def loadcsv_angles(filename, x, y, z):
  a = np.genfromtxt(filename, delimiter=',', skip_header=1)
  for row in a:
    if row[0] == float(x) and row[1] == float(y) and row[2] == float(z):
        return row[4], row[5], row[6]
  return np.nan, np.nan, np.nan
  
def hmatrix():
  return np.array([[1.0, 0, 0, 0], [0,1,0,0],[0,0,1,0],[0,0,0,1]])

def hrot(h, axis, angle):
    if axis == 'x':
        R = np.array([
            [1,0,0,0],
            [0,np.cos(angle), -np.sin(angle),0],
            [0, np.sin(angle), np.cos(angle),0],
            [0,0,0,1]])
    elif axis == 'y':
        R = np.array([
            [np.cos(angle),0,np.sin(angle),0],
            [0,1,0,0],
            [0,-np.sin(angle),np.cos(angle),0],
            [0,0,0,1]])
    elif axis == 'z':
        R = np.array([
            [np.cos(angle),-np.sin(angle),0,0],
            [np.sin(angle),np.cos(angle),0,0],
            [0,0,1,0],
            [0,0,0,1]])
    return np.dot(h, R)
      
  
if __name__ == "__main__":
    fig = plt.figure(1)
    ax = fig.gca(projection='3d')
    ax.set_aspect("equal")
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Z")
    
    if sys.argv[1] == "all":
        X,Y,Z = loadcsv("../quadruped/ikdata.csv")
        ax.scatter(X, Y, Z, c='r')
        plt.show()
    elif sys.argv[1] == "single":
        print(sys.argv)
        a0, a1, a2 = loadcsv_angles("../quadruped/ikdata.csv", sys.argv[2],
            sys.argv[3], sys.argv[4])
        if not np.isnan(a0):
            print("{} {} {}".format(a0, a1, a2))
            p0 = hrot(hmatrix(), 'z', np.pi/4*3)
            p0 = hrot(p0, 'z', a0)
            
            p1 = hmatrix()
            p1[0][3] = 0.2
            p1 = hrot(p1, 'x', np.pi/2.0)
            p1 = hrot(p1, 'z', a1)
            p1 = np.dot(p0, p1)
            
            p2 = hmatrix()
            p2[0][3] = 1
            p2 = hrot(p2, 'z', -np.pi/2.0)
            p2 = hrot(p2, 'z', a2)
            p2 = np.dot(p1, p2)
            
            p3 = hmatrix()
            p3[0][3] = 1
            p3 = np.dot(p2, p3)
            
            draw_hmatrix(p0)
            draw_hmatrix(p1)
            draw_hmatrix(p2)
            draw_hmatrix(p3)
            draw_line([p0,p1,p2,p3])
            ax.set_xlim((-2,2))
            ax.set_ylim((-2,2))
            ax.set_zlim((-2,2))
            plt.show()
            
