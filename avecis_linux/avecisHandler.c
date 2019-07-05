#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <memory.h>
#include "sendReceive.h"
#include "drawGraphics.h"
#include "playSnd.h"

#define TRUE 1
#define FALSE 0

#define UNKNOWN -1
#define SET_VIEW_START 0
#define SET_VIEW_END 1
#define SET_PERSPECTIVE 2
#define SET_ORTHOGRAPHIC_MODE 3
#define SET_FOG_MODE 4
#define SET_FOG_COLOR 5
#define SET_FOG_START 6
#define SET_FOG_END 7
#define SET_ANTIALIASING_MODE 8
#define SET_COLOR 9
#define CLEAR_SCREEN 10
#define DRAW_LINE 11
#define DRAW_PATH 12
#define SHOW_CONTENT 13
#define PRINT_STATUS 14
#define PLAY_SOUND 15
#define STOP_SOUND 16
#define END_TRANSMISSION 0xFF

#define CONNECTION_ESTABLISHED 1

Window window_glob;
Display *dpy;
XEvent msgEv;
Atom userMsg;
char textData[4096] = {0};


void *XPostEvent(void *data)
{
   Display *display;
   XEvent mEv;
   int *msgData;
   
   display = XOpenDisplay(NULL);
   
   if (display == NULL)
   return 0;

   msgData = data;
   
   memset(&mEv, 0, sizeof(mEv));
   mEv.type = ClientMessage;
   mEv.xclient.message_type = userMsg;
   mEv.xclient.format = 32;
   mEv.xclient.data.l[0] = msgData[0];
   mEv.xclient.data.l[1] = msgData[1];
   
   XSendEvent(display, window_glob, FALSE, NoEventMask, &mEv);
   XFlush(display);
   XCloseDisplay(display);
   
   return NULL;
}

void postMsgEvent(void *dataA, void *dataB)
{
   static void *data[2];
   pthread_t XPostEvent_thread;
   
   data[0] = dataA;
   data[1] = dataB;
   
   pthread_create(&XPostEvent_thread, NULL, XPostEvent, data);
}

void sendMsgEvent(void *dataA, void *dataB)
{
   if (dpy == NULL)
   {
      dpy = XOpenDisplay(NULL);
      
      if (dpy == NULL)
      return;
   }

   msgEv.xclient.data.l[0] = (int)dataA;
   msgEv.xclient.data.l[1] = (int)dataB;
   
   XSendEvent(dpy, window_glob, FALSE, NoEventMask, &msgEv);
   XFlush(dpy);
}


void iniAvecisHandler(int port, Window window)
{
   Display *display;
   
   display = XOpenDisplay(NULL);
   
   if (display == NULL)
   return;

   window_glob = window;
   dpy = NULL;
   
   userMsg = XInternAtom(display, "userMsg", 0);
   
   memset(&msgEv, 0, sizeof(msgEv));
   msgEv.type = ClientMessage;
   msgEv.xclient.message_type = userMsg;
   msgEv.xclient.format = 32;
   
   iniSendReceiveServer(port);
   XCloseDisplay(display);
}

void endAvecisHandler()
{
   endSendReceiveServer();
   
   if (dpy != NULL)
   XCloseDisplay(dpy);
}

