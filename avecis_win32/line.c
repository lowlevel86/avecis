#include <math.h>
#include <stdio.h>
#include <float.h>

struct rendrInfo
{
   int xWin;
   int yWin;
   float xCenter;
   float yCenter;
   float camLenZ;
   float camEndZ;
   float fogBgnZ;
   float fogEndZ;
   float perspctv;
   int ortho;
   float **zIndex;
   int **scrnBuff;
};
typedef struct rendrInfo RENDRINFO;


int colorTransparency(int mainColor, int bgColor, float transp)
{
   int r, g, b;
   
   r = transp * ((bgColor & 0xFF) - (mainColor & 0xFF)) + (mainColor & 0xFF);
   g = transp * (((bgColor & 0xFF00) >> 8) - ((mainColor & 0xFF00) >> 8)) + ((mainColor & 0xFF00) >> 8);
   b = transp * (((bgColor & 0xFF0000) >> 16) - ((mainColor & 0xFF0000) >> 16)) + ((mainColor & 0xFF0000) >> 16);
   
   mainColor = (int)b<<16 | (int)g<<8 | (int)r;
   
   return(mainColor);
}


void aPix(float x1, float y1, float z, int color, struct rendrInfo rI)
{
   int x, y;
   float x2, y2, transparence;
   int intX = (int)x1;
   int intY = (int)y1;
   
   x2 = 1 - (x1 - (float)intX);
   y2 = 1 - (y1 - (float)intY);
   x = (int)x1;
   y = (int)y1;
   transparence = 1 - x2 * y2;
   if ((x >= 0) && (x < rI.xWin) &&
       (y >= 0) && (y < rI.yWin))
   rI.scrnBuff[x][y] = colorTransparency(color, rI.scrnBuff[x][y], transparence);
   
   x2 = x1 - (float)intX;
   y2 = y1 - (float)intY;
   x = (int)x1 + 1;
   y = (int)y1 + 1;
   transparence = 1 - x2 * y2;
	if ((x >= 0) && (x < rI.xWin) &&
       (y >= 0) && (y < rI.yWin))
   rI.scrnBuff[x][y] = colorTransparency(color, rI.scrnBuff[x][y], transparence);
   
   x2 = x1 - (float)intX;
   y2 = 1 - (y1 - (float)intY);
   x = (int)x1 + 1;
   y = (int)y1;
   transparence = 1 - x2 * y2;
   if ((x >= 0) && (x < rI.xWin) &&
       (y >= 0) && (y < rI.yWin))
   rI.scrnBuff[x][y] = colorTransparency(color, rI.scrnBuff[x][y], transparence);
   
   x2 = 1 - (x1 - (float)intX);
   y2 = y1 - (float)intY;
   x = (int)x1;
   y = (int)y1 + 1;
   transparence = 1 - x2 * y2;
   if ((x >= 0) && (x < rI.xWin) &&
       (y >= 0) && (y < rI.yWin))
   rI.scrnBuff[x][y] = colorTransparency(color, rI.scrnBuff[x][y], transparence);
}


