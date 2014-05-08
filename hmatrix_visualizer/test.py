import threading
import Queue

main_queue = None

class A (threading.Thread):
    
    def __init__(self):
        threading.Thread.__init__(self)
        self.queue = Queue.Queue()
        global main_queue
        main_queue = self.queue
    
    def run(self):
        #do something with self.stuff
        import keyboard
        while 1:
            c = keyboard.getch()
            self.queue.put(c)
            if c == 'q':
                break

if __name__ == "__main__":
    t1 = A()
    t1.start()
    c = None
    while 1:
        try:
            c = main_queue.get(True, 0.1)
        except Queue.Empty:
            pass
        if c == 'q':
            break
        else:
            print c
            
