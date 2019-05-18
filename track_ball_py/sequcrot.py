import math

'''
sequcrot.py
sequcrot stands for sequence unit circle rotation

this code contains functions for rotating objects using unit circle coordinates

a specific rotation sequence (x, y, then z) is used to represent an object's orientation
'''

# get unit circle coordinates that represent degree rotations
def getUc(xDegrees, yDegrees, zDegrees):
   
   UC = []
   
   UC.append(math.cos(xDegrees * (math.pi * 2.0 / 360.0)))
   UC.append(math.sin(xDegrees * (math.pi * 2.0 / 360.0)))
   
   UC.append(math.cos(yDegrees * (math.pi * 2.0 / 360.0)))
   UC.append(math.sin(yDegrees * (math.pi * 2.0 / 360.0)))
   
   UC.append(math.cos(zDegrees * (math.pi * 2.0 / 360.0)))
   UC.append(math.sin(zDegrees * (math.pi * 2.0 / 360.0)))
   
   return UC


# rotate using unit circle coordinates
def ucRot(hUC, vUC, hPt, vPt):
   
   hLineA = hUC
   vLineA = vUC
   hLineB = -vUC
   vLineB = hUC
   
   new_hPt = vPt * hLineB + hPt * vLineB
   new_vPt = hPt * vLineA + vPt * hLineA
   
   return new_hPt, new_vPt


# apply x, y, and z rotations to a vertex
def ucRotVertex(UC, xPt, yPt, zPt):
   
   yPt, zPt = ucRot(UC[0], UC[1], yPt, zPt) # x-axis
   xPt, zPt = ucRot(UC[2], -UC[3], xPt, zPt)# y-axis negative to rotate in opposite direction
   xPt, yPt = ucRot(UC[4], UC[5], xPt, yPt) # z-axis
   
   return xPt, yPt, zPt


# reverse the applied x, y, and z rotations to a vertex
def ucRotVertexUNDO(UC, xPt, yPt, zPt):
   
   xPt, yPt = ucRot(UC[4], -UC[5], xPt, yPt)# z-axis
   xPt, zPt = ucRot(UC[2], UC[3], xPt, zPt) # y-axis positive
   yPt, zPt = ucRot(UC[0], -UC[1], yPt, zPt)# x-axis
   
   return xPt, yPt, zPt


