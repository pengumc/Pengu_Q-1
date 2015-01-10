import threading
import Queue

# as seen on http://stackoverflow.com/questions/510357/
class _Getch:
    # Gets a single character from standard input.  Does not echo to the screen.
    def __init__(self):
        try:
            self.impl = _GetchWindows()
        except ImportError:
            self.impl = _GetchUnix()

    def __call__(self): return self.impl()


class _GetchUnix:
    def __init__(self):
        import tty, sys

    def __call__(self):
        import sys, tty, termios
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(sys.stdin.fileno())
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch


class _GetchWindows:
    def __init__(self):
        import msvcrt

    def __call__(self):
        import msvcrt
        return msvcrt.getch()

getch = _Getch()

# thread to put keyboard chars on a queue
class KeyboardThread (threading.Thread):

    def __init__(self, queue):
        threading.Thread.__init__(self)
        self.queue = queue
        self.running = False

    def run(self):
        self.running = True
        while self.running:
            c = getch()
            if c == 'q':
                self.running = False
            self.queue.put(c)
        print "keyboard thread dying..."
        

if __name__ == '__main__':
    print("press 'q' to exit")
    while getch() != 'q':
        print('that was not q')
