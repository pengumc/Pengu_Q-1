#!/usr/bin/env python2

import os
import xml.etree.ElementTree as ElementTree

CONFIG_FILE = "crobot.xml"

class Configuration:
    
    def __init__(self):
        self.legs = []

    def load(self, filename=CONFIG_FILE):
        path = os.path.dirname(__file__) + os.sep + filename
        try:
            xml = ElementTree.parse(path)
        except IOError as e:
            raise(e)
        legs = xml.findall("leg")
        for leg in legs:
            n = int(leg.attrib["n"])
            l = None
            l = LegConfig(n)
            sections = leg.findall("section")
            for s in sections:
                l.sections[s.attrib["id"]] = s.attrib
            servos = leg.findall("servo")
            for s in servos:
                l.servos.append(s.attrib)
            offset = leg.find("offset")
            l.offset = offset.attrib
            self.legs.append(l)
        return(True)
        

class LegConfig:
    def __init__(self, n):
        self.sections = {}
        self.servos = []
        self.n = n
        self.offset = {}

#------------------------------- -----------------------------------------
if __name__ == "__main__":
    c = Configuration()
    c.load()
    #print(c.legs[0].sections[0]["length"])

