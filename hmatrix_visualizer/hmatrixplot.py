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
        self.fig = plt.figure(figsize=plt.figaspect(.5))
        self.ax = self.fig.add_subplot(1, 2, 1, projection='3d')
        self.ax.set_aspect("equal")
        self.ax.set_xlabel("X")
        self.ax.set_ylabel("Y")
        self.ax.set_zlabel("Z")
        for raw_H in self.hmatrices:
            H = np_conv_hmatrix16(raw_H)
            self.draw_hmatrix(H)
        self.ax.set_xlim((-20,20))
        self.ax.set_ylim((-20,20))
        self.ax.set_zlim((-20,20))
        ax_flat = self.fig.add_subplot(1, 2, 2)
        #assume all hmatrices are there
        #plot xy of feet
        feetx = [
            self.hmatrices[3][3], 
            self.hmatrices[7][3], 
            self.hmatrices[11][3], 
            self.hmatrices[15][3]]
        feety = [
            self.hmatrices[3][7], 
            self.hmatrices[7][7], 
            self.hmatrices[11][7], 
            self.hmatrices[15][7]]
        ax_flat.scatter(feetx, feety, s=40)
        ax_flat.text(feetx[0]+2, feety[0], '1')
        ax_flat.text(feetx[1]+2, feety[1], '2')
        ax_flat.text(feetx[2]+2, feety[2], '3')
        ax_flat.text(feetx[3]+2, feety[3], '4')
        #plot xy of com and cob
        ax_flat.scatter(
            self.hmatrices[16][3], self.hmatrices[16][7], s=200, c='r')
        ax_flat.scatter(0,0, s=100, c='g')
        #plot x and y line
        ax_flat.plot([-15, 15], [0, 0], c='r')
        ax_flat.plot([0,0],[-15, 15], c='r')
        #plot crossline
        ax_flat.plot(
            [self.hmatrices[3][3], self.hmatrices[11][3]],
            [self.hmatrices[3][7], self.hmatrices[11][7]], c="b")
        ax_flat.plot(
            [self.hmatrices[7][3], self.hmatrices[15][3]],
            [self.hmatrices[7][7], self.hmatrices[15][7]], c="b")
        ax_flat.plot([
            self.hmatrices[3][3], self.hmatrices[7][3],
            self.hmatrices[11][3], self.hmatrices[15][3],
            self.hmatrices[3][3]], [self.hmatrices[3][7], self.hmatrices[7][7],
            self.hmatrices[11][7], self.hmatrices[15][7],
            self.hmatrices[3][7]])

        #plot line of KM
        #plot line of LASMBF
        
        ax_flat.set_xlim([-17,17])
        ax_flat.set_ylim([-17,17])
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
        self.ax.scatter(*points[7], s=10, color="r")
        for s, e in combinations(points, 2):
            # combinations('ABCD', 2) --> AB AC AD BC BD CD
            #if np.sum(np.abs(s-e)) == r[1]-r[0]: #only take lines with length 2
            delta = s-e
            #magic 2.1
            if np.round(np.sqrt(delta[0]**2 + delta[1]**2 + delta[2]**2),2) <= 2.1:
                self.ax.plot3D(*zip(s,e), color="b")
