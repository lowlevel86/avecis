#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "drawGraphics.h"
#include "line.h"

#define TRUE 1
#define FALSE 0

static Display *dpy;
static Window win;
static GC dgc;
static XImage *ximage;

static int winByteWidth;
static char *pBits;
static int **scrnBuff;
static float **zIndex;
static RENDRINFO rI;

static int black = 0x000000;
static int *colorData = &black;
static int colorData_allocated = FALSE;
static int colorCnt = 1;
static int colorInc = 0;

static int fog = FALSE;
static int fColor = 0x888888;
static int antialiasing = FALSE;


void iniGraphics(Display *display, Window window, int width, int height)
{
   int i, x, y;
   Visual *vis;
   int scrn;
   
   dpy = display;
   win = window;
   scrn = DefaultScreen(dpy);
   vis = DefaultVisual(dpy, scrn);
   dgc = DefaultGC(dpy, scrn);
   
   rI.xWin = width;
   rI.yWin = height;
   rI.xCenter = (float)rI.xWin / 2;
   rI.yCenter = (float)rI.yWin / 2;
   
   winByteWidth = rI.xWin * 4;
   
   
   pBits = malloc(winByteWidth * rI.yWin);
   
   for (i=0; i < winByteWidth * rI.yWin; i++)
   pBits[i] = 0x0;
   
   scrnBuff = (int **)malloc(rI.xWin * sizeof(int *));
   for (i = 0; i < rI.xWin; i++)
   scrnBuff[i] = (int *)malloc(rI.yWin * sizeof(int));
   
   for (y = 0; y < rI.yWin; y++)
   for (x = 0; x < rI.xWin; x++)
   scrnBuff[x][y] = 0x888888;
   
   for (y = 0; y < rI.yWin; y++)
   for (x = 0; x < rI.xWin; x++)
   {
      pBits[0+x*4+y*winByteWidth] = (char)((scrnBuff[x][y] & 0xFF0000) >> 16);
      pBits[1+x*4+y*winByteWidth] = (char)((scrnBuff[x][y] & 0x00FF00) >> 8);
      pBits[2+x*4+y*winByteWidth] = (char)scrnBuff[x][y] & 0x0000FF;
   }
   ximage = XCreateImage(dpy, vis, 24, ZPixmap, 0, pBits, rI.xWin, rI.yWin, 32, 0);
   
   zIndex = (float **)malloc(rI.xWin * sizeof(float *));
   for (i = 0; i < rI.xWin; i++)
   zIndex[i] = (float *)malloc(rI.yWin * sizeof(float));
   
   for (y = 0; y < rI.yWin; y++)
   for (x = 0; x < rI.xWin; x++)
   zIndex[x][y] = -FLT_MAX;
   
   rI.camLenZ = 300;
   rI.camEndZ = -300;
   rI.fogBgnZ = 100;
   rI.fogEndZ = -300;
   rI.perspctv = 400;
   rI.ortho = FALSE;
   rI.zIndex = zIndex;
   rI.scrnBuff = scrnBuff;
}

void viewStart(float value)
{
   rI.camLenZ = value;
}

void viewEnd(float value)
{
   rI.camEndZ = value;
}

void setPerspective(float value)
{
   rI.perspctv = value;
}

void orthographicMode(int value)
{
   rI.ortho = value;
}

void fogMode(int value)
{
   fog = value;
}

void fogColor(int value)
{
   fColor = value;
}

void fogStart(float value)
{
   rI.fogBgnZ = value;
}

void fogEnd(float value)
{
   rI.fogEndZ = value;
}

void antialiasingMode(int value)
{
   antialiasing = value;
}

void setColor(char *colorBytes, int size)
{
   int i;
   int cInc = 0;
   
   if (colorData_allocated)
   free(colorData);
   
   colorData = (int *)malloc(size * sizeof(int));
   colorData_allocated = TRUE;
   
   for (i=0; i < size; i+=3)
   {
      colorData[cInc] = ((int)colorBytes[i] & 0xFF) |
                       (((int)colorBytes[i+1]<<8) & 0xFF00) |
                       (((int)colorBytes[i+2]<<16) & 0xFF0000);
      cInc++;
   }
   
   colorCnt = cInc;
   colorInc = 0;
}

void clearScreen()
{
   int x, y;
   
   for (y = 0; y < rI.yWin; y++)
   for (x = 0; x < rI.xWin; x++)
   {
      scrnBuff[x][y] = colorData[colorInc];
      
      if (colorInc < colorCnt-1)
      colorInc++;
   }
   
   for (y = 0; y < rI.yWin; y++)
   for (x = 0; x < rI.xWin; x++)
   zIndex[x][y] = -FLT_MAX;
}

