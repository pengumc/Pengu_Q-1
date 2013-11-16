from ctypes import *

class Crobot:
    
    SERVOCOUNT = 12
    LEGCOUNT = 4
    
    def __init__(self, libpath):
        self.lib = CDLL(libpath)
        #restypes
        self.lib.Quadruped_alloc.restype = c_void_p
        self.lib.Communication_alloc.restype = c_void_p
        #create com instance
        self.com_p = self.lib.Communication_alloc()
        self.com = COMSYS.from_address(self.com_p)
        #create Quadruped instance
        self.qped = self.lib.Quadruped_alloc()
        self.connected = 0

    def enable_com(self):
        #setup com system
        self.lib.Quadruped_enableCommunication(self.qped, self.com_p)
        
    #interface
    #---------------------------------------------------------------------------
    def connect(self):
        print("trying to connect to device...")
        self.connected = self.lib.Quadruped_startup(self.qped)
        return(self.connected)
    #---------------------------------------------------------------------------
    def __del__(self):
        self.lib.Quadruped_free(self.qped)
    #--------------------------------------------------------------------------    
    def getButton(self, button):
        t = self.lib.Quadruped_getPsButton(self.qped, button)
        return(t)
    #--------------------------------------------------------------------------
    def getButtonEdge(self, button):
        t = self.lib.Quadruped_getPsButtonEdge(self.qped, button)
        return(t)
    #--------------------------------------------------------------------------
    def getStick(self, axis):
        return(self.lib.Quadruped_getPsAxis(self.qped, axis))
    #--------------------------------------------------------------------------    
    def update_sensor_data(self):
        return(self.lib.Quadruped_update(self.qped))
    #--------------------------------------------------------------------------
    def changeServo(self, leg, s, value):
        #1: success, 0: error
        return(self.lib.Quadruped_changeSingleServo(
            self.qped, c_byte(leg), c_byte(s), c_double(value)))
    #--------------------------------------------------------------------------
    def commit(self):
        return(self.lib.Quadruped_commit(self.qped))
    #--------------------------------------------------------------------------
    def setLegLengths(self, legno, A, B, C):
        self.lib.Quadruped_configureLegLengths(
            self.qped, legno, c_double(A), c_double(B), c_double(C))
    #--------------------------------------------------------------------------
    def setServoOffset(self, legno, servono, offset):
        self.lib.Quadruped_configureServoOffset(
            self.qped, c_byte(int(legno)), c_byte(int(servono)), c_double(offset))
    #--------------------------------------------------------------------------
    def changeLeg(self, legno, dX, dY, dZ):
        result = self.lib.Quadruped_changeLegEndpoint(
            self.qped, legno, c_double(dX), c_double(dY), c_double(dZ))
        #TODO parse return value
        return(result)
    #--------------------------------------------------------------------------
    def changeAllLegs(self, dX, dY, dZ):
        result = self.lib.Quadruped_changeAllEndpoints(
            self.qped, c_double(dX), c_double(dY), c_double(dZ))
        #TODO parse return value
        print("changeAll: " +str(result))
        return(result)
    #--------------------------------------------------------------------------
    def setAllLegs(self, Z):
        result = self.lib.Quadruped_setAllEndpointZs(self.qped, c_double(Z))
        print("setall: " + str(result))
        return(result)
    #--------------------------------------------------------------------------
    def printLegs(self):
        self.lib.Quadruped_debugLegs(self.qped)
    #--------------------------------------------------------------------------
    def setServoDirection(self, legno, servono, direction):
        self.lib.Quadruped_configureServoDirection(self.qped, 
            c_byte(int(legno)), c_byte(int(servono)), c_int8(int(direction)))
    #--------------------------------------------------------------------------
    def printServoDetails(self, legno, servono):
        self.lib.Quadruped_printServoDetails(self.qped,
            c_byte(int(legno)), c_byte(int(servono)))
    #--------------------------------------------------------------------------
    def load_from_eeprom(self):
        return(self.lib.Quadruped_loadStartPos(self.qped))
    #--------------------------------------------------------------------------
    def set_leg_offset(self, legno, X, Y, Z):
        self.lib.Quadruped_configureLegOffset(
            self.qped, c_byte(legno), c_double(X), c_double(Y), c_double(Z))
    #--------------------------------------------------------------------------
    def rotate(self, xaxis, yaxis, zaxis):
        result = self.lib.Quadruped_rotate(self.qped,
            c_double(xaxis), c_double(yaxis), c_double(zaxis))
        #TODO parse return value
        print("rotate: " + str(result))
        return(result)
                
class SERVOPOS (Structure):
    _fields_ = [("x", c_double*Crobot.SERVOCOUNT),
                ("y", c_double*Crobot.SERVOCOUNT),
                ("z", c_double*Crobot.SERVOCOUNT)]
                
class ENDPOINTS (Structure):
    _fields_ = [("x", c_double*Crobot.LEGCOUNT),
                ("y", c_double*Crobot.LEGCOUNT),
                ("z", c_double*Crobot.LEGCOUNT)]

class COMSYS (Structure):
    _fields_ = [("pulsewidths", c_byte*Crobot.SERVOCOUNT),
                ("angles", c_double*Crobot.SERVOCOUNT),
                ("servopos", SERVOPOS),
                ("endpoints", ENDPOINTS)]