# apply x, y, and z rotations to another set of unit circle coordinates
def ucRotUc(UC_A, UC_B):

   XH = 0; XV = 1
   YH = 2; YV = 3
   ZH = 4; ZV = 5
   xPt_X_AXIS = 1.0; yPt_X_AXIS = 0.0; zPt_X_AXIS = 0.0
   xPt_Y_AXIS = 0.0; yPt_Y_AXIS = 1.0; zPt_Y_AXIS = 0.0

   # rotate a dual-axis object using the first xyz unit circle coordinates
   xPt_X_AXIS, yPt_X_AXIS, zPt_X_AXIS = ucRotVertex(UC_B, xPt_X_AXIS, yPt_X_AXIS, zPt_X_AXIS)
   xPt_Y_AXIS, yPt_Y_AXIS, zPt_Y_AXIS = ucRotVertex(UC_B, xPt_Y_AXIS, yPt_Y_AXIS, zPt_Y_AXIS)

   # rotate the dual-axis object again using the second xyz unit circle coordinates
   xPt_X_AXIS, yPt_X_AXIS, zPt_X_AXIS = ucRotVertex(UC_A, xPt_X_AXIS, yPt_X_AXIS, zPt_X_AXIS)
   xPt_Y_AXIS, yPt_Y_AXIS, zPt_Y_AXIS = ucRotVertex(UC_A, xPt_Y_AXIS, yPt_Y_AXIS, zPt_Y_AXIS)


   # normalize the dual-axis object to get the new xyz unit circle coordinates
   
   length = math.sqrt(xPt_X_AXIS * xPt_X_AXIS + yPt_X_AXIS * yPt_X_AXIS)
   
   if (length == 0.0): # no rotation if the line is parallel with the z-axis
      UC_B[ZH] = 1.0
      UC_B[ZV] = 0.0
   else:
      UC_B[ZH] = xPt_X_AXIS / length
      UC_B[ZV] = yPt_X_AXIS / length
   
   xPt_X_AXIS, yPt_X_AXIS = ucRot(UC_B[ZH], -UC_B[ZV], xPt_X_AXIS, yPt_X_AXIS)# z-axis
   xPt_Y_AXIS, yPt_Y_AXIS = ucRot(UC_B[ZH], -UC_B[ZV], xPt_Y_AXIS, yPt_Y_AXIS)# z-axis
   
   
   length = math.sqrt(xPt_X_AXIS * xPt_X_AXIS + zPt_X_AXIS * zPt_X_AXIS)
   
   if (length == 0.0): # no rotation if the line is parallel with the y-axis
      UC_B[YH] = 1.0
      UC_B[YV] = 0.0
   else:
      UC_B[YH] = xPt_X_AXIS / length
      UC_B[YV] = -zPt_X_AXIS / length # negative to rotate in opposite direction
   
   xPt_X_AXIS, zPt_X_AXIS = ucRot(UC_B[YH], UC_B[YV], xPt_X_AXIS, zPt_X_AXIS)# y-axis
   xPt_Y_AXIS, zPt_Y_AXIS = ucRot(UC_B[YH], UC_B[YV], xPt_Y_AXIS, zPt_Y_AXIS)# y-axis
   
   
   length = math.sqrt(yPt_Y_AXIS * yPt_Y_AXIS + zPt_Y_AXIS * zPt_Y_AXIS)
   
   if (length == 0.0): # no rotation if the line is parallel with the x-axis
      UC_B[XH] = 1.0
      UC_B[XV] = 0.0
   else:
      UC_B[XH] = yPt_Y_AXIS / length
      UC_B[XV] = zPt_Y_AXIS / length

   return UC_B


# reverse the applied x, y, and z rotations to a set of unit circle coordinates
def ucRotUcUNDO(UC_A, UC_B):

   XH = 0; XV = 1
   YH = 2; YV = 3
   ZH = 4; ZV = 5
   xPt_X_AXIS = 1.0; yPt_X_AXIS = 0.0; zPt_X_AXIS = 0.0
   xPt_Y_AXIS = 0.0; yPt_Y_AXIS = 1.0; zPt_Y_AXIS = 0.0

   # rotate a dual-axis object using the first xyz unit circle coordinates
   xPt_X_AXIS, yPt_X_AXIS, zPt_X_AXIS = ucRotVertex(UC_B, xPt_X_AXIS, yPt_X_AXIS, zPt_X_AXIS)
   xPt_Y_AXIS, yPt_Y_AXIS, zPt_Y_AXIS = ucRotVertex(UC_B, xPt_Y_AXIS, yPt_Y_AXIS, zPt_Y_AXIS)

   # rotate the dual-axis object again using the second xyz unit circle coordinates
   xPt_X_AXIS, yPt_X_AXIS, zPt_X_AXIS = ucRotVertexUNDO(UC_A, xPt_X_AXIS, yPt_X_AXIS, zPt_X_AXIS)
   xPt_Y_AXIS, yPt_Y_AXIS, zPt_Y_AXIS = ucRotVertexUNDO(UC_A, xPt_Y_AXIS, yPt_Y_AXIS, zPt_Y_AXIS)


   # normalize the dual-axis object to get the new xyz unit circle coordinates
   
   length = math.sqrt(xPt_X_AXIS * xPt_X_AXIS + yPt_X_AXIS * yPt_X_AXIS)
   
   if (length == 0.0): # no rotation if the line is parallel with the z-axis
      UC_B[ZH] = 1.0
      UC_B[ZV] = 0.0
   else:
      UC_B[ZH] = xPt_X_AXIS / length
      UC_B[ZV] = yPt_X_AXIS / length
   
   xPt_X_AXIS, yPt_X_AXIS = ucRot(UC_B[ZH], -UC_B[ZV], xPt_X_AXIS, yPt_X_AXIS)# z-axis
   xPt_Y_AXIS, yPt_Y_AXIS = ucRot(UC_B[ZH], -UC_B[ZV], xPt_Y_AXIS, yPt_Y_AXIS)# z-axis
   
   
   length = math.sqrt(xPt_X_AXIS * xPt_X_AXIS + zPt_X_AXIS * zPt_X_AXIS)
   
   if (length == 0.0): # no rotation if the line is parallel with the y-axis
      UC_B[YH] = 1.0
      UC_B[YV] = 0.0
   else:
      UC_B[YH] = xPt_X_AXIS / length
      UC_B[YV] = -zPt_X_AXIS / length # negative to rotate in opposite direction
   
   xPt_X_AXIS, zPt_X_AXIS = ucRot(UC_B[YH], UC_B[YV], xPt_X_AXIS, zPt_X_AXIS)# y-axis
   xPt_Y_AXIS, zPt_Y_AXIS = ucRot(UC_B[YH], UC_B[YV], xPt_Y_AXIS, zPt_Y_AXIS)# y-axis
   
   
   length = math.sqrt(yPt_Y_AXIS * yPt_Y_AXIS + zPt_Y_AXIS * zPt_Y_AXIS)
   
   if (length == 0.0): # no rotation if the line is parallel with the x-axis
      UC_B[XH] = 1.0
      UC_B[XV] = 0.0
   else:
      UC_B[XH] = yPt_Y_AXIS / length
      UC_B[XV] = zPt_Y_AXIS / length

   return UC_B