void drawLine(float *lineData, int floatCnt)
{
   int i;
   
   if ((antialiasing == FALSE) && (fog == FALSE))
   for (i=0; i < floatCnt; i+=6)
   {
      line(lineData[i+0], lineData[i+1], lineData[i+2],
           lineData[i+3], lineData[i+4], lineData[i+5], colorData[colorInc], rI);
      
      if (colorInc < colorCnt-1)
      colorInc++;
   }
   
   if ((antialiasing == TRUE) && (fog == FALSE))
   for (i=0; i < floatCnt; i+=6)
   {
      aLine(lineData[i+0], lineData[i+1], lineData[i+2],
            lineData[i+3], lineData[i+4], lineData[i+5], colorData[colorInc], rI);
      
      if (colorInc < colorCnt-1)
      colorInc++;
   }
   
   if ((antialiasing == FALSE) && (fog == TRUE))
   for (i=0; i < floatCnt; i+=6)
   {
      fLine(lineData[i+0], lineData[i+1], lineData[i+2],
            lineData[i+3], lineData[i+4], lineData[i+5], colorData[colorInc], fColor, rI);
      
      if (colorInc < colorCnt-1)
      colorInc++;
   }
   
   if ((antialiasing == TRUE) && (fog == TRUE))
   for (i=0; i < floatCnt; i+=6)
   {
      afLine(lineData[i+0], lineData[i+1], lineData[i+2],
             lineData[i+3], lineData[i+4], lineData[i+5], colorData[colorInc], fColor, rI);
      
      if (colorInc < colorCnt-1)
      colorInc++;
   }
}

void drawPath(float *pointData, int floatCnt)
{
   int i;
   
   if ((antialiasing == FALSE) && (fog == FALSE))
   for (i=0; i < floatCnt-3; i+=3)
   {
      line(pointData[i+0], pointData[i+1], pointData[i+2],
           pointData[i+3], pointData[i+4], pointData[i+5], colorData[colorInc], rI);
      
      if (colorInc < colorCnt-1)
      colorInc++;
   }
   
   if ((antialiasing == TRUE) && (fog == FALSE))
   for (i=0; i < floatCnt-3; i+=3)
   {
      aLine(pointData[i+0], pointData[i+1], pointData[i+2],
            pointData[i+3], pointData[i+4], pointData[i+5], colorData[colorInc], rI);
      
      if (colorInc < colorCnt-1)
      colorInc++;
   }
   
   if ((antialiasing == FALSE) && (fog == TRUE))
   for (i=0; i < floatCnt-3; i+=3)
   {
      fLine(pointData[i+0], pointData[i+1], pointData[i+2],
            pointData[i+3], pointData[i+4], pointData[i+5], colorData[colorInc], fColor, rI);
      
      if (colorInc < colorCnt-1)
      colorInc++;
   }
   
   if ((antialiasing == TRUE) && (fog == TRUE))
   for (i=0; i < floatCnt-3; i+=3)
   {
      afLine(pointData[i+0], pointData[i+1], pointData[i+2],
             pointData[i+3], pointData[i+4], pointData[i+5], colorData[colorInc], fColor, rI);
      
      if (colorInc < colorCnt-1)
      colorInc++;
   }
}

void showContent()
{
   int x, y, ny;
   
   ny = rI.yWin;
   
   for (y = 0; y < rI.yWin; y++)
   {
      ny--;
      for (x = 0; x < rI.xWin; x++)
      {
         pBits[0+x*4+y*winByteWidth] = (char)((scrnBuff[x][ny] & 0xFF0000) >> 16);
         pBits[1+x*4+y*winByteWidth] = (char)((scrnBuff[x][ny] & 0x00FF00) >> 8);
         pBits[2+x*4+y*winByteWidth] = (char)scrnBuff[x][ny] & 0x0000FF;
      }
   }
   
   XPutImage(dpy, win, dgc, ximage, 0, 0, 0, 0, rI.xWin, rI.yWin);
}


void refreshGraphics()
{
   XPutImage(dpy, win, dgc, ximage, 0, 0, 0, 0, rI.xWin, rI.yWin);
}

void closeGraphics()
{
   int i;
   
   ximage->data = 0;//to keep the manually created data from getting freed
   XDestroyImage(ximage);
   
   for (i = 0; i < rI.xWin; i++)
   free(zIndex[i]);
   free(zIndex);
   
   for (i = 0; i < rI.xWin; i++)
   free(scrnBuff[i]);
   free(scrnBuff);
   
   if (colorData_allocated)
   free(colorData);
   
   free(pBits);
}
