import socket
import threading
import Queue
import os

ss_quit_event = threading.Event()

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
                Client(c, self.q).start()
            except socket.timeout:
                if ss_quit_event.wait(0):
                    self.running = False
                    break
            except:
                self.running = False
                break
        self.s.close()
            
    def setup_socket(self):
        socket.setdefaulttimeout(1)
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.bind(('', self.port));
        self.s.listen(5)

#parse received data
#put connection and parsed req on queue as (c, req)
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
            if not data: break
            #create complete msg
            received = received + data
            if received.endswith('\r\n\r\n'):
                #received request
                req = parse_request(received)
                self.q.put((self.c, req))
                done = True
            
#take stuff from que and send responses
class SimpleHandler(threading.Thread):
    HTMLFILE1 = "web/webinterface1.html"
    HTMLFILE2 = "web/webinterface2.html"
    JQUERYFILE = "web/jquery-2.0.3.min.js"

    def __init__(self, q):
        threading.Thread.__init__(self)
        self.q = q

    def run(self):
        self.running = True
        while self.running:
            try:
                c, req = self.q.get(True, 0.01)
                data, mimetype = self.gen_data(req)
                send_data(c, data, mimetype)
                c.close()
            except Queue.Empty:
                pass
            #except 404, send_404, c.close
            except:
                self.running = False
                break
            if ss_quit_event.wait(0): self.running = False
    
    def gen_data(self, req):
        if req == '/quit':
            ss_quit_event.set()
        elif req == '/':
            return(self.gen_data_page(), "text/html")
        else:
            return('{"a":"43"}', "application/json")
            #raise 404
            
    def gen_data_page(self):
        f = open(self.HTMLFILE1)
        page = f.read()
        f.close()
        f = open(self.JQUERYFILE)
        page = page + f.read()
        f.close()
        f = open(self.HTMLFILE2)
        page = page + f.read()
        f.close()
        return(page)
            
S = SimpleWebServer(42426)
H = SimpleHandler(S.q)
    
if __name__ == "__main__":
    S.start()
    H.start()