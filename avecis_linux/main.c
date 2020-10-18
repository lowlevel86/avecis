#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "avecisHandler.h"
#include "sendReceive.h"
#include "drawGraphics.h"
#include "statusBar.h"
#include "windowIcon.h"
#include "icon.h"
#include "keyCodes.h"
#include "playSnd.h"
#include "cmdLineArgs.h"

#define TRUE 1
#define FALSE 0

#define KEY_DOWN 0
#define KEY_UP 1
#define L_MOUSE_DOWN 2
#define L_MOUSE_UP 3
#define R_MOUSE_DOWN 4
#define R_MOUSE_UP 5
#define M_MOUSE_DOWN 6
#define M_MOUSE_UP 7
#define MOUSE_MOVE 8
#define MOUSE_WHEEL_UP 9
#define MOUSE_WHEEL_DOWN 10
#define SOUND_DONE_SIGNAL 11
#define DISCONNECT_SIGNAL 0xFF

char flags[10] = {'t', 'x', 'y', 'w', 'i', 'p', 'b', 's', 'u', 'h'};
int argTypes[10] = {string_arg, int_arg, int_arg, int_arg, int_arg,
                    int_arg, int_arg, int_arg, only_flag, only_flag};
int argCnt = 10;

struct argumentList
{
   char *title;
   intptr_t window_x;
   intptr_t window_y;
   intptr_t window_w;
   intptr_t window_h;
   intptr_t port;
   intptr_t sound_buffer_size;
   intptr_t samples_per_second;
   intptr_t allow_popup;
   intptr_t display_help;
};

struct argumentList argL;

char helpText[] ={"\
avecis - is an audio, vector, input server.\n\
 \n\
Usage:\n\
avecis [options]\n\
 \n\
Options:\n\
-t [title], changes the window title; the default is 'Avecis'\n\
-x [x position], changes the x position; the default is 0\n\
-y [y position], changes the y position; the default is 0\n\
-w [width], changes the window width; the default is 640\n\
-i [height], changes the window height; the default is 360\n\
-p [port], changes the server port; the default is 27015\n\
-b [buffer size], changes the audio buffer size; the default is 4096\n\
-s [samples/sec], changes the audio sample rate; the default is 11025\n\
-u, pops the window up each time a connection is made\n\
-h, displays this message\n\
 \n\
Example usage:\n\
avecis -t 'New Title' -w 400 -i 400 -p 27016 -u\n\
"};

Atom wmDeleteMessage;
Atom userMsg;

int wndEventProc(Display *, Window);

int main(int argc, char *argv[])
{
   char *cmdLineStr;
   Display *dpy;
   Window window;
   
   XTextProperty textproperty;
   char *winTitle[1];
   int ar_supp;
   XSizeHints *win_size_hints;
   
   // default window settings
   argL.title = "Avecis";
   argL.window_x = 0;
   argL.window_y = 0;
   argL.window_w = 640;
   argL.window_h = 360;
   argL.port = 27015;
   argL.sound_buffer_size = 4096;
   argL.samples_per_second = 11025;
   argL.allow_popup = TRUE;
   argL.display_help = FALSE;

   // copy command line args to one line
   mergeCmdLineArgs(argv, argc, &cmdLineStr);

   readCmdLineArgs(cmdLineStr, flags, argTypes, argCnt, (void ***)&argL);
   
   if (argL.display_help)
   {
      printf("%s", helpText);
      freeCmdLineArgs();
      return 0;
   }
   
   dpy = XOpenDisplay(NULL);
   
   if (!dpy)
   {
      printf("Can not open display.\n");
      exit(1);
   }
   
   // create window
   window = XCreateWindow(dpy, DefaultRootWindow(dpy),
                          0, 0, argL.window_w, argL.window_h+STATUS_BAR_HEIGHT, 0,
                          CopyFromParent, CopyFromParent, CopyFromParent, 0, 0);
   
   setWindowIcon(dpy, window, &iconData[0], ICON_WIDTH, ICON_HEIGHT);
   
   // add window title
   winTitle[0] = argL.title;
   XStringListToTextProperty(winTitle, 1, &textproperty);
   XSetWMName(dpy, window, &textproperty);
   
   // disallow key up events when keys auto repeat
   XAutoRepeatOn(dpy);
   XkbSetDetectableAutoRepeat(dpy, True, &ar_supp);
   
   // define the window dimensions
   win_size_hints = XAllocSizeHints();
   if (!win_size_hints)
   {
      printf("XAllocSizeHints - out of memory\n");
      exit(1);
   }

   win_size_hints->flags = PMaxSize | PMinSize;
   win_size_hints->min_width = argL.window_w;
   win_size_hints->min_height = argL.window_h+STATUS_BAR_HEIGHT;
   win_size_hints->max_width = argL.window_w;
   win_size_hints->max_height = argL.window_h+STATUS_BAR_HEIGHT;

   XSetWMNormalHints(dpy, window, win_size_hints);
   XFree(win_size_hints);

   // keep the window from closing until ready
   wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
   XSetWMProtocols(dpy, window, &wmDeleteMessage, 1);
   
   XSelectInput(dpy, window, PointerMotionMask|ButtonPressMask|ButtonReleaseMask|ExposureMask|KeyPressMask|KeyReleaseMask);
   XMapWindow(dpy, window);
   XMoveWindow(dpy, window, argL.window_x, argL.window_y);
   
   userMsg = XInternAtom(dpy, "userMsg", 0);
   
   createStatusBar(dpy, window, argL.window_w, argL.window_h+STATUS_BAR_HEIGHT);
   
   iniGraphics(dpy, window, argL.window_w, argL.window_h);
   iniSndBuffs(argL.sound_buffer_size, argL.samples_per_second, "default"); // "plughw:0,0"
   iniAvecisHandler(argL.port, window);
   
   while (TRUE)
   {
      if (wndEventProc(dpy, window))
      break;
   }

   sendInputEvent(DISCONNECT_SIGNAL, 0);
   
   closeGraphics();
   closeSndBuffs();
   endAvecisHandler();
   
   freeCmdLineArgs();
   unsetWindowIcon();
   XFlush(dpy);
   XCloseDisplay(dpy);
   
   return 0;
}


