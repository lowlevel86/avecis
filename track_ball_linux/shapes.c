#include <stdio.h>
#include <math.h>

int getGridSize(int res)
{
   return (res+1) * 12;
}

void genGrid(float *grid, int res)
{
   int i, j;
   
   j = 0;
   for (i=0; i <= res; i++)
   {
      grid[j+0] = (float)i/res * 2.0 - 1.0;
      grid[j+1] = 1.0;
      grid[j+2] = 0.0;
      grid[j+3] = (float)i/res * 2.0 - 1.0;
      grid[j+4] = -1.0;
      grid[j+5] = 0.0;
   
      grid[j+6] = 1.0;
      grid[j+7] = (float)i/res * 2.0 - 1.0;
      grid[j+8] = 0.0;
      grid[j+9] = -1.0;
      grid[j+10] = (float)i/res * 2.0 - 1.0;
      grid[j+11] = 0.0;
      j+=12;
   }
}

int getCubeSize()
{
   return 6 * 12;
}

void genCube(float *cube)
{
   int i;
   static float cubeData[] = {-1.0, 1.0, 1.0, -1.0, 1.0,-1.0,  1.0, 1.0, 1.0,  1.0, 1.0,-1.0,
                               1.0,-1.0, 1.0,  1.0,-1.0,-1.0, -1.0,-1.0, 1.0, -1.0,-1.0,-1.0,
                               1.0,-1.0, 1.0, -1.0,-1.0, 1.0,  1.0, 1.0, 1.0, -1.0, 1.0, 1.0,
                               1.0, 1.0,-1.0, -1.0, 1.0,-1.0,  1.0,-1.0,-1.0, -1.0,-1.0,-1.0,
                               1.0, 1.0,-1.0,  1.0,-1.0,-1.0,  1.0, 1.0, 1.0,  1.0,-1.0, 1.0,
                              -1.0, 1.0, 1.0, -1.0,-1.0, 1.0, -1.0, 1.0,-1.0, -1.0,-1.0,-1.0};
   
   for (i=0; i < 6 * 12; i++)
   {
      cube[i] = cubeData[i];
   }
}

int getCircleSize(int verts)
{
   return verts * 6;
}

void genCircle(float *circle, int verts)
{
   int i, j;
   
   j = 0;
   for (i=1; i <= verts; i++)
   {
      circle[j+0] = cos((i-1) * (M_PI * 2.0 / verts));
      circle[j+1] = sin((i-1) * (M_PI * 2.0 / verts));
      circle[j+2] = 0.0;
      
      circle[j+3] = cos(i * (M_PI * 2.0 / verts));
      circle[j+4] = sin(i * (M_PI * 2.0 / verts));
      circle[j+5] = 0.0;
      
      j += 6;
   }
}

int getSphereSize(int hRings, int vRings)
{
   return (hRings * vRings * 2) * 6 + (vRings * (hRings * 2 + 2)) * 6;
}

