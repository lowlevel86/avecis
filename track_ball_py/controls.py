from avecis import *
from vector_ascii import *


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


class buttonCtrl:
   def __init__(self, text, size, x, y):
      self.text = text
      self.size = size
      self.x = x
      self.y = y
      self.down = False

def createButton(text, size, x, y):
   return buttonCtrl(text, size, x, y)

def clickButton(btn, evType, xMouse, yMouse):
   
   width = (len(btn.text) + 2.0) * (btn.size / 2.0)
   height = btn.size
   
   left = btn.x
   right = btn.x + width
   top = btn.y + height
   bottom = btn.y
   
   if btn.down == True:
      return False
   
   if evType == L_MOUSE_DOWN:
      if xMouse >= left and xMouse <= right and yMouse >= bottom and yMouse <= top:
         btn.down = True
         return True
   
   return False

def onButton(btn, xMouse, yMouse):
   
   width = (len(btn.text) + 2.0) * (btn.size / 2.0)
   height = btn.size
   
   left = btn.x
   right = btn.x + width
   top = btn.y + height
   bottom = btn.y
   
   if xMouse >= left and xMouse <= right and yMouse >= bottom and yMouse <= top:
      return True
   
   return False

def drawButton(btn, rv):
   box = [0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0]
   
   # set renderer variables for drawing the control
   topMost = 10000
   viewStart(topMost+3.0)
   viewEnd(topMost-1.0)
   orthographicMode(ON)
   fogMode(ON)
   fogStart(topMost+1.0)
   fogEnd(topMost-1.0)
   
   # draw button
   length = (len(btn.text) + 2.0) * (btn.size / 2.0)

   frame = scale(box, length, btn.size, 0.0)
   
   if btn.down == False: # if button is not down
      drawPath(move(frame, btn.x, btn.y, topMost+2.0)) # draw front frame
   
   drawPath(move(frame, btn.x-2.0, btn.y-2.0, topMost)) # draw back frame

   # draw button text
   chrLoc = 0
   chSz = btn.size / 2.0
   for ch in btn.text:
      
      if ch == ' ':
         chrLoc += 1
         continue
      
      ch = ord(ch) - 33
      
      for pth in range(0, len(asciiPaths[ch])):
         if btn.down == False: # if button is not down
            drawPath(move(scale(move(asciiPaths[ch][pth], chrLoc+1.5, 1.0, 0.0), chSz, chSz, chSz), btn.x+1.0, btn.y+1.0, topMost+2.0))
         else:
            drawPath(move(scale(move(asciiPaths[ch][pth], chrLoc+1.5, 1.0, 0.0), chSz, chSz, chSz), btn.x-1.0, btn.y-1.0, topMost))
               
      
      chrLoc += 1
   
   # set renderer variables back to their original values
   viewStart(rv.viewStart)
   viewEnd(rv.viewEnd)
   orthographicMode(rv.orthographic)
   fogMode(rv.fog)
   fogStart(rv.fogStart)
   fogEnd(rv.fogEnd)

   btn.down = False # reset button if down
   
   return


class sliderCtrl:
   def __init__(self, x, y, w, h, min, max, value):
      self.x = x
      self.y = y
      self.w = w
      self.h = h
      self.min = min
      self.max = max
      self.value = value
      self.drag = False

def createSlider(x, y, w, h, min, max, iniVal):
   return sliderCtrl(x, y, w, h, min, max, iniVal)

def moveSlider(sldr, evType, xMouse, yMouse):

   left = sldr.x
   right = sldr.x + sldr.w
   top = sldr.y + sldr.h
   bottom = sldr.y
   
   if xMouse >= left and xMouse <= right and yMouse >= bottom and yMouse <= top:
      if evType == L_MOUSE_DOWN:
         sldr.drag = True
   
   if evType == L_MOUSE_UP:
      sldr.drag = False
   
   if sldr.drag:
      if sldr.h > sldr.w:
         percent = float(yMouse - sldr.y) / sldr.h
         
         if percent > 1.0:
            percent = 1.0
         
         if percent < 0.0:
            percent = 0.0
         
         sldr.value = (sldr.max - sldr.min) * percent + sldr.min
      else:
         percent = float(xMouse - sldr.x) / sldr.w
         
         if percent > 1.0:
            percent = 1.0
         
         if percent < 0.0:
            percent = 0.0
         
         sldr.value = (sldr.max - sldr.min) * percent + sldr.min
      
      return True
   
   return False

