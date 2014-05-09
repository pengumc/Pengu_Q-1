import threading
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
def np_conv_hmatrix16(ar16):
    return np.array([ar16[0:4], ar16[4:8], ar16[8:12], ar16[12:16]])


#one thread, takes collection of hmatrices as input and does plt.show
class PlotThread (threading.Thread):

    def __init__(self, hmatrices):
        threading.Thread.__init__(self)
        self.hmatrices = hmatrices
        
    def run(self):
        from mpl_toolkits.mplot3d import Axes3D
        import matplotlib.pyplot as plt
        self.fig = plt.figure(1)
        self.ax = self.fig.gca(projection='3d')
        self.ax.set_aspect("equal")
        self.ax.set_xlabel("X")
        self.ax.set_ylabel("Y")
        self.ax.set_zlabel("Z")
        for raw_H in self.hmatrices:
            H = np_conv_hmatrix16(raw_H)
            self.draw_hmatrix(H)
        self.ax.set_xlim((-10,10))
        self.ax.set_ylim((-10,10))
        self.ax.set_zlim((-10,10))
        plt.show()
        plt.close('all')
    
    def draw_hmatrix(self, hmatrix):
        #multiply each point with the hmatrix
        transformed_points = np.dot(hmatrix, BASE_CUBE_POINTS)
        transformed_points = np.transpose(transformed_points)
        #now delete our 4th column
        transformed_points = np.round(np.delete(transformed_points, 3, 1),2)
        self.add_cube(transformed_points)

    def add_cube(self, points):
        self.ax.scatter(*points[7], s=30, color="g")
        for s, e in combinations(points, 2):
            # combinations('ABCD', 2) --> AB AC AD BC BD CD
            #if np.sum(np.abs(s-e)) == r[1]-r[0]: #only take lines with length 2
            delta = s-e
            #magic 2.1
            if np.round(np.sqrt(delta[0]**2 + delta[1]**2 + delta[2]**2),2) <= 2.1:
                self.ax.plot3D(*zip(s,e), color="b")
