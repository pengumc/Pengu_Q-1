import socket
import sys

PORT = 42426
HTMLFILE1 = "web/webinterface1.html"
HTMLFILE2 = "web/webinterface2.html"
JQUERYFILE = "web/jquery-2.0.3.min.js"

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
    response = (
        'HTTP/1.0 200 OK\r\n' + 
        'Connection: close\r\n' + 
        'Vary: Accept-Encoding\r\n' +        
        'Content-Type: text/html\r\n' +
        'Content-Length: ' + repr(len(page)) + '\r\n' +
        '\r\n' + 
        page
    )
    print(response)
    c.sendall(response)
    
def send_data(c):
    response = (
        'HTTP/1.0 200 OK\r\n' + 
        'Connection: close\r\n' + 
        'Vary: Accept-Encoding\r\n' +
        'Content-Length: 12\r\n' + 
        'Content-Type: text/html\r\n' +
        '\r\n' + 
        'ladida aap\r\n'
    )
    c.sendall(response)
    print(response)

def send_404(c):
    response =(
        'HTTP/1.0 404 Not Found\r\n' + 
        'Connection: close\r\n\r\n'
        'Content-Length: 0\r\n\r\n'
    )
    c.sendall(response)
    print(response)
    
def setup_socket():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('localhost', PORT));
    s.listen(10)
    return(s)

def accept_go(s):
    c,a = s.accept()
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
                print('==== sending page ===')
                send_page(c)
                sopen = False
            elif req == '/quit':
                print('=== stopping ===')
                c.close()
                s.close()
                sys.exit()
            elif req == '/favicon.ico':
                print('=== 404 ===')
                send_404(c)
                sopen = False
            else:
                print('=== data ===')
                send_data(c)
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
        accept_go(s)
    
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