# apply x, y, and z rotations to a vertex using a 2D vector
def vecRotVertex(x, y, xPt, yPt, zPt):

   # get the length representing how much to rotate
   length = math.sqrt(x * x + y * y) # degrees
   
   if (length == 0.0):
      xUc = 1.0
      yUc = 0.0
   else:
      xUc = -x / length
      yUc = -y / length
   
   # rotate vector to the x-axis
   xPt, yPt = ucRot(xUc, -yUc, xPt, yPt) # z-axis rotation
   
   # rotate the amount of length
   hUc = math.cos(length * (math.pi * 2.0 / 360.0))
   vUc = math.sin(length * (math.pi * 2.0 / 360.0))
   xPt, zPt = ucRot(xPt, zPt, hUc, vUc) # y-axis rotation
   
   # rotate back from the x-axis
   xPt, yPt = ucRot(xUc, yUc, xPt, yPt) # z-axis rotation
   
   return xPt, yPt, zPt


# apply rotations to a set of unit circle coordinates using a 2D vector
def vecRotUc(x, y, UC):
   
   XH = 0; XV = 1
   YH = 2; YV = 3
   ZH = 4; ZV = 5
   xPt_X_AXIS = 1.0; yPt_X_AXIS = 0.0; zPt_X_AXIS = 0.0
   xPt_Y_AXIS = 0.0; yPt_Y_AXIS = 1.0; zPt_Y_AXIS = 0.0

   # rotate a dual-axis object using the unit circle coordinates
   xPt_X_AXIS, yPt_X_AXIS, zPt_X_AXIS = ucRotVertex(UC, xPt_X_AXIS, yPt_X_AXIS, zPt_X_AXIS)
   xPt_Y_AXIS, yPt_Y_AXIS, zPt_Y_AXIS = ucRotVertex(UC, xPt_Y_AXIS, yPt_Y_AXIS, zPt_Y_AXIS)

   # rotate the dual-axis object using the x y coordinates
   xPt_X_AXIS, yPt_X_AXIS, zPt_X_AXIS = vecRotVertex(x, y, xPt_X_AXIS, yPt_X_AXIS, zPt_X_AXIS)
   xPt_Y_AXIS, yPt_Y_AXIS, zPt_Y_AXIS = vecRotVertex(x, y, xPt_Y_AXIS, yPt_Y_AXIS, zPt_Y_AXIS)


   # normalize the dual-axis object to get the xyz unit circle coordinates
   
   length = math.sqrt(xPt_X_AXIS * xPt_X_AXIS + yPt_X_AXIS * yPt_X_AXIS)
   
   if (length == 0.0): # no rotation if the line is parallel with the z-axis
      UC[ZH] = 1.0
      UC[ZV] = 0.0
   else:
      UC[ZH] = xPt_X_AXIS / length
      UC[ZV] = yPt_X_AXIS / length
   
   xPt_X_AXIS, yPt_X_AXIS = ucRot(UC[ZH], -UC[ZV], xPt_X_AXIS, yPt_X_AXIS)# z-axis
   xPt_Y_AXIS, yPt_Y_AXIS = ucRot(UC[ZH], -UC[ZV], xPt_Y_AXIS, yPt_Y_AXIS)# z-axis
   
   
   length = math.sqrt(xPt_X_AXIS * xPt_X_AXIS + zPt_X_AXIS * zPt_X_AXIS)
   
   if (length == 0.0): # no rotation if the line is parallel with the y-axis
      UC[YH] = 1.0
      UC[YV] = 0.0
   else:
      UC[YH] = xPt_X_AXIS / length
      UC[YV] = -zPt_X_AXIS / length # negative to rotate in opposite direction
   
   xPt_X_AXIS, zPt_X_AXIS = ucRot(UC[YH], UC[YV], xPt_X_AXIS, zPt_X_AXIS)# y-axis
   xPt_Y_AXIS, zPt_Y_AXIS = ucRot(UC[YH], UC[YV], xPt_Y_AXIS, zPt_Y_AXIS)# y-axis
   
   
   length = math.sqrt(yPt_Y_AXIS * yPt_Y_AXIS + zPt_Y_AXIS * zPt_Y_AXIS)
   
   if (length == 0.0): # no rotation if the line is parallel with the x-axis
      UC[XH] = 1.0
      UC[XV] = 0.0
   else:
      UC[XH] = yPt_Y_AXIS / length
      UC[XV] = zPt_Y_AXIS / length

   return UC


