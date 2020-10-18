#include "avecis.h"

#define HOSTNAME "127.0.0.1"
#define PORT 27015

//compile with:
//gcc main.c avecis.c -lws2_32 -o draw.exe

#define win_width 640
#define win_height 360

char white[3] = {0xFF, 0xFF, 0xFF};
char gray[3] = {0x99, 0x77, 0x77};

void eventCallback(int evType, int keyCode, int xM, int yM)
{
   static int lMouseDwn = FALSE;
   static float xMouse, yMouse;
   static float ptData[6] = {0};
   
   if (evType == MOUSE_MOVE)
   {
      xMouse = xM - win_width/2.0;
      yMouse = -yM + win_height/2.0;
   }
   
   if (evType == L_MOUSE_DOWN)
   {
      ptData[0] = xMouse;
      ptData[1] = yMouse;
      ptData[3] = xMouse;
      ptData[4] = yMouse;
      
      lMouseDwn = TRUE;
   }
   
   if (evType == L_MOUSE_UP)
   lMouseDwn = FALSE;

   if (lMouseDwn == TRUE)
   {
      ptData[3] = ptData[0];
      ptData[4] = ptData[1];
      ptData[0] = xMouse;
      ptData[1] = yMouse;
      
      setColor(gray, 3);
      drawLine(&ptData[0], 6);
      
      showContent();
   }
   
   if (evType == R_MOUSE_DOWN)
   {
      setColor(white, 3);
      clearScreen();
      showContent();
   }
   
   printSB("evType:%i keyCode:0x%X xM:%i yM:%i -- Press Esc to Disconnect", evType, keyCode, xM, yM);
   
   if (keyCode == KC_ESC)
   unblockAvecisDisconnect();
}

int main(int argc, char **argv) 
{
   if (avecisConnect(HOSTNAME, PORT))
   return 1;
   
   setColor(white, 3);
   clearScreen();
   showContent();
   antialiasingMode(OFF);
   
   blockAvecisDisconnect();
   return 0;
}
