import socket
import threading
import Queue
import os

ss_quit_event = threading.Event()
ss_queue = Queue.Queue()

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

def send_404(c):
    response =(
        'HTTP/1.0 404 Not Found\r\n' + 
        'Connection: close\r\n\r\n'
        'Content-Length: 0\r\n\r\n'
    )
    c.sendall(response)
    
    
def parse_request(text):
    #only look at first line
    first_linebreak = text.find('\r\n')
    if first_linebreak > 1 :
        #now find the request between the 
        #first two spaces
        a = text.find(' ')
        b = text.find(' ', a + 1)
        return(text[a+1:b])
    return('unknown')


class NotFound404(Exception):
    def __init__(self,value):
        self.value = value
    def __str__(self):
        return(repr(self.value))
    
#accept connections
#start client for those connections
class SimpleWebServer(threading.Thread):
    def __init__(self, port):
        threading.Thread.__init__(self)
        self.port = port
        self.running = False
        self.s = None
        
    def run(self):
        #accept and spawn clients
        self.setup_socket()
        self.running = True
        while self.running:
            try: 
                c, a = self.s.accept()
                Client(c).start()
            except socket.timeout:
                if ss_quit_event.wait(0):
                    self.running = False
                    break
            except Exception as e:
                print(e.value)
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
    def __init__(self, c):
        threading.Thread.__init__(self)
        self.c = c
        
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
                ss_queue.put((self.c, req))
                done = True
                return
        self.c.close()
#take stuff from que and send responses
class SimpleHandler(threading.Thread):
    HTMLFILE1 = "web/webinterface1.html"
    HTMLFILE2 = "web/webinterface2.html"
    JQUERYFILE = "web/jquery-2.0.3.min.js"

    def __init__(self):
        threading.Thread.__init__(self)

    def run(self):
        self.running = True
        while self.running:
            try:
                c, req = ss_queue.get(True, 1)
                print("handling: {}".format(req))
                data, mimetype = gen_data(req)
                send_data(c, data, mimetype)
                c.close()
            except Queue.Empty:
                continue
            except NotFound404:
                send_404(c)
                c.close()
                break
            except Exception as e:
                print(e)
                self.running = False
                c.close()
                break
            if ss_quit_event.wait(0): self.running = False
    
def gen_data(req):
    if req == '/quit':
        ss_quit_event.set()
        return('{"quit":"yes"}', "application/json")
    elif req == '/':
        return(gen_data_page(), "text/html")
    elif req == '/get_servo_data':
        return(gen_data_sd(), "application/json")
    elif req == 'is_library_loaded':
        return('{"aap":1}', "application/json")
    elif req == '/favicon.ico':
        raise(NotFound404("404"))
    else:
        return('{"error":"unknown function"}', "application/json")
        
def gen_data_page():
    f = open(SimpleHandler.HTMLFILE1)
    page = f.read()
    f.close()
    f = open(SimpleHandler.JQUERYFILE)
    page = page + f.read()
    f.close()
    f = open(SimpleHandler.HTMLFILE2)
    page = page + f.read()
    f.close()
    return(page)

def gen_data_sd():
    return('{"pw":[1,2,3,4,5,6,7,8,9,10,11,12]}')
            
S = SimpleWebServer(42426)
H = SimpleHandler()
    
if __name__ == "__main__":
    S.start()
    H.start()