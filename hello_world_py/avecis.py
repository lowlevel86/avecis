import sys
import socket
import threading
import struct

ON = 1
OFF = 0

KEY_DOWN = 0
KEY_UP = 1
L_MOUSE_DOWN = 2
L_MOUSE_UP = 3
R_MOUSE_DOWN = 4
R_MOUSE_UP = 5
M_MOUSE_DOWN = 6
M_MOUSE_UP = 7
MOUSE_MOVE = 8
MOUSE_WHEEL_UP = 9
MOUSE_WHEEL_DOWN = 10
SOUND_DONE_SIGNAL = 11
DISCONNECT_SIGNAL = 0xFF

KC_BACKSPACE = 0x8
KC_TAB = 0x9
KC_ENTER = 0xD
KC_NP_ENTER = 0x100000D
KC_LEFT_SHIFT = 0x10
KC_RIGHT_SHIFT = 0x2000010
KC_LEFT_CTRL = 0x11
KC_RIGHT_CTRL = 0x1000011
KC_LEFT_ALT = 0x12
KC_RIGHT_ALT = 0x1000012
KC_PAUSE = 0x13
KC_CAPS_LOCK = 0x14
KC_ESC = 0x1B
KC_SPACE = 0x20

KC_PAGE_UP = 0x1000021
KC_PAGE_DOWN = 0x1000022
KC_END = 0x1000023
KC_HOME = 0x1000024

KC_LEFT = 0x1000025
KC_UP = 0x1000026
KC_RIGHT = 0x1000027
KC_DOWN = 0x1000028

KC_PRINT_SCREEN = 0x100002C
KC_INSERT = 0x100002D
KC_DELETE = 0x100002E

KC_0 = 0x30
KC_1 = 0x31
KC_2 = 0x32
KC_3 = 0x33
KC_4 = 0x34
KC_5 = 0x35
KC_6 = 0x36
KC_7 = 0x37
KC_8 = 0x38
KC_9 = 0x39

KC_A = 0x41
KC_B = 0x42
KC_C = 0x43
KC_D = 0x44
KC_E = 0x45
KC_F = 0x46
KC_G = 0x47
KC_H = 0x48
KC_I = 0x49
KC_J = 0x4A
KC_K = 0x4B
KC_L = 0x4C
KC_M = 0x4D
KC_N = 0x4E
KC_O = 0x4F
KC_P = 0x50
KC_Q = 0x51
KC_R = 0x52
KC_S = 0x53
KC_T = 0x54
KC_U = 0x55
KC_V = 0x56
KC_W = 0x57
KC_X = 0x58
KC_Y = 0x59
KC_Z = 0x5A

KC_LEFT_MENU = 0x100005B
KC_RIGHT_MENU = 0x100005D

KC_NP_0 = 0x60
KC_NP_1 = 0x61
KC_NP_2 = 0x62
KC_NP_3 = 0x63
KC_NP_4 = 0x64
KC_NP_5 = 0x65
KC_NP_6 = 0x66
KC_NP_7 = 0x67
KC_NP_8 = 0x68
KC_NP_9 = 0x69

KC_NP_MULTIPLY = 0x6A
KC_NP_PLUS = 0x6B
KC_NP_MINUS = 0x6D
KC_NP_DECIMAL = 0x6E
KC_NP_DIVIDE = 0x100006F

KC_F1 = 0x70
KC_F2 = 0x71
KC_F3 = 0x72
KC_F4 = 0x73
KC_F5 = 0x74
KC_F6 = 0x75
KC_F7 = 0x76
KC_F8 = 0x77
KC_F9 = 0x78
KC_F10 = 0x79
KC_F11 = 0x7A
KC_F12 = 0x7B

KC_NUM_LOCK = 0x1000090
KC_SCROLL_LOCK = 0x91

KC_SEMICOLON = 0xBA
KC_PLUS = 0xBB
KC_COMMA = 0xBC
KC_MINUS = 0xBD
KC_PERIOD = 0xBE
KC_SLASH = 0xBF
KC_BACKQUOTE = 0xC0
KC_LEFT_BOX_BRACKET = 0xDB
KC_BACKSLASH = 0xDC
KC_RIGHT_BOX_BRACKET = 0xDD
KC_SINGLE_QUOTE = 0xDE


###############################################################################
######################### SERVER CONNECTION FUNCTIONS #########################

def iniClient(hostname, portNum):
   
   s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
   s.connect((hostname, portNum))

   return s

def endClient(s):
   s.close()

###############################################################################


