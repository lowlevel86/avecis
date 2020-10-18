#include "avecis.h"

#define HOSTNAME "127.0.0.1"
#define PORT 27015

//compile with:
//gcc main.c avecis.c -lm -pthread -o hello

void eventCallback(int evType, int keyCode, int xM, int yM)
{

}

int main(int argc, char **argv) 
{
   int i;
   float beep[5000];
   
   char black[3] = {0x0, 0x0, 0x0};
   char red[3] = {0xFF, 0x0, 0x0};
   char green[3] = {0x0, 0xFF, 0x0};
   char blue[3] = {0x0, 0x0, 0xFF};
   
   float leftEye[6] = {-20.0, 30.0, 0.0, -20.0, -30.0, 0.0};
   float rightEye[6] = {20.0, 30.0, 0.0, 20.0, -30.0, 0.0};
   float mouth[12] = {-75.0, -60.0, 0.0, -30.0, -95.0, 0.0, 20.0, -90.0, 0.0, 70.0, -50.0, 0.0};
   
   if (avecisConnect(HOSTNAME, PORT))
   return 1;
   
   // draw a smiley face
   setColor(black, 3);
   clearScreen();
   
   setColor(red, 3);
   drawLine(&leftEye[0], 6);
   
   setColor(green, 3);
   drawLine(&rightEye[0], 6);
   
   setColor(blue, 3);
   drawPath(&mouth[0], 12);
   
   showContent();
   
   // create and play square wave
   for (i=0; i < 5000; i++)
   {
      if (i%50 >= 25)
      beep[i] = 1.0;
      else
      beep[i] = 0.0;
   }
   
   playSound(beep, beep, 5000);
   
   // print to status bar
   printSB("Hello World!");

   avecisDisconnect();
   return 0;
}
