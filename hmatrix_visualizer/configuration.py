import xml.etree.ElementTree as ElementTree
#C.legs[4].pivots[3].x/y/z/rotx/roty/rotz

class Configuration:

    def __init__(self, path):
        self.legs = []
        self.reachable_sector_radius = 0.1
        self.transfer_speeds = [0.0, 0.0, 0.0]
        self.ASM_min = 0.0
        self.ground_clearance = 0.0
        self.search_width = 0.0
        self.load(path)
        
    def load(self, path):
        try:
            xml = ElementTree.parse(path)
        except IOError:
            print("file not found: " + path)
            return(False)
        #gaitgenerator config
        gg = xml.find("gaitgenerator")
        self.reachable_sector_radius = float(gg.attrib["HLlength"])
        self.transfer_speeds[0] = float(gg.attrib["transfer_speed1"])
        self.transfer_speeds[1] = float(gg.attrib["transfer_speed2"])
        self.transfer_speeds[2] = float(gg.attrib["transfer_speed3"])
        self.ASM_min = float(gg.attrib["ASM_min"])
        self.ground_clearance = float(gg.attrib["ground_clearance"])
        self.search_width = float(gg.attrib["search_width"])
        #mechanical config
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
                offset = pivot.attrib["offset"]
                abs_max = pivot.attrib["abs_max"]
                pos = pivot.find("pos")
                x = pos.attrib["x"]
                y = pos.attrib["y"]
                z = pos.attrib["z"]
                self.legs[-1].pivots.append(ConfigPivot(
                    id, x, y, z, rotx, roty, rotz, offset, abs_max))
        return(True)


class ConfigLeg:
    
    def __init__(self, id):
        self.id = int(id)
        self.pivots= []

class ConfigPivot:

    def __init__(self, id, x, y, z, rotx, roty, rotz, offset, abs_max):
        self.id = int(id)
        self.x = float(x)
        self.y = float(y)
        self.z = float(z)
        self.rotx = float(rotx)
        self.roty = float(roty)
        self.rotz = float(rotz)
        self.offset = float(offset)
        self.abs_max = float(abs_max)