void aLine(float xA, float yA, float zA,
           float xB, float yB, float zB, int color, struct rendrInfo rI)
{
	int i, length;
   float x, y, z;
   float perspctvA, perspctvB;
   float xAB = 0, yAB = 0, zAB = 0;

   if ((rI.perspctv <= rI.camLenZ) && (!rI.ortho))
   return;
   if ((zA > rI.camLenZ) && (zB > rI.camLenZ))
   return;
   if ((zA < rI.camEndZ) && (zB < rI.camEndZ))
   return;

   if (zA > rI.camLenZ)
   {
      xA = xB - (rI.camLenZ - zB) / (zA - zB) * (xB - xA);
      yA = yB - (rI.camLenZ - zB) / (zA - zB) * (yB - yA);
      zA = rI.camLenZ;
   }
   if (zB > rI.camLenZ)
   {
      xB = xA - (rI.camLenZ - zA) / (zB - zA) * (xA - xB);
      yB = yA - (rI.camLenZ - zA) / (zB - zA) * (yA - yB);
      zB = rI.camLenZ;
   }

   if (zA < rI.camEndZ)
   {
      xA = xB - (zB - rI.camEndZ) / (zB - zA) * (xB - xA);
      yA = yB - (zB - rI.camEndZ) / (zB - zA) * (yB - yA);
      zA = rI.camEndZ;
   }
   if (zB < rI.camEndZ)
   {
      xB = xA - (zA - rI.camEndZ) / (zA - zB) * (xA - xB);
      yB = yA - (zA - rI.camEndZ) / (zA - zB) * (yA - yB);
      zB = rI.camEndZ;
   }

   if (!rI.ortho)
   {
      perspctvA = rI.perspctv / (rI.perspctv - zA);
      perspctvB = rI.perspctv / (rI.perspctv - zB);
      
      xA = xA * perspctvA;
      yA = yA * perspctvA;
      zA = zA * perspctvA;

      xB = xB * perspctvB;
      yB = yB * perspctvB;
      zB = zB * perspctvB;
   }

   xA = xA + rI.xCenter;
   yA = yA + rI.yCenter;
   xB = xB + rI.xCenter;
   yB = yB + rI.yCenter;

   if (xA <= -1)
   {
      zA = zB - (xB - -1) / (xB - xA) * (zB - zA);
      yA = yB - (xB - -1) / (xB - xA) * (yB - yA);
      xA = -1;
   }
   if (xA >= rI.xWin)
   {
      zA = zB - (xB - rI.xWin) / (xB - xA) * (zB - zA);
      yA = yB - (xB - rI.xWin) / (xB - xA) * (yB - yA);
      xA = rI.xWin;
   }
   if (yA <= -1)
   {
      zA = zB - (yB - -1) / (yB - yA) * (zB - zA);
      xA = xB - (yB - -1) / (yB - yA) * (xB - xA);
      yA = -1;
   }
   if (yA >= rI.yWin)
   {
      zA = zB - (yB - rI.yWin) / (yB - yA) * (zB - zA);
      xA = xB - (yB - rI.yWin) / (yB - yA) * (xB - xA);
      yA = rI.yWin;
   }

   if (xB <= -1)
   {
      zB = zA - (xA - -1) / (xA - xB) * (zA - zB);
      yB = yA - (xA - -1) / (xA - xB) * (yA - yB);
      xB = -1;
   }
   if (xB >= rI.xWin)
   {
      zB = zA - (xA - rI.xWin) / (xA - xB) * (zA - zB);
      yB = yA - (xA - rI.xWin) / (xA - xB) * (yA - yB);
      xB = rI.xWin;
   }
   if (yB <= -1)
   {
      zB = zA - (yA - -1) / (yA - yB) * (zA - zB);
      xB = xA - (yA - -1) / (yA - yB) * (xA - xB);
      yB = -1;
   }
   if (yB >= rI.yWin)
   {
      zB = zA - (yA - rI.yWin) / (yA - yB) * (zA - zB);
      xB = xA - (yA - rI.yWin) / (yA - yB) * (xA - xB);
      yB = rI.yWin;
   }

   if (fabs(round(xB) - round(xA)) > fabs(round(yB) - round(yA)))
   length = fabs(round(xB) - round(xA));
   else
   length = fabs(round(yB) - round(yA));
   
   if (xA != xB)
   xAB = (xB - xA) / length;
   if (yA != yB)
   yAB = (yB - yA) / length;
   if (zA != zB)
   zAB = (zB - zA) / length;
   
   for (i = 0; i <= length; i++)
   {
      x = xAB * i + xA;
      y = yAB * i + yA;
      z = zAB * i + zA;
      
      if ((round(x) >= 0) && (round(x) < rI.xWin) &&
          (round(y) >= 0) && (round(y) < rI.yWin))
      if (z > rI.zIndex[(int)round(x)][(int)round(y)])
      {
         rI.zIndex[(int)round(x)][(int)round(y)] = z;
         aPix(x, y, z, color, rI);
      }
   }
}


