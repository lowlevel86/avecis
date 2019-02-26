from avecis import *

host = '127.0.0.1'
port = 27015

def eventCallback(evType, keyCode, xM, yM):
   return

def Main():
   black = [0x0, 0x0, 0x0]
   red = [0xFF, 0x0, 0x0]
   green = [0x0, 0xFF, 0x0]
   blue = [0x0, 0x0, 0xFF]
   
   leftEye = [-20.0, 30.0, 0.0, -20.0, -30.0, 0.0]
   rightEye = [20.0, 30.0, 0.0, 20.0, -30.0, 0.0]
   mouth = [-75.0, -60.0, 0.0, -30.0, -95.0, 0.0, 20.0, -90.0, 0.0, 70.0, -50.0, 0.0]
   
   beep = []
   
   avecisConnect(host, port, eventCallback)
   
   # draw a smiley face
   setColor(black)
   clearScreen()
   
   setColor(red)
   drawLine(leftEye)
   
   setColor(green)
   drawLine(rightEye)
   
   setColor(blue)
   drawPath(mouth)
   
   showContent()
   
   # create and play square wave
   for i in range(0, 5000):
      if i%50 >= 25:
         beep.append(1.0)
      else:
         beep.append(0.0)
   
   playSound(beep, beep)
   
   # print to status bar
   printSB("Hello World!")
   
   avecisDisconnect()

Main()
