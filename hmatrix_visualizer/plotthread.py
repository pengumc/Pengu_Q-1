import threading
import Queue
import numpy as np
from itertools import product, combinations

def np_conv_hmatrix16(ar16):
    return np.array([ar16[0:4], ar16[4:8], ar16[8:12], ar16[12:16]])


class PlotThread(threading.Thread):

    def __init__(self, queue):
        threading.Thread.__init__(self)
        self.queue = queue
    
    def run(self):
        import matplotlib.pyplot as plt
        from mpl_toolkits.mplot3d import Axes3D
        import matplotlib.animation as ani
        self.fig = plt.figure(figsize=plt.figaspect(.5))
        # setup 3d axis
        self.ax3d = self.fig.add_subplot(1, 2, 1, projection='3d')
        self.ax3d.set_xlabel("X")
        self.ax3d.set_ylabel("Y")
        self.ax3d.set_zlabel("Z")
        self.ax3d_lines = [
            self.ax3d.plot([1,1], [2,2], [3,3], c="b")[0] for i in range(17*12)
        ]
        self.ax3d_scatters = [self.ax3d.scatter(0,0) for i in range(17)]
        # setup 2d axis
        self.ax_flat = self.fig.add_subplot(1, 2, 2)
        self.ax_flat.set_xlim([-17,17])
        self.ax_flat.set_ylim([-17,17])
        self.ax_flat.set_xlabel("X")
        self.ax_flat.set_ylabel("Y")
        self.ax_flat.grid()
        self.ax_flat_scatters = [self.ax_flat.scatter(0,0) for i in range(2)]
        self.ax_flat_lines = [self.ax_flat.plot([],[],c="b")[0] for i in range(5)]
        self.BASE_CUBE_POINTS = np.array(
            [[-1, -1, -1],
            [-1, -1,  1],
            [-1,  1, -1],
            [-1,  1,  1],
            [ 1, -1, -1],
            [ 1, -1,  1],
            [ 1,  1, -1],
            [ 1,  1,  1]])
        self.BASE_CUBE_POINTS = np.transpose(np.insert(self.BASE_CUBE_POINTS,
                                                   3, 1, axis=1))
        a = ani.FuncAnimation(self.fig, self.runani, self.data_gen, blit=False, interval=300)
        plt.show()

    def data_gen(self):
        #get all data from queue, use last one as data
        try:
            queue_data = self.queue.get(False) # don't block, [throw empty exception]
            if queue_data:
                yield queue_data
            else:
                yield []
        except:
            yield []

    def runani(self, data):
        # draw stuff
        i = 0
        raw_hmatrices = data
        for raw_H in raw_hmatrices:
            hmatrix = np_conv_hmatrix16(raw_H)
            # multiply each point with the hmatrix
            transformed_points = np.dot(hmatrix, self.BASE_CUBE_POINTS)
            transformed_points = np.transpose(transformed_points)
            # now delete our 4th column
            transformed_points = np.round(np.delete(transformed_points, 3, 1),2)
            self.add_cube(transformed_points, i)
            i = i + 1
        if len(raw_hmatrices) == 17:
            self.plot_topdown(raw_hmatrices)
        self.ax3d.set_xlim((-20,20))
        self.ax3d.set_ylim((-20,20))
        self.ax3d.set_zlim((-20,20))
            

    def add_cube(self, points, i):
        self.ax3d_scatters[i].remove()
        self.ax3d_scatters[i] = self.ax3d.scatter(*points[7], s=10, color="r")
        index = 12*i
        for s, e in combinations(points, 2):
            # combinations('ABCD', 2) --> AB AC AD BC BD CD
            # if np.sum(np.abs(s-e)) == r[1]-r[0]: #only take lines with length 2
            delta = s-e
            # magic 2.1
            if np.round(np.sqrt(delta[0]**2 + delta[1]**2 + delta[2]**2),2) <= 2.1:
                # ax3d.plot3D(*zip(s,e), color="b")
                self.ax3d_lines[index].set_data(*zip(s[:2],e[:2]))
                self.ax3d_lines[index].set_3d_properties([s[2], e[2]])
                index = index + 1
            
    def plot_topdown(self, hmatrices):
        # plot xy of com
        self.ax_flat_scatters[0].remove()
        self.ax_flat_scatters[0] = self.ax_flat.scatter(
            hmatrices[16][3], hmatrices[16][7], s=200, c='r')
        # plot xy of cob (aka 0,0)
        self.ax_flat_scatters[1].remove()
        self.ax_flat_scatters[1] = self.ax_flat.scatter(0,0, s=30, c='g')
        # plot crosslines
        self.ax_flat_lines[0].set_data(
            [hmatrices[3][3], hmatrices[11][3]],
            [hmatrices[3][7], hmatrices[11][7]])
        self.ax_flat_lines[1].set_data(
            [hmatrices[7][3], hmatrices[15][3]],
            [hmatrices[7][7], hmatrices[15][7]])
        self.ax_flat_lines[2].set_data(
            [hmatrices[3][3], hmatrices[7][3],
            hmatrices[11][3], hmatrices[15][3],
            hmatrices[3][3]],
            [hmatrices[3][7], hmatrices[7][7],
            hmatrices[11][7], hmatrices[15][7],
            hmatrices[3][7]])