void genSphere(float *sphere, int hRings, int vRings)
{
   int i, j, k;
   float height, size;
   float xPt, yPt, zPt;
   float rotOffset;
   float hUC, vUC;
   float hLineA, vLineA;
   float hLineB, vLineB;
   
   k = 0;
   
   // draw horizontal rings
   for (j=1; j <= hRings; j++)
   {
      height = cos(j * (M_PI / (hRings+1)));
      size = sin(j * (M_PI / (hRings+1)));
      
      xPt = cos(0 * (M_PI / vRings)) * size;
      yPt = sin(0 * (M_PI / vRings)) * size;
      zPt = height;
      
      for (i=1; i <= vRings*2; i++)
      {
         sphere[k+0] = xPt;
         sphere[k+1] = yPt;
         sphere[k+2] = zPt;
         k += 3;
         
         xPt = cos(i * (M_PI / vRings)) * size;
         yPt = sin(i * (M_PI / vRings)) * size;
         zPt = height;
         
         sphere[k+0] = xPt;
         sphere[k+1] = yPt;
         sphere[k+2] = zPt;
         k += 3;
      }
   }
   
   // draw vertical rings
   if (hRings % 2) // odd or even
   rotOffset = 0.0;
   else
   rotOffset = 1.0 / 2; // rotate slightly
   
   for (j=0; j < vRings; j++)
   {
      hUC = cos(j * (M_PI / vRings));
      vUC = sin(j * (M_PI / vRings));

      xPt = cos(rotOffset * (M_PI / (hRings+1)));
      yPt = 0.0;
      zPt = sin(rotOffset * (M_PI / (hRings+1)));
      
      for (i=1; i <= (hRings+1)*2; i++)
      {
         hLineA = hUC;
         vLineA = vUC;
         hLineB = -vUC;
         vLineB = hUC;
         sphere[k+0] = yPt * hLineB + xPt * vLineB;
         sphere[k+1] = xPt * vLineA + yPt * hLineA;
         sphere[k+2] = zPt;
         k += 3;
         
         xPt = cos((i+rotOffset) * (M_PI / (hRings+1)));
         yPt = 0.0;
         zPt = sin((i+rotOffset) * (M_PI / (hRings+1)));
         
         hLineA = hUC;
         vLineA = vUC;
         hLineB = -vUC;
         vLineB = hUC;
         sphere[k+0] = yPt * hLineB + xPt * vLineB;
         sphere[k+1] = xPt * vLineA + yPt * hLineA;
         sphere[k+2] = zPt;
         k += 3;
      }
   }
}

int getTorusSize(int hRings, int vRings)
{
   return hRings * vRings * 6 + vRings * hRings * 6;
}

void genTorus(float *torus, int hRings, int vRings, float tubeDiam)
{
   int i, j, k;
   float height, size;
   float xPt, yPt, zPt;
   float hUC, vUC;
   float hLineA, vLineA;
   float hLineB, vLineB;
   float holeSz;
   
   holeSz = 2.0 - tubeDiam;
   k = 0;
   
   // draw horizontal rings
   for (j=0; j < hRings; j++)
   {
      height = sin(j * (M_PI / (hRings/2.0))) * tubeDiam;
      size = cos(j * (M_PI / (hRings/2.0))) * tubeDiam + holeSz;
      
      xPt = cos(0 * (M_PI / (vRings/2.0))) * size;
      yPt = sin(0 * (M_PI / (vRings/2.0))) * size;
      zPt = height;
      
      for (i=1; i <= vRings; i++)
      {
         torus[k+0] = xPt;
         torus[k+1] = yPt;
         torus[k+2] = zPt;
         k += 3;
         
         xPt = cos(i * (M_PI / (vRings/2.0))) * size;
         yPt = sin(i * (M_PI / (vRings/2.0))) * size;
         zPt = height;
         
         torus[k+0] = xPt;
         torus[k+1] = yPt;
         torus[k+2] = zPt;
         k += 3;
      }
   }
   
   // draw vertical rings
   for (j=0; j < vRings; j++)
   {
      hUC = cos(j * (M_PI / (vRings/2.0)));
      vUC = sin(j * (M_PI / (vRings/2.0)));
      
      xPt = cos(0 * (M_PI / (hRings/2.0))) * tubeDiam + holeSz;
      yPt = 0.0;
      zPt = sin(0 * (M_PI / (hRings/2.0))) * tubeDiam;
      
      for (i=1; i <= hRings; i++)
      {
         hLineA = hUC;
         vLineA = vUC;
         hLineB = -vUC;
         vLineB = hUC;
         torus[k+0] = yPt * hLineB + xPt * vLineB;
         torus[k+1] = xPt * vLineA + yPt * hLineA;
         torus[k+2] = zPt;
         k += 3;
         
         xPt = cos((i) * (M_PI / (hRings/2.0))) * tubeDiam + holeSz;
         yPt = 0.0;
         zPt = sin((i) * (M_PI / (hRings/2.0))) * tubeDiam;
         
         hLineA = hUC;
         vLineA = vUC;
         hLineB = -vUC;
         vLineB = hUC;
         torus[k+0] = yPt * hLineB + xPt * vLineB;
         torus[k+1] = xPt * vLineA + yPt * hLineA;
         torus[k+2] = zPt;
         k += 3;
      }
   }
}