###########################################################################
######################### DATA TRANSFER FUNCTIONS #########################

sendReceive_ConnectSocket = None;
sendReceive_CloseDataReceiver = 0;
sendReceive_ReceiveData_Thread = None;


def receiveData(receiveCallback = None, eventCallback = None):
   global sendReceive_CloseDataReceiver
   global sendReceive_ConnectSocket
   
   while sendReceive_CloseDataReceiver == 0:
      rBuff = sendReceive_ConnectSocket.recv(4096)
      receiveCallback(rBuff, len(rBuff), eventCallback)


def sendData(bytes):
   global sendReceive_ConnectSocket
   
   sendReceive_ConnectSocket.send(bytes)


def iniSendReceiveClient(host, port, receiveCallback = None, eventCallback = None):
   global sendReceive_ConnectSocket
   global sendReceive_ReceiveData_Thread
   
   sendReceive_ConnectSocket = iniClient(host, port)

   sendReceive_ReceiveData_Thread = threading.Thread(target=receiveData, args=(receiveCallback, eventCallback))
   sendReceive_ReceiveData_Thread.start()


def waitUntilReceiverEnd():
   global sendReceive_ReceiveData_Thread
   
   sendReceive_ReceiveData_Thread.join()


def closeDataReceiver():
   global sendReceive_CloseDataReceiver

   sendReceive_CloseDataReceiver = 1;


def endSendReceiveClient():
   global sendReceive_CloseDataReceiver
   global sendReceive_ConnectSocket
   
   sendReceive_CloseDataReceiver = 1
   endClient(sendReceive_ConnectSocket)

###########################################################################


#########################################################################
######################### MAIN AVECIS FUNCTIONS #########################

avecis_sendData_lock = threading.Lock()

def bToINT(ch):
   if sys.version_info < (3,):
      return ord(ch)
   else:
      return ch

def viewStart(value):
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(struct.pack('<I', 4) + b'\x00') # 0 = SET_VIEW_START
      
      # send the data
      sendData(struct.pack('<f', value))
   finally:
      avecis_sendData_lock.release()

def viewEnd(value):
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(struct.pack('<I', 4) + b'\x01') # 1 = SET_VIEW_END
      
      # send the data
      sendData(struct.pack('<f', value))
   finally:
      avecis_sendData_lock.release()

def setPerspective(value):
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(struct.pack('<I', 4) + b'\x02') # 2 = SET_PERSPECTIVE
      
      # send the data
      sendData(struct.pack('<f', value))
   finally:
      avecis_sendData_lock.release()

def orthographicMode(value):
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(struct.pack('<I', 1) + b'\x03') # 3 = SET_ORTHOGRAPHIC_MODE
      
      # send the data
      if value:
         sendData(b'\x01')
      else:
         sendData(b'\x00')
   finally:
      avecis_sendData_lock.release()

def fogMode(value):
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(struct.pack('<I', 1) + b'\x04') # 4 = SET_FOG_MODE
      
      # send the data
      if value:
         sendData(b'\x01')
      else:
         sendData(b'\x00')
   finally:
      avecis_sendData_lock.release()

def fogColor(value):
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(struct.pack('<I', 4) + b'\x05') # 5 = SET_FOG_COLOR
      
      # send the data
      sendData(struct.pack('<I', value))
   finally:
      avecis_sendData_lock.release()

def fogStart(value):
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(struct.pack('<I', 4) + b'\x06') # 6 = SET_FOG_START
      
      # send the data
      sendData(struct.pack('<f', value))
   finally:
      avecis_sendData_lock.release()

def fogEnd(value):
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(struct.pack('<I', 4) + b'\x07') # 7 = SET_FOG_END
      
      # send the data
      sendData(struct.pack('<f', value))
   finally:
      avecis_sendData_lock.release()

def antialiasingMode(value):
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(struct.pack('<I', 1) + b'\x08') # 8 = SET_ANTIALIASING_MODE
      
      # send the data
      if value:
         sendData(b'\x01')
      else:
         sendData(b'\x00')
   finally:
      avecis_sendData_lock.release()

def setColor(colorData):
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(struct.pack('<I', len(colorData)) + b'\x09') # 9 = SET_COLOR
      
      # send the data
      sendData(struct.pack('%sB' % len(colorData), *colorData))
   finally:
      avecis_sendData_lock.release()

def clearScreen():
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(b'\x00\x00\x00\x00\x0A') # 10 = CLEAR_SCREEN
   finally:
      avecis_sendData_lock.release()