def getSliderValue(sldr):
   return sldr.value

def onSlider(sldr, xMouse, yMouse):

   left = sldr.x
   right = sldr.x + sldr.w
   top = sldr.y + sldr.h
   bottom = sldr.y
   
   if xMouse >= left and xMouse <= right and yMouse >= bottom and yMouse <= top:
      return True
   
   return False

def drawSlider(sldr, rv):
   box = [0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0]
   
   # set renderer variables for drawing the control
   topMost = 10000
   viewStart(topMost+1.0)
   viewEnd(topMost-1.0)
   orthographicMode(ON)
   fogMode(OFF)
   
   # draw slider
   x = sldr.x
   y = sldr.y
   z = topMost
   w = sldr.w
   h = sldr.h
   percent = float(sldr.value - sldr.min) / (sldr.max - sldr.min)
   
   if percent > 1.0:
      percent = 1.0
   
   if percent < 0.0:
      percent = 0.0
   
   if h > w: # larger height makes it vertical else horizontal
      pos = (h-w/2.0) * percent
      drawPath(move(scale(box, w, w/2.0, 0.0), x, y+pos, z))
      drawLine([x, y, z, x+w, y, z])
      drawLine([x, y+h, z, x+w, y+h, z])
      drawLine([x+w/2.0, y, z, x+w/2.0, y+h, z])
   else:
      pos = (w-h/2.0) * percent
      drawPath(move(scale(box, h/2.0, h, 0.0), x+pos, y, z))
      drawLine([x, y, z, x, y+h, z])
      drawLine([x+w, y, z, x+w, y+h, z])
      drawLine([x, y+h/2.0, z, x+w, y+h/2.0, z])
   
   # set renderer variables back to their original values
   viewStart(rv.viewStart)
   viewEnd(rv.viewEnd)
   orthographicMode(rv.orthographic)
   fogMode(rv.fog)

   return


class labelCtrl:
   def __init__(self, text, size, x, y, border):
      self.text = text
      self.size = size
      self.x = x
      self.y = y
      self.border = border

def createLabel(text, size, x, y, border):
   return labelCtrl(text, size, x, y, border)

def onLabel(label, xMouse, yMouse):
   
   width = (len(label.text) + 1.0) * (label.size / 2.0)
   height = label.size
   
   left = label.x
   right = label.x + width
   top = label.y + height
   bottom = label.y
   
   if xMouse >= left and xMouse <= right and yMouse >= bottom and yMouse <= top:
      return True
   
   return False

def drawLabel(label, rv):
   box = [0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0]
   
   # set renderer variables for drawing the control
   topMost = 10000
   viewStart(topMost+1.0)
   viewEnd(topMost-1.0)
   orthographicMode(ON)
   fogMode(OFF)
   
   # draw border
   if label.border:
      length = (len(label.text) + 1.0) * (label.size / 2.0)
      frame = scale(box, length, label.size, 0.0)
      drawPath(move(frame, label.x, label.y-1.0, topMost))

   # draw label text
   chrLoc = 0
   chSz = label.size / 2.0
   for ch in label.text:
      
      if ch == ' ':
         chrLoc += 1
         continue
      
      ch = ord(ch) - 33
      
      for pth in range(0, len(asciiPaths[ch])):
         drawPath(move(scale(move(asciiPaths[ch][pth], chrLoc+1.0, 1.0, 0.0), chSz, chSz, chSz), label.x, label.y, topMost))
      
      chrLoc += 1
   
   # set renderer variables back to their original values
   viewStart(rv.viewStart)
   viewEnd(rv.viewEnd)
   orthographicMode(rv.orthographic)
   fogMode(rv.fog)

   return
