#include <math.h>

//this code contains functions for rotating objects using unit circle coordinates
//a specific rotation sequence (x, y, then z) is used to represent an object's orientation


#define XH 0
#define XV 1
#define YH 2
#define YV 3
#define ZH 4
#define ZV 5


// get unit circle coordinates that represent degree rotations
void getUc(float xDegrees, float yDegrees, float zDegrees, float *UC)
{
   UC[0] = cos(xDegrees * (M_PI * 2.0 / 360.0));
   UC[1] = sin(xDegrees * (M_PI * 2.0 / 360.0));
   
   UC[2] = cos(yDegrees * (M_PI * 2.0 / 360.0));
   UC[3] = sin(yDegrees * (M_PI * 2.0 / 360.0));
   
   UC[4] = cos(zDegrees * (M_PI * 2.0 / 360.0));
   UC[5] = sin(zDegrees * (M_PI * 2.0 / 360.0));
}


// rotate using unit circle coordinates
void ucRot(float hUc, float vUc, float *hP, float *vP)
{
   float h, v;
   float hLine1, vLine1;
   float hLine2, vLine2;
   
   hLine1 = hUc;
   vLine1 = vUc;
   hLine2 = -vUc;
   vLine2 = hUc;
   
   h = *vP * hLine2 + *hP * vLine2;
   v = *hP * vLine1 + *vP * hLine1;
   
   *hP = h;
   *vP = v;
}


// apply x, y, and z rotations to a vertex
void ucRotVertex(float *UC, float *xP, float *yP, float *zP)
{
   ucRot(UC[0], UC[1], &*yP, &*zP);// x-axis
   ucRot(UC[2], -UC[3], &*xP, &*zP);// y-axis  negative to rotate in opposite direction
   ucRot(UC[4], UC[5], &*xP, &*yP);// z-axis
}


// reverse the applied x, y, and z rotations to a vertex
void ucRotVertexUNDO(float *UC, float *xP, float *yP, float *zP)
{
   ucRot(UC[4], -UC[5], &*xP, &*yP);// z-axis
   ucRot(UC[2], UC[3], &*xP, &*zP);// y-axis  positive direction
   ucRot(UC[0], -UC[1], &*yP, &*zP);// x-axis
}


// apply x, y, and z rotations to another set of unit circle coordinates
void ucRotUc(float *UCA, float *UCB)
{
   float xPt_X_AXIS = 1.0, yPt_X_AXIS = 0.0, zPt_X_AXIS = 0.0;
   float xPt_Y_AXIS = 0.0, yPt_Y_AXIS = 1.0, zPt_Y_AXIS = 0.0;
   float length;

   // rotate a dual-axis object using the unit circle coordinates
   ucRotVertex(&UCB[0], &xPt_X_AXIS, &yPt_X_AXIS, &zPt_X_AXIS);
   ucRotVertex(&UCB[0], &xPt_Y_AXIS, &yPt_Y_AXIS, &zPt_Y_AXIS);

   ucRotVertex(&UCA[0], &xPt_X_AXIS, &yPt_X_AXIS, &zPt_X_AXIS);
   ucRotVertex(&UCA[0], &xPt_Y_AXIS, &yPt_Y_AXIS, &zPt_Y_AXIS);


   // normalize the dual-axis object to get the new xyz unit circle coordinates
   length = sqrt(xPt_X_AXIS * xPt_X_AXIS + yPt_X_AXIS * yPt_X_AXIS);
   
   if (length == 0.0)// no rotation if the line is parallel with the z-axis
   {
      UCB[ZH] = 1.0;
      UCB[ZV] = 0.0;
   }
   else
   {
      UCB[ZH] = xPt_X_AXIS / length;
      UCB[ZV] = yPt_X_AXIS / length;
   }
   
   ucRot(UCB[ZH], -UCB[ZV], &xPt_X_AXIS, &yPt_X_AXIS);// z-axis
   ucRot(UCB[ZH], -UCB[ZV], &xPt_Y_AXIS, &yPt_Y_AXIS);// z-axis
   
   
   length = sqrt(xPt_X_AXIS * xPt_X_AXIS + zPt_X_AXIS * zPt_X_AXIS);
   
   if (length == 0.0)// no rotation if the line is parallel with the y-axis
   {
      UCB[YH] = 1.0;
      UCB[YV] = 0.0;
   }
   else
   {
      UCB[YH] = xPt_X_AXIS / length;
      UCB[YV] = -zPt_X_AXIS / length;// negative to rotate in opposite direction
   }
   
   ucRot(UCB[YH], UCB[YV], &xPt_X_AXIS, &zPt_X_AXIS);// y-axis
   ucRot(UCB[YH], UCB[YV], &xPt_Y_AXIS, &zPt_Y_AXIS);// y-axis
   
   
   length = sqrt(yPt_Y_AXIS * yPt_Y_AXIS + zPt_Y_AXIS * zPt_Y_AXIS);
   
   if (length == 0.0)// no rotation if the line is parallel with the x-axis
   {
      UCB[XH] = 1.0;
      UCB[XV] = 0.0;
   }
   else
   {
      UCB[XH] = yPt_Y_AXIS / length;
      UCB[XV] = zPt_Y_AXIS / length;
   }
}