void receiveCallback(char *bytes, int byteCnt)
{
   int i, r, g, b;
   int byteInc = 0;
   static uint32_t opDataSz = 0;
   static int opDataSzBytesAcquired = 0;
   static char *opDataBuff;
   static int opDataBuffInc = 0;
   static int opType = UNKNOWN;
   static int blockAllData = TRUE;
   static char errorText[] = "DATA ERROR";
   
   // init/reset avecis handler
   if (byteCnt == 0)
   {
      // reset variables
      opDataSz = 0;
      opDataSzBytesAcquired = 0;
      opType = UNKNOWN;
      
      if (opDataBuffInc)
      free(opDataBuff);
      
      opDataBuffInc = 0;
      
      sendMsgEvent((void *)CONNECTION_ESTABLISHED, (void *)0);
      blockAllData = FALSE;
      
      return;
   }
   
   if (blockAllData)
   return;
   
   while (TRUE)
   {
      // get the operation data size
      if (opDataSzBytesAcquired < sizeof(uint32_t))
      {
         if (opDataSzBytesAcquired == 0)
         {
            opDataSz = bytes[byteInc] & 0xFF;
            opDataSzBytesAcquired++;
            byteInc++;
         }
         
         if (byteInc >= byteCnt)
         return;
         
         if (opDataSzBytesAcquired == 1)
         {
            opDataSz |= (bytes[byteInc] << 8) & 0xFF00;
            opDataSzBytesAcquired++;
            byteInc++;
         }
         
         if (byteInc >= byteCnt)
         return;
         
         if (opDataSzBytesAcquired == 2)
         {
            opDataSz |= (bytes[byteInc] << 16) & 0xFF0000;
            opDataSzBytesAcquired++;
            byteInc++;
         }
         
         if (byteInc >= byteCnt)
         return;
         
         if (opDataSzBytesAcquired == 3)
         {
            opDataSz |= (bytes[byteInc] << 24) & 0xFF000000;
            opDataSzBytesAcquired++;
            byteInc++;
         }
         
         if (byteInc >= byteCnt)
         return;
      }
      
      
      if (opType == UNKNOWN)
      {
         opType = bytes[byteInc] & 0xFF;
         byteInc++;
         
         opDataBuff = malloc(opDataSz);
      }
      
      
      // print function count, type and size for debugging
      /*static int funcCount = 0;
      if (opDataBuffInc == 0)
      {
         funcCount++;
         printf("%i, type:%i, data size:%i\n", funcCount, opType, opDataSz);
      }*/
      
      // check for errors
      if (opType != 0xFF)
      if ((opType < -1) || (opType > 17))
      {
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
         
         blockAllData = TRUE;
         
         sendMsgEvent((void *)PRINT_STATUS, (void *)&errorText[0]);
         postMsgEvent((void *)END_TRANSMISSION, (void *)0);
         
         return;
      }
      
      
      ////////////////////////////////////////////////////////////
      ///////////// these operations require no data /////////////
      
      if (opType == CLEAR_SCREEN)
      {
         clearScreen();
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (opType == SHOW_CONTENT)
      {
         sendMsgEvent((void *)SHOW_CONTENT, (void *)0);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (opType == STOP_SOUND)
      {
         stopSound();
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (opType == END_TRANSMISSION)
      {
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
         
         blockAllData = TRUE;
         
         postMsgEvent((void *)END_TRANSMISSION, (void *)0);
         
         return;
      }
      
      ////////////////////////////////////////////////////////////
      
      
      if (byteInc >= byteCnt)
      return;
      
      
      if (opType == SET_VIEW_START)
      {
         while (TRUE)
         {
            opDataBuff[opDataBuffInc] = bytes[byteInc];
            opDataBuffInc++;
            byteInc++;
            
            if (opDataBuffInc >= opDataSz)
            break;
            
            if (byteInc >= byteCnt)
            return;
         }
         
         viewStart(*(float *)opDataBuff);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (byteInc >= byteCnt)
      return;
      
      
      if (opType == SET_VIEW_END)
      {
         while (TRUE)
         {
            opDataBuff[opDataBuffInc] = bytes[byteInc];
            opDataBuffInc++;
            byteInc++;
            
            if (opDataBuffInc >= opDataSz)
            break;
            
            if (byteInc >= byteCnt)
            return;
         }
         
         viewEnd(*(float *)opDataBuff);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (byteInc >= byteCnt)
      return;
      
      
      if (opType == SET_PERSPECTIVE)
      {
         while (TRUE)
         {
            opDataBuff[opDataBuffInc] = bytes[byteInc];
            opDataBuffInc++;
            byteInc++;
            
            if (opDataBuffInc >= opDataSz)
            break;
            
            if (byteInc >= byteCnt)
            return;
         }
         
         setPerspective(*(float *)opDataBuff);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (byteInc >= byteCnt)
      return;
      
      
      if (opType == SET_ORTHOGRAPHIC_MODE)
      {
         while (TRUE)
         {
            opDataBuff[opDataBuffInc] = bytes[byteInc];
            opDataBuffInc++;
            byteInc++;
            
            if (opDataBuffInc >= opDataSz)
            break;
            
            if (byteInc >= byteCnt)
            return;
         }
         
         orthographicMode((int)opDataBuff[0] & 0xFF);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (byteInc >= byteCnt)
      return;
      
      
      if (opType == SET_FOG_MODE)
      {
         while (TRUE)
         {
            opDataBuff[opDataBuffInc] = bytes[byteInc];
            opDataBuffInc++;
            byteInc++;
            
            if (opDataBuffInc >= opDataSz)
            break;
            
            if (byteInc >= byteCnt)
            return;
         }
         
         fogMode((int)opDataBuff[0] & 0xFF);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (byteInc >= byteCnt)
      return;
      
      
      if (opType == SET_FOG_COLOR)
      {
         while (TRUE)
         {
            opDataBuff[opDataBuffInc] = bytes[byteInc];
            opDataBuffInc++;
            byteInc++;
            
            if (opDataBuffInc >= opDataSz)
            break;
            
            if (byteInc >= byteCnt)
            return;
         }
         
         r = opDataBuff[0] & 0xFF;
         g = opDataBuff[1] & 0xFF;
         b = opDataBuff[2] & 0xFF;
         fogColor(r | g<<8 | b<<16);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (byteInc >= byteCnt)
      return;
      
      
      if (opType == SET_FOG_START)
      {
         while (TRUE)
         {
            opDataBuff[opDataBuffInc] = bytes[byteInc];
            opDataBuffInc++;
            byteInc++;
            
            if (opDataBuffInc >= opDataSz)
            break;
            
            if (byteInc >= byteCnt)
            return;
         }
         
         fogStart(*(float *)opDataBuff);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (byteInc >= byteCnt)
      return;
      
      
      if (opType == SET_FOG_END)
      {
         while (TRUE)
         {
            opDataBuff[opDataBuffInc] = bytes[byteInc];
            opDataBuffInc++;
            byteInc++;
            
            if (opDataBuffInc >= opDataSz)
            break;
            
            if (byteInc >= byteCnt)
            return;
         }
         
         fogEnd(*(float *)opDataBuff);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (byteInc >= byteCnt)
      return;
      
      
      if (opType == SET_ANTIALIASING_MODE)
      {
         while (TRUE)
         {
            opDataBuff[opDataBuffInc] = bytes[byteInc];
            opDataBuffInc++;
            byteInc++;
            
            if (opDataBuffInc >= opDataSz)
            break;
            
            if (byteInc >= byteCnt)
            return;
         }
         
         antialiasingMode((int)opDataBuff[0] & 0xFF);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (byteInc >= byteCnt)
      return;
      
      
      if (opType == SET_COLOR)
      {
         while (TRUE)
         {
            opDataBuff[opDataBuffInc] = bytes[byteInc];
            opDataBuffInc++;
            byteInc++;
            
            if (opDataBuffInc >= opDataSz)
            break;
            
            if (byteInc >= byteCnt)
            return;
         }
         
         setColor(opDataBuff, opDataSz);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (byteInc >= byteCnt)
      return;
      
      
      if (opType == DRAW_LINE)
      {
         while (TRUE)
         {
            opDataBuff[opDataBuffInc] = bytes[byteInc];
            opDataBuffInc++;
            byteInc++;
            
            if (opDataBuffInc >= opDataSz)
            break;
            
            if (byteInc >= byteCnt)
            return;
         }
         
         drawLine((float *)&opDataBuff[0], opDataSz/4);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (byteInc >= byteCnt)
      return;
      
      
      if (opType == DRAW_PATH)
      {
         while (TRUE)
         {
            opDataBuff[opDataBuffInc] = bytes[byteInc];
            opDataBuffInc++;
            byteInc++;
            
            if (opDataBuffInc >= opDataSz)
            break;
            
            if (byteInc >= byteCnt)
            return;
         }
         
         drawPath((float *)&opDataBuff[0], opDataSz/4);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (byteInc >= byteCnt)
      return;
      
      
      if (opType == PRINT_STATUS)
      {
         while (TRUE)
         {
            opDataBuff[opDataBuffInc] = bytes[byteInc];
            opDataBuffInc++;
            byteInc++;
            
            if (opDataBuffInc >= opDataSz)
            break;
            
            if (byteInc >= byteCnt)
            return;
         }
         
         for (i=0; i < opDataSz; i++)
         {
            if (i >= sizeof(textData)-1)
            break;
            
            textData[i] = opDataBuff[i];
         }
         
         textData[i] = 0;
         
         sendMsgEvent((void *)PRINT_STATUS, (void *)&textData[0]);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (byteInc >= byteCnt)
      return;
      
      
      if (opType == PLAY_SOUND)
      {
         while (TRUE)
         {
            opDataBuff[opDataBuffInc] = bytes[byteInc];
            opDataBuffInc++;
            byteInc++;
            
            if (opDataBuffInc >= opDataSz)
            break;
            
            if (byteInc >= byteCnt)
            return;
         }
         
         playSound((float *)&opDataBuff[0], (float *)&opDataBuff[opDataSz / 2], opDataSz/4 / 2);
         
         // reset variables
         opDataSz = 0;
         opDataSzBytesAcquired = 0;
         opType = UNKNOWN;
         
         if (opDataBuffInc)
         free(opDataBuff);
         
         opDataBuffInc = 0;
      }
      
      if (byteInc >= byteCnt)
      return;
   }
}

void sendInputEvent(int eventType, int eventData)
{
   char inputEventData[5];
   
   // load type and data to buffer
   inputEventData[0] = eventType;
   inputEventData[1] = eventData & 0xFF;
   inputEventData[2] = (eventData >> 8) & 0xFF;
   inputEventData[3] = (eventData >> 16) & 0xFF;
   inputEventData[4] = (eventData >> 24) & 0xFF;
   
   // send buffer
   sendData(&inputEventData[0], sizeof(inputEventData));
}