void afLine(float xA, float yA, float zA,
            float xB, float yB, float zB, int color, int fogColor, struct rendrInfo rI)
{
	int i, length;
   float x, y, z, zCoor;
   float fogBgnZ_persp, fogEndZ_persp, fogDensity;
   float perspctvA, perspctvB;
   float xAB = 0, yAB = 0, zAB = 0;

   if ((rI.perspctv <= rI.camLenZ) && (!rI.ortho))
   return;
   if ((zA > rI.camLenZ) && (zB > rI.camLenZ))
   return;
   if ((zA < rI.camEndZ) && (zB < rI.camEndZ))
   return;

   if (zA > rI.camLenZ)
   {
      xA = xB - (rI.camLenZ - zB) / (zA - zB) * (xB - xA);
      yA = yB - (rI.camLenZ - zB) / (zA - zB) * (yB - yA);
      zA = rI.camLenZ;
   }
   if (zB > rI.camLenZ)
   {
      xB = xA - (rI.camLenZ - zA) / (zB - zA) * (xA - xB);
      yB = yA - (rI.camLenZ - zA) / (zB - zA) * (yA - yB);
      zB = rI.camLenZ;
   }

   if (zA < rI.camEndZ)
   {
      xA = xB - (zB - rI.camEndZ) / (zB - zA) * (xB - xA);
      yA = yB - (zB - rI.camEndZ) / (zB - zA) * (yB - yA);
      zA = rI.camEndZ;
   }
   if (zB < rI.camEndZ)
   {
      xB = xA - (zA - rI.camEndZ) / (zA - zB) * (xA - xB);
      yB = yA - (zA - rI.camEndZ) / (zA - zB) * (yA - yB);
      zB = rI.camEndZ;
   }

   if (!rI.ortho)
   {
      perspctvA = rI.perspctv / (rI.perspctv - zA);
      perspctvB = rI.perspctv / (rI.perspctv - zB);

      xA = xA * perspctvA;
      yA = yA * perspctvA;
      zA = zA * perspctvA;

      xB = xB * perspctvB;
      yB = yB * perspctvB;
      zB = zB * perspctvB;
   }

   xA = xA + rI.xCenter;
   yA = yA + rI.yCenter;
   xB = xB + rI.xCenter;
   yB = yB + rI.yCenter;

   if (xA <= -1)
   {
      zA = zB - (xB - -1) / (xB - xA) * (zB - zA);
      yA = yB - (xB - -1) / (xB - xA) * (yB - yA);
      xA = -1;
   }
   if (xA >= rI.xWin)
   {
      zA = zB - (xB - rI.xWin) / (xB - xA) * (zB - zA);
      yA = yB - (xB - rI.xWin) / (xB - xA) * (yB - yA);
      xA = rI.xWin;
   }
   if (yA <= -1)
   {
      zA = zB - (yB - -1) / (yB - yA) * (zB - zA);
      xA = xB - (yB - -1) / (yB - yA) * (xB - xA);
      yA = -1;
   }
   if (yA >= rI.yWin)
   {
      zA = zB - (yB - rI.yWin) / (yB - yA) * (zB - zA);
      xA = xB - (yB - rI.yWin) / (yB - yA) * (xB - xA);
      yA = rI.yWin;
   }

   if (xB <= -1)
   {
      zB = zA - (xA - -1) / (xA - xB) * (zA - zB);
      yB = yA - (xA - -1) / (xA - xB) * (yA - yB);
      xB = -1;
   }
   if (xB >= rI.xWin)
   {
      zB = zA - (xA - rI.xWin) / (xA - xB) * (zA - zB);
      yB = yA - (xA - rI.xWin) / (xA - xB) * (yA - yB);
      xB = rI.xWin;
   }
   if (yB <= -1)
   {
      zB = zA - (yA - -1) / (yA - yB) * (zA - zB);
      xB = xA - (yA - -1) / (yA - yB) * (xA - xB);
      yB = -1;
   }
   if (yB >= rI.yWin)
   {
      zB = zA - (yA - rI.yWin) / (yA - yB) * (zA - zB);
      xB = xA - (yA - rI.yWin) / (yA - yB) * (xA - xB);
      yB = rI.yWin;
   }

   if (fabs(round(xB) - round(xA)) > fabs(round(yB) - round(yA)))
   length = fabs(round(xB) - round(xA));
   else
   length = fabs(round(yB) - round(yA));
   
   if (xA != xB)
   xAB = (xB - xA) / length;
   if (yA != yB)
   yAB = (yB - yA) / length;
   if (zA != zB)
   zAB = (zB - zA) / length;
   
   // calculate the fog given the perspective
   if (!rI.ortho)
   {
      if (rI.fogBgnZ >= rI.perspctv)
      fogBgnZ_persp = FLT_MAX;
      else
      fogBgnZ_persp = rI.fogBgnZ * (rI.perspctv / (rI.perspctv - rI.fogBgnZ));
      
      if (rI.fogEndZ >= rI.perspctv)
      fogEndZ_persp = FLT_MAX;
      else
      fogEndZ_persp = rI.fogEndZ * (rI.perspctv / (rI.perspctv - rI.fogEndZ));
   }
   
   for (i = 0; i <= length; i++)
   {
      x = xAB * i + xA;
      y = yAB * i + yA;
      z = zAB * i + zA;
      
      // calculate fog per point
      if (!rI.ortho)
      {
         zCoor = (rI.perspctv * z) / (rI.perspctv + z); // undo perspective
         
         if (rI.fogBgnZ <= rI.fogEndZ)
         fogDensity = 0.0;
         else
         fogDensity = (rI.fogBgnZ - zCoor) / (rI.fogBgnZ - rI.fogEndZ);
         
         if (z >= fogBgnZ_persp)
         fogDensity = 0.0;
         
         if (z <= fogEndZ_persp)
         fogDensity = 1.0;
      }
      else
      {
         if (rI.fogBgnZ <= rI.fogEndZ)
         fogDensity = 0.0;
         else
         fogDensity = (rI.fogBgnZ - z) / (rI.fogBgnZ - rI.fogEndZ);
         
         if (z >= rI.fogBgnZ)
         fogDensity = 0.0;
         
         if (z <= rI.fogEndZ)
         fogDensity = 1.0;
      }
      
      if ((round(x) >= 0) && (round(x) < rI.xWin) &&
          (round(y) >= 0) && (round(y) < rI.yWin))
      if (z > rI.zIndex[(int)round(x)][(int)round(y)])
      {
         rI.zIndex[(int)round(x)][(int)round(y)] = z;
         aPix(x, y, z, colorTransparency(color, fogColor, fogDensity), rI);
      }
   }
}


