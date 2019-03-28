from avecis import *

host = '127.0.0.1'
port = 27015
win_width = 640
win_height = 360

lMouseDwn = 0
xMouse = None
yMouse = None
ptData = [0, 0, 0, 0, 0, 0]

def eventCallback(evType, keyCode, xM, yM):
   global lMouseDwn
   global xMouse
   global yMouse
   global ptData
   white = [0xFF, 0xFF, 0xFF]
   gray = [0x99, 0x77, 0x77]
   
   if evType == MOUSE_MOVE:
      xMouse = xM - win_width/2.0
      yMouse = -yM + win_height/2.0
   
   if evType == L_MOUSE_DOWN:
      ptData[0] = xMouse
      ptData[1] = yMouse
      ptData[3] = xMouse
      ptData[4] = yMouse
      lMouseDwn = 1
   
   if evType == L_MOUSE_UP:
      lMouseDwn = 0

   if lMouseDwn == 1:
      ptData[3] = ptData[0]
      ptData[4] = ptData[1]
      ptData[0] = xMouse
      ptData[1] = yMouse
      setColor(gray)
      drawLine(ptData)
      showContent()
   
   if evType == R_MOUSE_DOWN:
      setColor(white)
      clearScreen()
      showContent()
   
   printSB("evType:%i keyCode:0x%X xM:%i yM:%i -- Press Esc to Disconnect" % (evType, keyCode, xM, yM))
   
   if keyCode == KC_ESC:
      unblockAvecisDisconnect()

def Main():
   avecisConnect(host, port, eventCallback)
   
   setColor([0xFF, 0xFF, 0xFF])
   clearScreen()
   showContent()
   antialiasingMode(OFF)
   
   blockAvecisDisconnect()

Main()
