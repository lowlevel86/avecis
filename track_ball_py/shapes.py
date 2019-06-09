import math


def genGrid(res):
   
   grid = []
   
   for i in range(0, res+1):

      grid.append(float(i)/res * 2.0 - 1.0)
      grid.append(1.0)
      grid.append(0.0)
      grid.append(float(i)/res * 2.0 - 1.0)
      grid.append(-1.0)
      grid.append(0.0)
   
      grid.append(1.0)
      grid.append(float(i)/res * 2.0 - 1.0)
      grid.append(0.0)
      grid.append(-1.0)
      grid.append(float(i)/res * 2.0 - 1.0)
      grid.append(0.0)
      
   return grid


def genCube():
   return [-1.0, 1.0, 1.0, -1.0, 1.0,-1.0,  1.0, 1.0, 1.0,  1.0, 1.0,-1.0,\
            1.0,-1.0, 1.0,  1.0,-1.0,-1.0, -1.0,-1.0, 1.0, -1.0,-1.0,-1.0,\
            1.0,-1.0, 1.0, -1.0,-1.0, 1.0,  1.0, 1.0, 1.0, -1.0, 1.0, 1.0,\
            1.0, 1.0,-1.0, -1.0, 1.0,-1.0,  1.0,-1.0,-1.0, -1.0,-1.0,-1.0,\
            1.0, 1.0,-1.0,  1.0,-1.0,-1.0,  1.0, 1.0, 1.0,  1.0,-1.0, 1.0,\
           -1.0, 1.0, 1.0, -1.0,-1.0, 1.0, -1.0, 1.0,-1.0, -1.0,-1.0,-1.0]


def genCircle(verts):
   
   circle = []
   
   for i in range(1, verts+1):
      
      circle.append(math.cos((i-1) * (math.pi * 2.0 / verts)))
      circle.append(math.sin((i-1) * (math.pi * 2.0 / verts)))
      circle.append(0.0)
      
      circle.append(math.cos(i * (math.pi * 2.0 / verts)))
      circle.append(math.sin(i * (math.pi * 2.0 / verts)))
      circle.append(0.0)
   
   return circle


def genSphere(hRings, vRings):
   
   sphere = []
   
   # draw horizontal rings
   for j in range(1, hRings+1):
      
      height = math.cos(j * (math.pi / (hRings+1)))
      size = math.sin(j * (math.pi / (hRings+1)))

      xPt = math.cos(0 * (math.pi / vRings)) * size
      yPt = math.sin(0 * (math.pi / vRings)) * size
      zPt = height
      
      for i in range(1, vRings*2+1):
         
         sphere.append(xPt)
         sphere.append(yPt)
         sphere.append(zPt)
         
         xPt = math.cos(i * (math.pi / vRings)) * size
         yPt = math.sin(i * (math.pi / vRings)) * size
         zPt = height
         
         sphere.append(xPt)
         sphere.append(yPt)
         sphere.append(zPt)
   
   
   # draw vertical rings
   if hRings % 2: # odd or even
      rotOffset = 0.0
   else:
      rotOffset = 1.0 / 2 # rotate slightly
   
   for j in range(0, vRings):
      
      hUC = math.cos(j * (math.pi / vRings))
      vUC = math.sin(j * (math.pi / vRings))

      xPt = math.cos(rotOffset * (math.pi / (hRings+1)))
      yPt = 0.0
      zPt = math.sin(rotOffset * (math.pi / (hRings+1)))
      
      for i in range(1, (hRings+1)*2+1):
         
         hLineA = hUC
         vLineA = vUC
         hLineB = -vUC
         vLineB = hUC
         sphere.append(yPt * hLineB + xPt * vLineB)
         sphere.append(xPt * vLineA + yPt * hLineA)
         sphere.append(zPt)
      
         xPt = math.cos((i+rotOffset) * (math.pi / (hRings+1)))
         yPt = 0.0
         zPt = math.sin((i+rotOffset) * (math.pi / (hRings+1)))
         
         hLineA = hUC
         vLineA = vUC
         hLineB = -vUC
         vLineB = hUC
         sphere.append(yPt * hLineB + xPt * vLineB)
         sphere.append(xPt * vLineA + yPt * hLineA)
         sphere.append(zPt)
   
   return sphere


def genTorus(hRings, vRings, tubeDiam):
   
   torus = []
   holeSz = 2.0 - tubeDiam
   
   # draw horizontal rings
   for j in range(0, hRings):
      
      height = math.sin(j * (math.pi / (hRings/2.0))) * tubeDiam
      size = math.cos(j * (math.pi / (hRings/2.0))) * tubeDiam + holeSz
      
      xPt = math.cos(0 * (math.pi / (vRings/2.0))) * size
      yPt = math.sin(0 * (math.pi / (vRings/2.0))) * size
      zPt = height
      
      for i in range(1, vRings+1):
         
         torus.append(xPt)
         torus.append(yPt)
         torus.append(zPt)
         
         xPt = math.cos(i * (math.pi / (vRings/2.0))) * size
         yPt = math.sin(i * (math.pi / (vRings/2.0))) * size
         zPt = height
         
         torus.append(xPt)
         torus.append(yPt)
         torus.append(zPt)
   
   
   # draw vertical rings
   for j in range(0, vRings):
      
      hUC = math.cos(j * (math.pi / (vRings/2.0)))
      vUC = math.sin(j * (math.pi / (vRings/2.0)))

      xPt = math.cos(0 * (math.pi / (hRings/2.0))) * tubeDiam + holeSz
      yPt = 0.0
      zPt = math.sin(0 * (math.pi / (hRings/2.0))) * tubeDiam
      
      for i in range(1, hRings+1):
         
         hLineA = hUC
         vLineA = vUC
         hLineB = -vUC
         vLineB = hUC
         torus.append(yPt * hLineB + xPt * vLineB)
         torus.append(xPt * vLineA + yPt * hLineA)
         torus.append(zPt)
      
         xPt = math.cos((i) * (math.pi / (hRings/2.0))) * tubeDiam + holeSz
         yPt = 0.0
         zPt = math.sin((i) * (math.pi / (hRings/2.0))) * tubeDiam
         
         hLineA = hUC
         vLineA = vUC
         hLineB = -vUC
         vLineB = hUC
         torus.append(yPt * hLineB + xPt * vLineB)
         torus.append(xPt * vLineA + yPt * hLineA)
         torus.append(zPt)
   
   return torus
   