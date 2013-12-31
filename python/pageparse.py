import re
import os.path

def build_page(filename):
    L = [];
    dir = os.path.dirname(filename)
    f = open(filename)
    src = f.read()
    f.close()
    pos = add_scripts(src, L, dir)
    pos += add_style(src[pos:], L, dir)
    #add remainder of file
    L.append(src[pos:])
    return("".join(L))
        

def add_scripts(src, L, dir):
    pat = '<script\s+src="(?P<path>[^"]+)"\s*>\s*</script>'
    matchiter = re.finditer(pat, src)
    return(add_filecontents(matchiter, src, L, dir, "script"))

def add_style(src, L, dir):
    pat = '<link\s+rel="stylesheet"\s+href="(?P<path>[^"]+)"\s*>'
    matchiter = re.finditer(pat, src)
    return(add_filecontents(matchiter, src, L, dir, "style"))
    
def add_filecontents(matchiter, src, L, dir, t):
    pos = 0
    if matchiter:
        while 1:
            try:
                match = matchiter.next()
                extfile = match.group('path')
                #print(extfile)
                s = match.start(0)
                e = match.end(0)
                #print(src[s:e])
                L.append(src[pos:s])
                add_file(L, os.path.join(dir, extfile), s, e, t)
                pos = e
            except StopIteration:
                break
            except IndexError:
                continue
    return(pos)

    
def add_file(L, path, s, e, t):
    #opening tag
    L.append("<")
    L.append(t)
    L.append(">")
    #file
    f = open(path)
    L.append(f.read())
    f.close()
    #closing tag
    L.append("</")
    L.append(t)
    L.append(">")
    