void lastAudioSegmentEventCallback()
{
   sendInputEvent(SOUND_DONE_SIGNAL, 0);
}


int wndEventProc(Display *dpy, Window window)
{
   XEvent ev;
   XClientMessageEvent clientEv;
   KeySym keysym;
   
   XNextEvent(dpy, &ev);
   
   if (ev.type == KeyPress)
   {
      XLookupString(&ev.xkey, NULL, 0, &keysym, 0);
      sendInputEvent(KEY_DOWN, getKeyboardCode(keysym));
   }
   
   if (ev.type == KeyRelease)
   {
      XLookupString(&ev.xkey, NULL, 0, &keysym, 0);
      sendInputEvent(KEY_UP, getKeyboardCode(keysym));
   }

   if (ev.type == ButtonPress)
   {
      if (ev.xbutton.button == 1)
      sendInputEvent(L_MOUSE_DOWN, 0);
      else
      if (ev.xbutton.button == 2)
		sendInputEvent(M_MOUSE_DOWN, 0);
      else
      if (ev.xbutton.button == 3)
		sendInputEvent(R_MOUSE_DOWN, 0);
      else
      if (ev.xbutton.button == 4)
		sendInputEvent(MOUSE_WHEEL_UP, 0);
      else
      if (ev.xbutton.button == 5)
		sendInputEvent(MOUSE_WHEEL_DOWN, 0);
   }
   
   if (ev.type == ButtonRelease)
   {
      if (ev.xbutton.button == 1)
		sendInputEvent(L_MOUSE_UP, 0);
      else
      if (ev.xbutton.button == 2)
		sendInputEvent(M_MOUSE_UP, 0);
      else
      if (ev.xbutton.button == 3)
		sendInputEvent(R_MOUSE_UP, 0);
   }
   
   if (ev.type == MotionNotify)
   {
      if ((ev.xbutton.x >= 0) && (ev.xbutton.y >= 0))
      sendInputEvent(MOUSE_MOVE, ev.xbutton.x | (ev.xbutton.y << 16));
   }
   
   if (ev.type == ClientMessage)
   {
      #define SHOW_CONTENT 13
      #define PRINT_STATUS 14
      #define END_TRANSMISSION 0xFF
      
      if (ev.xclient.message_type == userMsg)
      {
         if (ev.xclient.data.l[0] == CONNECTION_ESTABLISHED)
         {
            if (argL.allow_popup)
            {
               memset(&clientEv, 0, sizeof(clientEv));
               clientEv.type = ClientMessage;
               clientEv.window = window;
               clientEv.message_type = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", True);
               clientEv.format = 32;
               clientEv.data.l[0] = 1;
               clientEv.data.l[1] = CurrentTime;
               clientEv.data.l[2] = 0;
               clientEv.data.l[3] = 0;
               clientEv.data.l[4] = 0;
               XSendEvent(dpy, RootWindow(dpy, XDefaultScreen(dpy)), False,
                          SubstructureRedirectMask|SubstructureNotifyMask, (XEvent*)&clientEv);
               XFlush(dpy);
            }
         }
         
         if (ev.xclient.data.l[0] == SHOW_CONTENT)
         showContent();
         
         if (ev.xclient.data.l[0] == PRINT_STATUS)
         textToStatusBar(textData);
         
         if (ev.xclient.data.l[0] == END_TRANSMISSION)
         {
            sendInputEvent(DISCONNECT_SIGNAL, 0);
            endAvecisHandler();
            iniAvecisHandler(argL.port, window);
         }
      }
   }
   
   if (ev.type == Expose)
   {
      refreshGraphics();
      refreshStatusBar();
   }
   
   // close window button
   if (ev.type == ClientMessage)
   if (ev.xclient.data.l[0] == wmDeleteMessage)
   return TRUE;
   
   return FALSE;
}
