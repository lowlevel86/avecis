#include "statusBar.h"

static int winWidth;
static int winHeight;

static Display *dpy;
static Window win;
static GC background_gc;
static GC border_gc;
static GC text_gc;

static char textData[4096] = {0};
static int textSize = 0;

void createStatusBar(Display *display, Window window, int windowWidth, int windowHeight)
{
   dpy = display;
   win = window;
   winWidth = windowWidth;
   winHeight = windowHeight;
   
   XColor background_color;
   Colormap background_colormap;
   XColor border_color;
   Colormap border_colormap;
   
   Font font;
   
   // create background
   background_colormap = DefaultColormap(dpy, 0);
   background_gc = XCreateGC(dpy, win, 0, NULL);
   background_color.red = 54484;
   background_color.green = 53456;
   background_color.blue = 51400;
   background_color.flags = DoRed | DoGreen | DoBlue;
   XAllocColor(dpy, background_colormap, &background_color);
   XSetForeground(dpy, background_gc, background_color.pixel);
   
   XFillRectangle(dpy, win, background_gc, 0, winHeight-STATUS_BAR_HEIGHT, winWidth, STATUS_BAR_HEIGHT);
   
   // create border
   border_colormap = DefaultColormap(dpy, 0);
   border_gc = XCreateGC(dpy, win, 0, NULL);
   border_color.red = 32896;
   border_color.green = 32896;
   border_color.blue = 32896;
   border_color.flags = DoRed | DoGreen | DoBlue;
   XAllocColor(dpy, border_colormap, &border_color);
   XSetForeground(dpy, border_gc, border_color.pixel);
   
   XDrawLine(dpy, win, border_gc, 0, winHeight-STATUS_BAR_HEIGHT+2, winWidth-2, winHeight-STATUS_BAR_HEIGHT+2);
   
   // create font format
   font = XLoadFont(dpy, "*x13");
   text_gc = XCreateGC(dpy, win, 0, NULL);
   XSetFont(dpy, text_gc, font);
}

void textToStatusBar(char *text)
{
   int i;
   
   textSize = 4096;
   
   for (i=0; i < 4096; i++)
   {
      if (!text[i])
      {
         textSize = i+1;
         break;
      }
      
      textData[i] = text[i];
   }
   
   XFillRectangle(dpy, win, background_gc, 0, winHeight-STATUS_BAR_HEIGHT, winWidth, STATUS_BAR_HEIGHT);
   XDrawLine(dpy, win, border_gc, 0, winHeight-STATUS_BAR_HEIGHT+2, winWidth-2, winHeight-STATUS_BAR_HEIGHT+2);
   XDrawString(dpy, win, text_gc, 2, winHeight-4, &textData[0], textSize-1);
}

void refreshStatusBar()
{
   XFillRectangle(dpy, win, background_gc, 0, winHeight-STATUS_BAR_HEIGHT, winWidth, STATUS_BAR_HEIGHT);
   XDrawLine(dpy, win, border_gc, 0, winHeight-STATUS_BAR_HEIGHT+2, winWidth-2, winHeight-STATUS_BAR_HEIGHT+2);
   XDrawString(dpy, win, text_gc, 2, winHeight-4, &textData[0], textSize-1);
}
