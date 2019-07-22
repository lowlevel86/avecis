import math
import time
from avecis import *
from shapes import *
from ucrot import *
from vector_ascii import *
from controls import *


host = '127.0.0.1'
port = 27015
win_width = 640
win_height = 360
ball_radius = 120

class rv:
   """renderer variables"""
   viewStart = 250
   viewEnd = -50
   perspective = 300
   orthographic = True
   fog = True
   fogStart = 100
   fogEnd = -50
   antialiasing = True

class rc:
   """renderer controls"""
   aa_btn = None
   ortho_btn = None
   fog_btn = None
   fog_start_sl = None
   fog_end_sl = None
   perspective_sl = None
   view_start_sl = None
   view_end_sl = None
   fog_start_label = None
   fog_end_label = None
   view_start_label = None
   view_end_label = None
   fog_label = None
   perspective_label = None
   view_label = None


def rot(verts, UC):
   new_verts = []
   for i in range(0, int(len(verts)/3)):
      x, y, z = ucRotVertex(UC, verts[i*3+0], verts[i*3+1], verts[i*3+2])
      new_verts.append(x)
      new_verts.append(y)
      new_verts.append(z)
   return new_verts

def scale(verts, x, y, z):
   new_verts = []
   for i in range(0, int(len(verts)/3)):
      new_verts.append(verts[i*3+0] * x)
      new_verts.append(verts[i*3+1] * y)
      new_verts.append(verts[i*3+2] * z)
   return new_verts

def move(verts, x, y, z):
   new_verts = []
   for i in range(0, int(len(verts)/3)):
      new_verts.append(verts[i*3+0] + x)
      new_verts.append(verts[i*3+1] + y)
      new_verts.append(verts[i*3+2] + z)
   return new_verts


def drawScreen():
   global msec
   global msec_prior
   
   # draw screen at time intervals
   msec = int(round(time.time() * 1000))
   
   if (msec - msec_prior >= 50):
      
      setColor([0xFF, 0xFF, 0xFF])
      clearScreen()
      setColor([0x99, 0x77, 0x77])
      drawLine(rot(ball, ball_UC))
      setColor([0x77, 0x77, 0x99])
      drawLine(rot(torus, torus_UC))
      
      setColor([0x55, 0x55, 0x55])
      
      drawButton(rc.aa_btn, rv)
      drawButton(rc.ortho_btn, rv)
      drawButton(rc.fog_btn, rv)
      
      drawSlider(rc.fog_start_sl, rv)
      drawSlider(rc.fog_end_sl, rv)
      drawSlider(rc.perspective_sl, rv)
      drawSlider(rc.view_start_sl, rv)
      drawSlider(rc.view_end_sl, rv)
   
      drawLabel(rc.fog_start_label, rv)
      drawLabel(rc.fog_end_label, rv)
      drawLabel(rc.view_start_label, rv)
      drawLabel(rc.view_end_label, rv)
      drawLabel(rc.fog_label, rv)
      drawLabel(rc.perspective_label, rv)
      drawLabel(rc.view_label, rv)
      
      showContent()
      msec_prior = msec
   
   return


