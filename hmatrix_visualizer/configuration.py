import xml.etree.ElementTree as ElementTree
#C.legs[4].pivots[3].x/y/z/rotx/roty/rotz

class Configuration:

    def __init__(self, path):
        self.legs = []
        self.load(path)
        
    def load(self, path):
        try:
            xml = ElementTree.parse(path)
        except IOError:
            print("file not found: " + path)
            return(False)
        legs = xml.findall("leg")
        for leg in legs:
            id = int(leg.attrib["id"])
            self.legs.append(ConfigLeg(id))
            pivots = leg.findall("pivot")
            for pivot in pivots:
                id = pivot.attrib["id"]
                rotx = pivot.attrib["rotx"]
                roty = pivot.attrib["roty"]
                rotz = pivot.attrib["rotz"]
                pos = pivot.find("pos")
                x = pos.attrib["x"]
                y = pos.attrib["y"]
                z = pos.attrib["z"]
                self.legs[-1].pivots.append(ConfigPivot(
                    id, x, y, z, rotx, roty, rotz))
        return(True)


class ConfigLeg:
    
    def __init__(self, id):
        self.id = int(id)
        self.pivots= []

class ConfigPivot:

    def __init__(self, id, x, y, z, rotx, roty, rotz):
        self.id = int(id)
        self.x = float(x)
        self.y = float(y)
        self.z = float(z)
        self.rotx = float(rotx)
        self.roty = float(roty)
        self.rotz = float(rotz)