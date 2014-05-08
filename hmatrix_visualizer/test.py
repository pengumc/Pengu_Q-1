from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np
from itertools import product, combinations

fig = plt.figure()
ax = fig.gca(projection='3d')
ax.set_aspect("equal")
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
#r = [-1, 1]
#points = np.array(list(product(r,r,r)))
#points = np.insert(points, 3, 1, axis=1) #add 1's to make 4d vectors

def draw_hmatrix(hmatrix):
    #multiply each point with the hmatrix
    transformed_points = np.dot(hmatrix, BASE_CUBE_POINTS)
    transformed_points = np.transpose(transformed_points)
    #now delete our 4th column
    transformed_points = np.round(np.delete(transformed_points, 3, 1),2)
    add_cube(transformed_points)

def add_cube(points):
    ax.scatter(*points[7], s=30, color="g")
    for s, e in combinations(points, 2):
        # combinations('ABCD', 2) --> AB AC AD BC BD CD
        #if np.sum(np.abs(s-e)) == r[1]-r[0]: #only take the lines with length 2
        delta = s-e
        if np.round(np.sqrt(delta[0]**2 + delta[1]**2 + delta[2]**2),2) == 2:
            ax.plot3D(*zip(s,e), color="b")


phi = 0.1
A = np.array(
    [[np.cos(phi), -np.sin(phi), 0, 0],
    [np.sin(phi), np.cos(phi), 0, 0],
    [0, 0, 1, 0],
    [0, 0, 0, 1]])
draw_hmatrix(A)
B = A
B[0][3] = 20.5
draw_hmatrix(B)
ax.set_xlim((-10,10))
ax.set_ylim((-10,10))
ax.set_zlim((-10,10))
ax.set_xlabel("X")
ax.set_ylabel("Y")
ax.set_zlabel("Z")
plt.show()
#plt.ion()
#plt.draw()
