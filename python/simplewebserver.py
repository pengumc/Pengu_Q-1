import socket
import threading
import Queue
import os

def send_data(c, data, mimetype="application/json"):
    response = (
        'HTTP/1.0 200 OK\r\n' + 
        'Connection: close\r\n' + 
        'Vary: Accept-Encoding\r\n' +
        'Content-Type: ' + mimetype + '\r\n' +
        'Content-Length: ' + repr(len(data)) + '\r\n\r\n' +
        data
    )
    c.sendall(response)

def parse_request(text):
    #only look at first line
    first_linebreak = text.find('\r\n')
    if first_linebreak > 1 :
        print(text[0:first_linebreak])
        #now find the request between the 
        #first two spaces
        a = text.find(' ')
        b = text.find(' ', a + 1)
        return(text[a+1:b])
    return('')


#accept connections
#start client for those connections
class SimpleWebServer(threading.Thread):
    def __init__(self, port):
        threading.Thread.__init__(self)
        self.port = port
        self.q = Queue.Queue()
        self.running = False
        self.s = None
        
    def run(self):
        #accept and spawn clients
        self.setup_socket()
        self.running = True
        while self.running:
            try: 
                c, a = self.s.accept()
            except:
                self.running = False
                break
            Client(c, self.q).start()
            
    def setup_socket(self):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.bind(('', self.port));
        self.s.listen(5)

#parse received data
#put connection and parsed req on queue
class Client(threading.Thread):
    def __init__(self, c, q):
        threading.Thread.__init__(self)
        self.c = c
        self.q = q
        
    def run(self):
        #recv and parse
        done = False
        data = ""
        received = ""
        req = ""
        while not done:
            #read socket
            try:
                data = self.c.recv(1024)
            except:
                done = True
                break
            #create complete msg
            received = received + data
            if received.endswith('\r\n\r\n'):
                #received request
                req = parse_request(received)
                self.q.put((self.c, req))
                done = True
            
#take stuff from que and send responses
class SimpleHandler(threading.Thread):
    def __init__(self, q):
        threading.Thread.__init__(self)
        self.q = q

    def run(self):
        while 1:
            try:
                c, req = self.q.get()
            except:
                break
            send_data(c, "{'1':'42'}")
            c.close()
            if req == 'quit':
                os._exit(0)
            
S = SimpleWebServer(42426)
H = SimpleHandler(S.q)
    
if __name__ == "__main__":
    pass