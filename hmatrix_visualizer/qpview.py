import hmatrixplot
from main import *

if __name__ == "__main__":
    kbt = KeyboardThread()
    kbt.start()
    while True:
        if kbt.is_alive():
            data = kbt.queue.get(True, None)
            while not kbt.queue.empty():
                kbt.queue.get(True, None)
            if data:
                pt = hmatrixplot.PlotThread(data)
                pt.run()
            else:
              break
        else:
            break
        
