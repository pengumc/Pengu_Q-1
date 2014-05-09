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
        