def eventCallback(evType, keyCode, xM, yM):
   global mMouseDwn
   global rMouseDwn
   global xMouse
   global yMouse
   global xMousePrior
   global yMousePrior
   
   global ball
   global ball_UC
   global torus
   global torus_UC
   
   updateGraphics = False
   
   # store mouse events
   if evType == MOUSE_MOVE:
      xMouse = xM - win_width/2.0
      yMouse = -yM - -win_height/2.0

   if evType == L_MOUSE_DOWN:
      updateGraphics = True
   
   if evType == L_MOUSE_UP:
      updateGraphics = True

   if evType == M_MOUSE_DOWN:
      mMouseDwn = True
      updateGraphics = True
   
   if evType == M_MOUSE_UP:
      mMouseDwn = False
      updateGraphics = True

   if evType == R_MOUSE_DOWN:
      rMouseDwn = True
      updateGraphics = True
   
   if evType == R_MOUSE_UP:
      rMouseDwn = False
      updateGraphics = True

   # press 'R' to refresh graphics
   if keyCode == KC_R:
      updateGraphics = True

   if clickButton(rc.aa_btn, evType, xMouse, yMouse):
      rv.antialiasing = not rv.antialiasing
      antialiasingMode(rv.antialiasing)

   if clickButton(rc.ortho_btn, evType, xMouse, yMouse):
      rv.orthographic = not rv.orthographic
      orthographicMode(rv.orthographic)

   if clickButton(rc.fog_btn, evType, xMouse, yMouse):
      rv.fog = not rv.fog
      fogMode(rv.fog)

   if moveSlider(rc.fog_start_sl, evType, xMouse, yMouse):
      rv.fogStart = getSliderValue(rc.fog_start_sl)
      updateGraphics = True

   if moveSlider(rc.fog_end_sl, evType, xMouse, yMouse):
      rv.fogEnd = getSliderValue(rc.fog_end_sl)
      updateGraphics = True

   if moveSlider(rc.perspective_sl, evType, xMouse, yMouse):
      rv.perspective = getSliderValue(rc.perspective_sl)
      setPerspective(rv.perspective)
      updateGraphics = True

   if moveSlider(rc.view_start_sl, evType, xMouse, yMouse):
      rv.viewStart = getSliderValue(rc.view_start_sl)
      viewStart(rv.viewStart)
      updateGraphics = True

   if moveSlider(rc.view_end_sl, evType, xMouse, yMouse):
      rv.viewEnd = getSliderValue(rc.view_end_sl)
      viewEnd(rv.viewEnd)
      updateGraphics = True

   if rMouseDwn or mMouseDwn:
      ball_UC = trackBallRotUc(ball_radius, xMousePrior, yMousePrior, xMouse, yMouse, ball_UC)
      
      # copy unit circle coordinates of the z axis
      torus_UC[4] = ball_UC[4]
      torus_UC[5] = ball_UC[5]
      
      updateGraphics = True
   
   if updateGraphics:
      drawScreen()
      updateGraphics = False
   
   if onButton(rc.aa_btn, xMouse, yMouse):
      if rv.antialiasing:
         printSB("Antialiasing is on.")
      else:
         printSB("Antialiasing is off.")
   
   if onButton(rc.ortho_btn, xMouse, yMouse):
      if rv.orthographic:
         printSB("Orthographic mode is on.")
      else:
         printSB("Orthographic mode is off.")
   
   if onButton(rc.fog_btn, xMouse, yMouse):
      if rv.fog:
         printSB("Fog is on.")
      else:
         printSB("Fog is off.")
   
   if onSlider(rc.fog_start_sl, xMouse, yMouse):
      printSB(("Fog starts at %.2f." % getSliderValue(rc.fog_start_sl)))
   
   if onSlider(rc.fog_end_sl, xMouse, yMouse):
      printSB(("Fog ends at %.2f." % getSliderValue(rc.fog_end_sl)))
   
   if onSlider(rc.perspective_sl, xMouse, yMouse):
      printSB(("Perspective: %.2f (must be greater than view start)" % getSliderValue(rc.perspective_sl)))
   
   if onSlider(rc.view_start_sl, xMouse, yMouse):
      printSB(("View starts at %.2f." % getSliderValue(rc.view_start_sl)))
   
   if onSlider(rc.view_end_sl, xMouse, yMouse):
      printSB(("View ends at %.2f." % getSliderValue(rc.view_end_sl)))
   
   if evType == MOUSE_MOVE:
      xMousePrior = xMouse
      yMousePrior = yMouse
   
   if keyCode == KC_ESC:
      unblockAvecisDisconnect()


def Main():
   global msec
   global msec_prior
   
   global mMouseDwn
   global rMouseDwn
   global xMouse
   global yMouse
   global xMousePrior
   global yMousePrior
   
   global ball
   global ball_UC
   global torus
   global torus_UC
   
   # initialize timing variables
   msec = 0
   msec_prior = 0
   
   # initialize navigation variables
   mMouseDwn = False
   rMouseDwn = False
   xMouse = 0
   yMouse = 0
   xMousePrior = 0
   yMousePrior = 0
   ball_UC = getUc(0.0, 0.0, 0.0)
   torus_UC = getUc(0.0, 0.0, 0.0)
   
   # create gui and geometry
   rc.aa_btn = createButton("Antialiasing", 25, -win_width/2.0+8, win_height/2.0-35)
   rc.ortho_btn = createButton("Orthographic Mode", 25, -win_width/2.0+8, win_height/2.0-70)
   rc.fog_btn = createButton("Fog", 25, -win_width/2.0+8, win_height/2.0-105)
   
   rc.fog_start_sl = createSlider(-200, -80, 25, 160, -500.0, 500.0, rv.fogStart)
   rc.fog_end_sl = createSlider(-120, -80, 25, 160, -500.0, 500.0, rv.fogEnd)
   rc.perspective_sl = createSlider(-12.5, -80, 25, 160, -500.0, 500.0, rv.perspective)
   rc.view_start_sl = createSlider(95, -80, 25, 160, -500.0, 500.0, rv.viewStart)
   rc.view_end_sl = createSlider(175, -80, 25, 160, -500.0, 500.0, rv.viewEnd)
   
   rc.fog_start_label = createLabel("start", 20, -218, -100, False)
   rc.fog_end_label = createLabel("end", 20, -128, -100, False)
   rc.view_start_label = createLabel("start", 20, 77, -100, False)
   rc.view_end_label = createLabel("end", 20, 167, -100, False)
   
   rc.fog_label = createLabel("Fog", 20, -168, -120, True)
   rc.perspective_label = createLabel("Perspective", 20, -60, -110, True)
   rc.view_label = createLabel("View", 20, 122, -120, True)
   
   ball = scale(genSphere(8, 8), ball_radius, ball_radius, ball_radius)
   torus = scale(genTorus(16, 16, 0.75), ball_radius*2, ball_radius*2, ball_radius*2)
   
   avecisConnect(host, port, eventCallback)
   
   # initialize graphics variables
   viewStart(rv.viewStart)
   viewEnd(rv.viewEnd)
   setPerspective(rv.perspective)
   orthographicMode(rv.orthographic)
   fogMode(rv.fog)
   fogStart(rv.fogStart)
   fogEnd(rv.fogEnd)
   antialiasingMode(rv.antialiasing)
   
   setColor([0xFF, 0xFF, 0xFF])
   fogColor()
   
   drawScreen()
   
   blockAvecisDisconnect()

Main()
