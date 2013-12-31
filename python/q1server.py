import socket
import sys, os
import threading
import Queue
import crobotlib
import configuration
import pageparse
import math

PORT = 42426
HTMLFILE = os.path.join(os.path.dirname(__file__),"web/gui.html")
NUMBER_OF_LEGS = 4
SERVOS_PER_LEG = 3
class Client(threading.Thread):
    def __init__(self, c):
        threading.Thread.__init__(self)
        self.c = c
    def run(self):
        accept_go(self.c)
        
def send_data(c, data, mimetype="application/json"):
    response = 'HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nVary: Accept-Encoding\r\nContent-Type: {0}\r\nContent-Length: {1}\r\n\r\n{2}'.format(mimetype, len(data), data)    
    c.sendall(response)
    #print("-send data")

def send_404(c):
    response ='HTTP/1.1 404 Not Found\r\nConnection: keep-alive\r\n\r\nContent-Length: 0\r\n\r\n'
    c.sendall(response)
    #print("-send 404")
    
def send_error(c, error):
    data = '{"error":"' + repr(error).replace("\"", "\\\"") + '"}'
    response = (
        'HTTP/1.1 500 Internal Error\r\n' + 
        'Connection: keep-alive\r\n' + 
        'Vary: Accept-Encoding\r\n' +
        'Content-Type: application/json\r\n' +
        'Content-Length: ' + repr(len(data)) + '\r\n\r\n' +
        data
    )
    c.sendall(response)
    
def setup_socket():
    #socket.setdefaulttimeout(1)
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('localhost', PORT));
    s.listen(5)
    return(s)

def accept_go(c):
    print("+1")
    received = ""
    sopen = True
    while sopen:
        try:
            data = c.recv(1024)
        except: break
        if not data: break
        received = received + data
        if received.endswith('\r\n\r\n'):
            req = parse_request(received)
            switchboard(c, req)
            received = ""
    c.close()
    print("-1")

def parse_request(text):
    #only look at first line
    first_linebreak = text.find('\r\n')
    if first_linebreak > 1 :
        #print(text[0:first_linebreak])
        #now find the request between the 
        #first two spaces
        a = text.find(' ')
        b = text.find(' ', a + 1)
        req = text[a+1:b] #req = "/page?ladida=3"
        qm = req.find('?')
        if qm > 0:
            d = grab_params(req[qm+1:b])
            req = req[:qm]
        else:
            d = {}
        return([req,d])
    return(['',{}])

def grab_params(req):
    d = {}
    if req.endswith(';'): req = req[:-1]
    paramlist = req.split('&')
    for set in paramlist:
        i = set.find('=')
        if i > 0 and len(set) > i+1:
            d[set[:i]] = set[i+1:]
    return(d)
    
def switchboard(c, req):
    #print(req[0])
    if req[0] == '/':
        send_page(c)
    elif req[0] == '/quit':
        print('=== stopping ===')
        send_data(c, "")
        c.close()
        os._exit(0)
    elif req[0] == '/favicon.ico':
        send_404(c)
    elif req[0] == '/is_library_loaded':
        send_libloaded(c)
    elif req[0] == '/echo':
        send_echo(c, req[1])
    elif req[0] == '/load_lib':
        send_load_lib(c, req[1])
    elif req[0] == '/change_servo':
        send_change_servo(c, req[1]);
    elif req[0] == '/connect':
        send_connect(c)
    else:
        send_error(c, req[0]+' unknown')

def send_page(c):
    page = pageparse.build_page(HTMLFILE)
    send_data(c, page, "text/html")

def send_libloaded(c):
    if crobot:
        send_data(c, '{"is_library_loaded":1}')
    else:
        send_data(c, '{"is_library_loaded":0}')
        
def send_echo(c, d={}):
    try:
        data = '{'
        for k, v in d.iteritems():
            data = data + '"' + repr(k) + '":"' + repr(v) + '",'
        send_data(c, data[:-1]+'}')
    except Exception as e:
        send_error(e)
        
def send_load_lib(c, d={}, retry=True):
    try:
        global crobot
        if crobot:
            send_error(c, "already loaded")
            return
        path = d['path']
        #path = os.path.join(os.path.dirname(__file__), path)
        crobot = crobotlib.Crobot(path)
        crobot.enable_com()
        load_config()
    except OSError as e:
        if e.errno == 22 and not os.path.exists(path) and retry:
            path = os.path.join(os.path.dirname(__file__), path)
            print("retrying {}".format(path))
            send_load_lib(c, {'path':path}, False)
        else:
            send_error(c, e)
    except Exception as e:
        send_error(c, e)
        return
    send_data(c, '{"success":"1"}')    
    
def send_change_servo(c, d={}):
    synced = True
    try:
        l = int(d['n'])/SERVOS_PER_LEG
        s = int(d['n'])%SERVOS_PER_LEG
        v = float(d['v'])
        crobot.changeServo(l, s, v)
        r = crobot.commit()
        if r < 0:
            synced = False
        send_data(c, gen_servo_data(sync))
    except Exception as e:
        send_error(c, e)
    
def gen_servo_data(synced):
    global crobot
    if synced:
        # usb device has same servo pws
        l = ['{"synced":"1",']
    else:
        l = ['{"synced":"0",']
    l.append('"pw":[')
    for pw in crobot.com.pulsewidths:
        l.append(repr(pw))
        l.append(',')
    l[-1] = '], "angles":['
    for angle in crobot.com.angles:
        l.append(repr(angle))
        l.append(',')
    l[-1] = ']}'
    return("".join(l))

def send_connect(c):
    global crobot
    try:
        if crobot.connect() < 1:
            raise(Exception("connection failed"))
        send_data(c, gen_servo_data())
    except Exception as e:
        send_error(c, e)
    

def load_config():
    path = configuration.CONFIG_FILE #default
    global crobot
    try:
        config = configuration.Configuration()
        config.load(path) #raises ioerrer for filenotfound
        for leg in config.legs:
            for servo in leg.servos:
                crobot.setServoOffset(leg.n, servo["n"], float(servo["offset"]))
                ori = servo["orientation"]
                if ori == "xy":
                    crobot.setServoDirection(leg.n, servo["n"], -1);
                elif ori == "zx":
                    crobot.setServoDirection(leg.n, servo["n"], -1);
            crobot.setLegLengths(
                leg.n,
                float(leg.sections["A"]["length"]),
                float(leg.sections["B"]["length"]),
                float(leg.sections["C"]["length"]))
            crobot.set_leg_offset(
                leg.n, 
                float(leg.offset["x"]),
                float(leg.offset["y"]),
                float(leg.offset["z"]))
    except Exception as e:
        print(e)
        
        
class Server(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
    def run(self):
        self.s = setup_socket()
        while 1:
            try:
                c, a = self.s.accept()
            except socket.timeout:
                continue
            except:
                return
            Client(c).start()    
    
if __name__ == "__main__":
    crobot = None
    try:
        crobot = crobotlib.Crobot("libcrobot642.dll")
        load_config()
    except Exception as e:
        print(e)
    S = Server()
    S.start()
    S.join()