def drawLine(coordData):
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(struct.pack('<I', len(coordData)*4) + b'\x0B') # 11 = DRAW_LINE
      
      # send the data
      sendData(struct.pack('<%sf' % len(coordData), *coordData))
   finally:
      avecis_sendData_lock.release()

def drawPath(coordData):
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(struct.pack('<I', len(coordData)*4) + b'\x0C') # 12 = DRAW_PATH
      
      # send the data
      sendData(struct.pack('<%sf' % len(coordData), *coordData))
   finally:
      avecis_sendData_lock.release()

def showContent():
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(b'\x00\x00\x00\x00\x0D') # 13 = SHOW_CONTENT
   finally:
      avecis_sendData_lock.release()

def printSB(textData):
   # send the data head array
   asciiStr = textData.encode('ascii')
   
   avecis_sendData_lock.acquire()
   try:
      sendData(struct.pack('<I', len(asciiStr)) + b'\x0E') # 14 = PRINT_STATUS
      
      # send the data
      sendData(asciiStr)
   finally:
      avecis_sendData_lock.release()

def playSound(leftSnd, rightSnd):
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(struct.pack('<I', len(leftSnd)*4 + len(rightSnd)*4) + b'\x0F') # 15 = PLAY_SOUND
      
      # send the data
      sendData(struct.pack('<%sf' % len(leftSnd), *leftSnd))
      sendData(struct.pack('<%sf' % len(rightSnd), *rightSnd))
   finally:
      avecis_sendData_lock.release()

def stopSound():
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(b'\x00\x00\x00\x00\x10') # 16 = STOP_SOUND
   finally:
      avecis_sendData_lock.release()

def endTransmission():
   avecis_sendData_lock.acquire()
   try:
      # send the data head array
      sendData(b'\x00\x00\x00\x00\xFF') # 255 = END_TRANSMISSION
   finally:
      avecis_sendData_lock.release()

#static variables
avecis_varsDataInc = 0
avecis_eventType = None
avecis_eventData = None

def receiveCallback(bytes, byteCnt, eventCallback = None):
   global avecis_varsDataInc
   global avecis_eventType
   global avecis_eventData
   byteInc = 0
   avecis_varsDataInc = 0
   
   if byteCnt == 0:
      avecis_varsDataInc = 0
      return
   
   # unpack 5 bytes, 1 byte for "eventType" and 4 bytes for "eventData"
   while 1:
      if avecis_varsDataInc == 0:
         avecis_eventType = bToINT(bytes[byteInc])
         byteInc += 1
         avecis_varsDataInc += 1
      
      if byteInc >= byteCnt:
         return
      
      if avecis_varsDataInc == 1:
         avecis_eventData = bToINT(bytes[byteInc])
         byteInc += 1
         avecis_varsDataInc += 1
      
      if byteInc >= byteCnt:
         return
      
      if avecis_varsDataInc == 2:
         avecis_eventData |= bToINT(bytes[byteInc]) << 8
         byteInc += 1
         avecis_varsDataInc += 1
      
      if byteInc >= byteCnt:
         return
      
      if avecis_varsDataInc == 3:
         avecis_eventData |= bToINT(bytes[byteInc]) << 16
         byteInc += 1
         avecis_varsDataInc += 1
      
      if byteInc >= byteCnt:
         return
      
      if avecis_varsDataInc == 4:
         avecis_eventData |= bToINT(bytes[byteInc]) << 24
         byteInc += 1
         
         if avecis_eventType == 0xFF: # 0xFF = DISCONNECT_SIGNAL
            exit()
         
         if avecis_eventType == 8: # 8 = MOUSE_MOVE
            eventCallback(avecis_eventType, 0, avecis_eventData&0xFFFF, avecis_eventData>>16&0xFFFF)
         else:
            eventCallback(avecis_eventType, avecis_eventData, 0, 0)
         
         avecis_varsDataInc = 0
      
      if byteInc >= byteCnt:
         return

def avecisConnect(host, port, eventCallback = None):
   iniSendReceiveClient(host, port, receiveCallback, eventCallback)

# this function is not to be used if avecisDisconnect() is used
def blockAvecisDisconnect():
   waitUntilReceiverEnd()

# this function is not to be used if avecisDisconnect() is used
def unblockAvecisDisconnect():
   # blockAvecisDisconnect() must be called first
   endSendReceiveClient()

# this function is not to be used if
# blockAvecisDisconnect() and
# unblockAvecisDisconnect() is used
def avecisDisconnect():
   closeDataReceiver()
   endTransmission()
   waitUntilReceiverEnd()
   endSendReceiveClient()

#########################################################################