void line(float xA, float yA, float zA,
          float xB, float yB, float zB, int color, struct rendrInfo rI)
{
	int i, length, x, y;
   float z;
   float perspctvA, perspctvB;
   float xAB = 0, yAB = 0, zAB = 0;

   if ((rI.perspctv <= rI.camLenZ) && (!rI.ortho))
   return;
   if ((zA > rI.camLenZ) && (zB > rI.camLenZ))
   return;
   if ((zA < rI.camEndZ) && (zB < rI.camEndZ))
   return;

   if (zA > rI.camLenZ)
   {
      xA = xB - (rI.camLenZ - zB) / (zA - zB) * (xB - xA);
      yA = yB - (rI.camLenZ - zB) / (zA - zB) * (yB - yA);
      zA = rI.camLenZ;
   }
   if (zB > rI.camLenZ)
   {
      xB = xA - (rI.camLenZ - zA) / (zB - zA) * (xA - xB);
      yB = yA - (rI.camLenZ - zA) / (zB - zA) * (yA - yB);
      zB = rI.camLenZ;
   }

   if (zA < rI.camEndZ)
   {
      xA = xB - (zB - rI.camEndZ) / (zB - zA) * (xB - xA);
      yA = yB - (zB - rI.camEndZ) / (zB - zA) * (yB - yA);
      zA = rI.camEndZ;
   }
   if (zB < rI.camEndZ)
   {
      xB = xA - (zA - rI.camEndZ) / (zA - zB) * (xA - xB);
      yB = yA - (zA - rI.camEndZ) / (zA - zB) * (yA - yB);
      zB = rI.camEndZ;
   }

   if (!rI.ortho)
   {
      perspctvA = rI.perspctv / (rI.perspctv - zA);
      perspctvB = rI.perspctv / (rI.perspctv - zB);

      xA = xA * perspctvA;
      yA = yA * perspctvA;
      zA = zA * perspctvA;

      xB = xB * perspctvB;
      yB = yB * perspctvB;
      zB = zB * perspctvB;
   }

   xA = xA + rI.xCenter;
   yA = yA + rI.yCenter;
   xB = xB + rI.xCenter;
   yB = yB + rI.yCenter;

   if (xA <= -1)
   {
      zA = zB - (xB - -1) / (xB - xA) * (zB - zA);
      yA = yB - (xB - -1) / (xB - xA) * (yB - yA);
      xA = -1;
   }
   if (xA >= rI.xWin)
   {
      zA = zB - (xB - rI.xWin) / (xB - xA) * (zB - zA);
      yA = yB - (xB - rI.xWin) / (xB - xA) * (yB - yA);
      xA = rI.xWin;
   }
   if (yA <= -1)
   {
      zA = zB - (yB - -1) / (yB - yA) * (zB - zA);
      xA = xB - (yB - -1) / (yB - yA) * (xB - xA);
      yA = -1;
   }
   if (yA >= rI.yWin)
   {
      zA = zB - (yB - rI.yWin) / (yB - yA) * (zB - zA);
      xA = xB - (yB - rI.yWin) / (yB - yA) * (xB - xA);
      yA = rI.yWin;
   }

   if (xB <= -1)
   {
      zB = zA - (xA - -1) / (xA - xB) * (zA - zB);
      yB = yA - (xA - -1) / (xA - xB) * (yA - yB);
      xB = -1;
   }
   if (xB >= rI.xWin)
   {
      zB = zA - (xA - rI.xWin) / (xA - xB) * (zA - zB);
      yB = yA - (xA - rI.xWin) / (xA - xB) * (yA - yB);
      xB = rI.xWin;
   }
   if (yB <= -1)
   {
      zB = zA - (yA - -1) / (yA - yB) * (zA - zB);
      xB = xA - (yA - -1) / (yA - yB) * (xA - xB);
      yB = -1;
   }
   if (yB >= rI.yWin)
   {
      zB = zA - (yA - rI.yWin) / (yA - yB) * (zA - zB);
      xB = xA - (yA - rI.yWin) / (yA - yB) * (xA - xB);
      yB = rI.yWin;
   }

   if (fabs(round(xB) - round(xA)) > fabs(round(yB) - round(yA)))
   length = fabs(round(xB) - round(xA));
   else
   length = fabs(round(yB) - round(yA));
   
   if (xA != xB)
   xAB = (xB - xA) / length;
   if (yA != yB)
   yAB = (yB - yA) / length;
   if (zA != zB)
   zAB = (zB - zA) / length;
   
   for (i = 0; i <= length; i++)
   {
      x = round(xAB * i + xA);
      y = round(yAB * i + yA);
      z = zAB * i + zA;
      
      if ((x >= 0) && (x < rI.xWin) && (y >= 0) && (y < rI.yWin))
      if (z >= rI.zIndex[x][y])
      {
         rI.scrnBuff[x][y] = color;
         rI.zIndex[x][y] = z;
      }
   }
}