// reverse apply a rotation to another and recalculate the new xyz unit circle coordinates
void ucRotUcUNDO(float *UCA, float *UCB)
{
   float xPt_X_AXIS = 1.0, yPt_X_AXIS = 0.0, zPt_X_AXIS = 0.0;
   float xPt_Y_AXIS = 0.0, yPt_Y_AXIS = 1.0, zPt_Y_AXIS = 0.0;
   float length;

   // rotate a dual-axis object using the unit circle coordinates
   ucRotVertex(&UCB[0], &xPt_X_AXIS, &yPt_X_AXIS, &zPt_X_AXIS);
   ucRotVertex(&UCB[0], &xPt_Y_AXIS, &yPt_Y_AXIS, &zPt_Y_AXIS);

   // reverse
   ucRotVertexUNDO(&UCA[0], &xPt_X_AXIS, &yPt_X_AXIS, &zPt_X_AXIS);
   ucRotVertexUNDO(&UCA[0], &xPt_Y_AXIS, &yPt_Y_AXIS, &zPt_Y_AXIS);


   // normalize the dual-axis object to get the new xyz unit circle coordinates
   length = sqrt(xPt_X_AXIS * xPt_X_AXIS + yPt_X_AXIS * yPt_X_AXIS);
   
   if (length == 0.0)// no rotation if the line is parallel with the z-axis
   {
      UCB[ZH] = 1.0;
      UCB[ZV] = 0.0;
   }
   else
   {
      UCB[ZH] = xPt_X_AXIS / length;
      UCB[ZV] = yPt_X_AXIS / length;
   }
   
   ucRot(UCB[ZH], -UCB[ZV], &xPt_X_AXIS, &yPt_X_AXIS);// z-axis
   ucRot(UCB[ZH], -UCB[ZV], &xPt_Y_AXIS, &yPt_Y_AXIS);// z-axis
   
   
   length = sqrt(xPt_X_AXIS * xPt_X_AXIS + zPt_X_AXIS * zPt_X_AXIS);
   
   if (length == 0.0)// no rotation if the line is parallel with the y-axis
   {
      UCB[YH] = 1.0;
      UCB[YV] = 0.0;
   }
   else
   {
      UCB[YH] = xPt_X_AXIS / length;
      UCB[YV] = -zPt_X_AXIS / length;// negative to rotate in opposite direction
   }
   
   ucRot(UCB[YH], UCB[YV], &xPt_X_AXIS, &zPt_X_AXIS);// y-axis
   ucRot(UCB[YH], UCB[YV], &xPt_Y_AXIS, &zPt_Y_AXIS);// y-axis
   
   
   length = sqrt(yPt_Y_AXIS * yPt_Y_AXIS + zPt_Y_AXIS * zPt_Y_AXIS);
   
   if (length == 0.0)// no rotation if the line is parallel with the x-axis
   {
      UCB[XH] = 1.0;
      UCB[XV] = 0.0;
   }
   else
   {
      UCB[XH] = yPt_Y_AXIS / length;
      UCB[XV] = zPt_Y_AXIS / length;
   }
}


// apply x, y, and z rotations to a vertex using a 2D vector
void vecRotVertex(float x, float y, float *xPt, float *yPt, float *zPt)
{
   float length;
   float zUcH, zUcV;
   float yUcH, yUcV;
   
   // get the length representing how much to rotate
   length = sqrt(x * x + y * y); // degrees
   
   if (length == 0.0)
   {
      zUcH = 1.0;
      zUcV = 0.0;
   }
   else
   {
      zUcH = -x / length;
      zUcV = -y / length;
   }
   
   // rotate vector to the x-axis
   ucRot(zUcH, -zUcV, &*xPt, &*yPt); // z-axis rotation
   
   // rotate the amount of length
   yUcH = cos(length * (M_PI * 2.0 / 360.0));
   yUcV = sin(length * (M_PI * 2.0 / 360.0));
   ucRot(yUcH, yUcV, &*xPt, &*zPt); // y-axis rotation
   
   // rotate back from the x-axis
   ucRot(zUcH, zUcV, &*xPt, &*yPt); // z-axis rotation
}


