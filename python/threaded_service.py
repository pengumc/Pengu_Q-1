import socket
import sys, os
import threading

PORT = 42426
HTMLFILE1 = "web/webinterface1.html"
HTMLFILE2 = "web/webinterface2.html"
JQUERYFILE = "web/jquery-2.0.3.min.js"

class Client(threading.Thread):
    def __init__(self, c):
        threading.Thread.__init__(self)
        self.c = c
    def run(self):
        accept_go(self.c)
        

def send_page(c):
    f = open(HTMLFILE1)
    page = f.read()
    f.close()
    f = open(JQUERYFILE)
    page = page + f.read()
    f.close()
    f = open(HTMLFILE2)
    page = page + f.read()
    f.close()
    send_data(c, page, "text/html")
    print("-send page")

def send_libloaded(c):
    pass
    
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
    print("-send data")

def send_404(c):
    response =(
        'HTTP/1.0 404 Not Found\r\n' + 
        'Connection: close\r\n\r\n'
        'Content-Length: 0\r\n\r\n'
    )
    c.sendall(response)
    print("-send 404")
    
def setup_socket():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('', PORT));
    s.listen(10)
    return(s)

def accept_go(c):
    received = ""
    sopen = True
    while sopen:
        try:
            data = c.recv(1024)
        except:
            sopen = False
            break
        if not data: break
        received = received + data
        if received.endswith('\r\n\r\n'):
            #print(received)
            req = parse_request(received)
            print('parsed: {}'.format(req))
            if req == '/':
                send_page(c)
                sopen = False
            elif req == '/quit':
                print('=== stopping ===')
                c.close()
                os._exit(0)
            elif req == '/favicon.ico':
                send_404(c)
                sopen = False
            elif req == 'is_library_loaded':
                send_libloaded(c)
                sopen = False
            else:
                send_data(c, '{"a":"42"}')
                sopen = False
    c.close()

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
    
if __name__ == "__main__":
    s = setup_socket()
    while 1:
        try:
            c, a = s.accept()
        except:
            sys.exit()
        print('starting new thread')
        Client(c).start()
    
# HTTP/1.1 200 OK
# Date: Wed, 13 Nov 2013 15:37:10 GMT
# Server: Apache/2.2.22 (Debian)
# X-Powered-By: PHP/5.4.4-14+deb7u5
# Vary: Accept-Encoding
# Content-Encoding: gzip
# Content-Length: 1038
# Keep-Alive: timeout=15, max=100
# Connection: Keep-Alive
# Content-Type: text/html