void fLine(float xA, float yA, float zA,
           float xB, float yB, float zB,
           int color, int fogColor, struct rendrInfo rI)
{
   int i, length, x, y;
   float z, fogDensity, zCoor;
   float fogBgnZ_persp, fogEndZ_persp;
   float perspctvA, perspctvB;
   float xAB = 0, yAB = 0, zAB = 0;
   
   if ((rI.perspctv <= rI.camLenZ) && (!rI.ortho))
   return;
   if ((zA > rI.camLenZ) && (zB > rI.camLenZ))
   return;
   if ((zA < rI.camEndZ) && (zB < rI.camEndZ))
   return;

   if (zA > rI.camLenZ)
   {
      xA = xB - (rI.camLenZ - zB) / (zA - zB) * (xB - xA);
      yA = yB - (rI.camLenZ - zB) / (zA - zB) * (yB - yA);
      zA = rI.camLenZ;
   }
   if (zB > rI.camLenZ)
   {
      xB = xA - (rI.camLenZ - zA) / (zB - zA) * (xA - xB);
      yB = yA - (rI.camLenZ - zA) / (zB - zA) * (yA - yB);
      zB = rI.camLenZ;
   }

   if (zA < rI.camEndZ)
   {
      xA = xB - (zB - rI.camEndZ) / (zB - zA) * (xB - xA);
      yA = yB - (zB - rI.camEndZ) / (zB - zA) * (yB - yA);
      zA = rI.camEndZ;
   }
   if (zB < rI.camEndZ)
   {
      xB = xA - (zA - rI.camEndZ) / (zA - zB) * (xA - xB);
      yB = yA - (zA - rI.camEndZ) / (zA - zB) * (yA - yB);
      zB = rI.camEndZ;
   }

   if (!rI.ortho)
   {
      perspctvA = rI.perspctv / (rI.perspctv - zA);
      perspctvB = rI.perspctv / (rI.perspctv - zB);

      xA = xA * perspctvA;
      yA = yA * perspctvA;
      zA = zA * perspctvA;

      xB = xB * perspctvB;
      yB = yB * perspctvB;
      zB = zB * perspctvB;
   }

   xA = xA + rI.xCenter;
   yA = yA + rI.yCenter;
   xB = xB + rI.xCenter;
   yB = yB + rI.yCenter;

   if (xA <= -1)
   {
      zA = zB - (xB - -1) / (xB - xA) * (zB - zA);
      yA = yB - (xB - -1) / (xB - xA) * (yB - yA);
      xA = -1;
   }
   if (xA >= rI.xWin)
   {
      zA = zB - (xB - rI.xWin) / (xB - xA) * (zB - zA);
      yA = yB - (xB - rI.xWin) / (xB - xA) * (yB - yA);
      xA = rI.xWin;
   }
   if (yA <= -1)
   {
      zA = zB - (yB - -1) / (yB - yA) * (zB - zA);
      xA = xB - (yB - -1) / (yB - yA) * (xB - xA);
      yA = -1;
   }
   if (yA >= rI.yWin)
   {
      zA = zB - (yB - rI.yWin) / (yB - yA) * (zB - zA);
      xA = xB - (yB - rI.yWin) / (yB - yA) * (xB - xA);
      yA = rI.yWin;
   }

   if (xB <= -1)
   {
      zB = zA - (xA - -1) / (xA - xB) * (zA - zB);
      yB = yA - (xA - -1) / (xA - xB) * (yA - yB);
      xB = -1;
   }
   if (xB >= rI.xWin)
   {
      zB = zA - (xA - rI.xWin) / (xA - xB) * (zA - zB);
      yB = yA - (xA - rI.xWin) / (xA - xB) * (yA - yB);
      xB = rI.xWin;
   }
   if (yB <= -1)
   {
      zB = zA - (yA - -1) / (yA - yB) * (zA - zB);
      xB = xA - (yA - -1) / (yA - yB) * (xA - xB);
      yB = -1;
   }
   if (yB >= rI.yWin)
   {
      zB = zA - (yA - rI.yWin) / (yA - yB) * (zA - zB);
      xB = xA - (yA - rI.yWin) / (yA - yB) * (xA - xB);
      yB = rI.yWin;
   }

   if (fabs(round(xB) - round(xA)) > fabs(round(yB) - round(yA)))
   length = fabs(round(xB) - round(xA));
   else
   length = fabs(round(yB) - round(yA));
   
   if (xA != xB)
   xAB = (xB - xA) / length;
   if (yA != yB)
   yAB = (yB - yA) / length;
   if (zA != zB)
   zAB = (zB - zA) / length;
   
   // calculate the fog given the perspective
   if (!rI.ortho)
   {
      if (rI.fogBgnZ >= rI.perspctv)
      fogBgnZ_persp = FLT_MAX;
      else
      fogBgnZ_persp = rI.fogBgnZ * (rI.perspctv / (rI.perspctv - rI.fogBgnZ));
      
      if (rI.fogEndZ >= rI.perspctv)
      fogEndZ_persp = FLT_MAX;
      else
      fogEndZ_persp = rI.fogEndZ * (rI.perspctv / (rI.perspctv - rI.fogEndZ));
   }
   
   for (i = 0; i <= length; i++)
   {
      x = round(xAB * i + xA);
      y = round(yAB * i + yA);
      z = zAB * i + zA;
      
      // calculate fog per pixel
      if (!rI.ortho)
      {
         zCoor = (rI.perspctv * z) / (rI.perspctv + z); // undo perspective
         
         if (rI.fogBgnZ <= rI.fogEndZ)
         fogDensity = 0.0;
         else
         fogDensity = (rI.fogBgnZ - zCoor) / (rI.fogBgnZ - rI.fogEndZ);
         
         if (z >= fogBgnZ_persp)
         fogDensity = 0.0;
         
         if (z <= fogEndZ_persp)
         fogDensity = 1.0;
      }
      else
      {
         if (rI.fogBgnZ <= rI.fogEndZ)
         fogDensity = 0.0;
         else
         fogDensity = (rI.fogBgnZ - z) / (rI.fogBgnZ - rI.fogEndZ);
         
         if (z >= rI.fogBgnZ)
         fogDensity = 0.0;
         
         if (z <= rI.fogEndZ)
         fogDensity = 1.0;
      }
      
      if ((x >= 0) && (x < rI.xWin) && (y >= 0) && (y < rI.yWin))
      if (z >= rI.zIndex[x][y])
      {
         rI.scrnBuff[x][y] = colorTransparency(color, fogColor, fogDensity);
         rI.zIndex[x][y] = z;
      }
   }
}