// apply rotations to a set of unit circle coordinates using a 2D vector
void vecRotUc(float x, float y, float *UC)
{
   float xPt_X_AXIS = 1.0, yPt_X_AXIS = 0.0, zPt_X_AXIS = 0.0;
   float xPt_Y_AXIS = 0.0, yPt_Y_AXIS = 1.0, zPt_Y_AXIS = 0.0;
   float length;

   // rotate a dual-axis object using the unit circle coordinates
   ucRotVertex(&UC[0], &xPt_X_AXIS, &yPt_X_AXIS, &zPt_X_AXIS);
   ucRotVertex(&UC[0], &xPt_Y_AXIS, &yPt_Y_AXIS, &zPt_Y_AXIS);

   // rotate the dual-axis object using the x y coordinates
   vecRotVertex(x, y, &xPt_X_AXIS, &yPt_X_AXIS, &zPt_X_AXIS);
   vecRotVertex(x, y, &xPt_Y_AXIS, &yPt_Y_AXIS, &zPt_Y_AXIS);


   // normalize the dual-axis object to get the xyz unit circle coordinates
   
   length = sqrt(xPt_X_AXIS * xPt_X_AXIS + yPt_X_AXIS * yPt_X_AXIS);
   
   if (length == 0.0)// no rotation if the line is parallel with the z-axis
   {
      UC[ZH] = 1.0;
      UC[ZV] = 0.0;
   }
   else
   {
      UC[ZH] = xPt_X_AXIS / length;
      UC[ZV] = yPt_X_AXIS / length;
   }
   
   ucRot(UC[ZH], -UC[ZV], &xPt_X_AXIS, &yPt_X_AXIS);// z-axis
   ucRot(UC[ZH], -UC[ZV], &xPt_Y_AXIS, &yPt_Y_AXIS);// z-axis
   
   
   length = sqrt(xPt_X_AXIS * xPt_X_AXIS + zPt_X_AXIS * zPt_X_AXIS);
   
   if (length == 0.0)// no rotation if the line is parallel with the y-axis
   {
      UC[YH] = 1.0;
      UC[YV] = 0.0;
   }
   else
   {
      UC[YH] = xPt_X_AXIS / length;
      UC[YV] = -zPt_X_AXIS / length;// negative to rotate in opposite direction
   }
   
   ucRot(UC[YH], UC[YV], &xPt_X_AXIS, &zPt_X_AXIS);// y-axis
   ucRot(UC[YH], UC[YV], &xPt_Y_AXIS, &zPt_Y_AXIS);// y-axis
   
   
   length = sqrt(yPt_Y_AXIS * yPt_Y_AXIS + zPt_Y_AXIS * zPt_Y_AXIS);
   
   if (length == 0.0)// no rotation if the line is parallel with the x-axis
   {
      UC[XH] = 1.0;
      UC[XV] = 0.0;
   }
   else
   {
      UC[XH] = yPt_Y_AXIS / length;
      UC[XV] = zPt_Y_AXIS / length;
   }
}


// apply rotations to a set of unit circle coordinates using a 2D vector on a track ball
void trackBallRotUc(float radius, float xContact, float yContact,
                    float xDestination, float yDestination, float *UC)
{
   float UC_Contact[6];
   float xTravel, yTravel, zTravel;
   float length;
   float zContact;
   
   xTravel = xDestination - xContact;
   yTravel = yDestination - yContact;
   zTravel = 0.0;
   
   // get the z coordinate of the initial contact point on the track ball
   length = sqrt(xContact * xContact + yContact * yContact);
   
   if (length < radius)
   {
      zContact = sqrt(1.0 - (length/radius) * (length/radius)); // missing circle coordinate
      zContact *= radius;
   }
   else
   {
      zContact = 0.0;
   }
   
   
   // find unit circle coordinates that will rotate the contact point vertically
   
   // find the x-axis unit circle coordinate
   length = sqrt(yContact * yContact + zContact * zContact);
   
   if (length == 0.0)// no rotation
   {
      UC_Contact[XH] = 1.0;
      UC_Contact[XV] = 0.0;
   }
   else
   {
      UC_Contact[XH] = zContact / length;
      UC_Contact[XV] = yContact / length;
   }
   
   ucRot(UC_Contact[XH], UC_Contact[XV], &yContact, &zContact);// x-axis rotation
   
   // find the y-axis unit circle coordinate
   length = sqrt(xContact * xContact + zContact * zContact);
   
   if (length == 0.0)// no rotation
   {
      UC_Contact[YH] = 1.0;
      UC_Contact[YV] = 0.0;
   }
   else
   {
      UC_Contact[YH] = zContact / length;
      UC_Contact[YV] = -xContact / length;
   }
   
   UC_Contact[ZH] = 1.0;
   UC_Contact[ZV] = 0.0;
   
   
   // apply the unit circle coordinates gotten from the contact point to the travel path
   ucRotVertex(UC_Contact, &xTravel, &yTravel, &zTravel);
   
   // rotate the contact point vertically, apply the travel path, then reverse the first rotation
   ucRotUc(UC_Contact, &UC[0]);
   vecRotUc(xTravel, yTravel, &UC[0]);
   ucRotUcUNDO(UC_Contact, &UC[0]);
}
