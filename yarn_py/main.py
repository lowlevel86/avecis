import math
import time
from avecis import *

host = '127.0.0.1'
port = 27015
win_width = 640
win_height = 360

bgImg = None
yarn = None


def loadImg(imgFile, width, height):
   img = []
   
   with open(imgFile, "rb") as f:
      data = f.read()
   
   headerSize = len(data) - width * height * 3
   for i in range(width*height):
      img.append(bToINT(data[headerSize+i*3+2]))
      img.append(bToINT(data[headerSize+i*3+1]))
      img.append(bToINT(data[headerSize+i*3+0]))
   
   return img


def rotVert(degrees, hPt, vPt):
   
   hUC = math.cos(degrees * (math.pi * 2.0 / 360.0))
   vUC = math.sin(degrees * (math.pi * 2.0 / 360.0))
   
   new_hPt = vPt * -vUC + hPt * hUC
   new_vPt = hPt * vUC + vPt * hUC
   
   return new_hPt, new_vPt


def rotate(verts, x, y, z):
   new_verts = []
   
   for i in range(0, int(len(verts)/3)):
      xPt = verts[i*3+0]
      yPt = verts[i*3+1]
      zPt = verts[i*3+2]
      yPt, zPt = rotVert(x, yPt, zPt)
      xPt, zPt = rotVert(y, xPt, zPt)
      xPt, yPt = rotVert(z, xPt, yPt)
      new_verts.append(xPt)
      new_verts.append(yPt)
      new_verts.append(zPt)
   
   return new_verts


msec = 0
msec_prior = 0
def eventCallback(evType, keyCode, xM, yM):
   global msec
   global msec_prior
   global bgImg
   global yarn
   
   if evType == MOUSE_MOVE:
      
      # draw screen at time intervals
      msec = int(round(time.time() * 1000))
      
      if (msec - msec_prior >= 100):
         
         setColor(bgImg)
         clearScreen()
         setColor([0xFF, 0x0, 0x0])
         drawPath(rotate(yarn, yM, -xM, 0))
         showContent()
         
         msec_prior = msec
   
   if keyCode == KC_ESC:
      unblockAvecisDisconnect()


def Main():
   global bgImg
   global yarn
   
   bgImg = loadImg("cat.bmp", win_width, win_height)
   
   # create yarn
   yarn = []
   for i in range(0, 256):
      percent = i / 255.0
      yarn.append(math.sin(percent * (math.pi * 2.0 / (1.0/9))) * 100 * math.sin(percent * (math.pi * 2.0 / 2.0)))
      yarn.append(math.cos(percent * (math.pi * 2.0 / (1.0/9))) * 100 * math.sin(percent * (math.pi * 2.0 / 2.0)))
      yarn.append(math.cos(percent * (math.pi * 2.0 / 2.0)) * 100)
   
   avecisConnect(host, port, eventCallback)
   
   antialiasingMode(ON)
   orthographicMode(OFF)
   setPerspective(210)
   viewStart(200)
   viewEnd(-200)
   fogStart(100)
   fogEnd(-75)
   fogMode(ON)
   
   setColor([0x40, 0x40, 0x40])
   fogColor()
   
   setColor(bgImg)
   clearScreen()
   setColor([0xFF, 0x0, 0x0])
   drawPath(yarn)
   showContent()
   
   printSB("Hello Cat!")
   
   blockAvecisDisconnect()

Main()