# apply rotations to a set of unit circle coordinates using a 2D vector on a track ball
def trackBallRotUc(radius, xContact, yContact, xDestination, yDestination, UC):
   
   UC_Contact = []
   XH = 0; XV = 1
   YH = 2; YV = 3
   ZH = 4; ZV = 5
   
   xTravel = xDestination - xContact
   yTravel = yDestination - yContact
   zTravel = 0.0
   
   # get the z coordinate of the initial contact point on the track ball
   length = math.sqrt(xContact * xContact + yContact * yContact)
   
   if (length < radius):
      zContact = math.sqrt(1.0 - (length/radius) * (length/radius)) # missing circle coordinate
      zContact *= radius
   else:
      zContact = 0.0
   
   
   # find unit circle coordinates that will rotate the contact point vertically
   
   # find the x-axis unit circle coordinate
   length = math.sqrt(yContact * yContact + zContact * zContact)
   
   if (length == 0.0):# no rotation
      UC_Contact.append(1.0)
      UC_Contact.append(0.0)
   else:
      UC_Contact.append(zContact / length)
      UC_Contact.append(yContact / length)
   
   yContact, zContact = ucRot(UC_Contact[XH], UC_Contact[XV], yContact, zContact)# x-axis rotation
   
   # find the y-axis unit circle coordinate
   length = math.sqrt(xContact * xContact + zContact * zContact)
   
   if (length == 0.0):# no rotation
      UC_Contact.append(1.0)
      UC_Contact.append(0.0)
   else:
      UC_Contact.append(zContact / length)
      UC_Contact.append(-xContact / length)
   
   UC_Contact.append(1.0)
   UC_Contact.append(0.0)
   
   
   # apply the unit circle coordinates gotten from the contact point to the travel path
   xTravel, yTravel, zTravel = ucRotVertex(UC_Contact, xTravel, yTravel, zTravel)
   
   # rotate the contact point vertically, apply the travel path, then reverse the first rotation
   UC = ucRotUc(UC_Contact, UC)
   UC = vecRotUc(xTravel, yTravel, UC)
   UC = ucRotUcUNDO(UC_Contact, UC)
   
   return UC
