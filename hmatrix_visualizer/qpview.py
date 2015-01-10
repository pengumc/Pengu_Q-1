import Queue
import librarythread
import keyboard
import actions
            
if __name__ == "__main__":
    # start library thread
    # lib_output holds the retval from calls to 'library_thread.thread_call'
    # each call will always add 1 item to the queue
    # the output queue will get a "ready" on it when initilization is done
    lib_output = Queue.Queue()
    library_thread = librarythread.LibraryThread(Queue.Queue(), lib_output,
                                                 Queue.Queue(), "config.xml")
    library_thread.start()


    
    
    # wait for  librarythread to initialize
    try:
        print lib_output.get(timeout=1)
        print ""
    except Queue.Empty:
        print "failed to initialize in a timely fashion"
        library_thread.thread_call(library_thread.die)

    # try to connect quadruped
    actions.act_on_key('c', library_thread)

    # listen to keyboard
    while True:
        c = keyboard.getch()
        actions.act_on_key(c, library_thread)
        if c == 'q':
            break;
        
    # at the end, kill the library thread
    library_thread.thread_call(library_thread.die)
