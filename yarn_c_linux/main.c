#include <math.h>
#include <sys/stat.h>
#include "avecis.h"

#define HOSTNAME "127.0.0.1"
#define PORT 27015
#define win_width 640
#define win_height 360

//compile with:
//gcc main.c avecis.c -lm -pthread -o yarn

char bgImg[win_width*win_height*3];
float yarn_static[256*3];
float yarn[256*3];
int yarnSz = 256*3;

int loadImg(char *imgFile, char *img, int width, int height)
{
   int i;
   struct stat stat_p;
   FILE *bmpFile;
   int headerSize;
   
   // does the file exist
   if (-1 == stat(imgFile, &stat_p))
   return -1;
   
   // open file for read
   bmpFile = fopen(imgFile,"rb");
   if (!bmpFile)
   return -1;
   
   // read image file to buffer
   headerSize = stat_p.st_size - width * height * 3;
   fseek(bmpFile, headerSize, SEEK_CUR);
   
   for (i=0; i < width*height*3; i+=3)
   {
      img[i+2] = fgetc(bmpFile);//blue
      img[i+1] = fgetc(bmpFile);//green
      img[i+0] = fgetc(bmpFile);//red
   }
   
   fclose(bmpFile);
   return 0;
}

void rotVert(float *horiP, float *vertP, float degrees)
{
   float h, v;
   float hUc, vUc;
   
   hUc = cos(degrees * (M_PI * 2.0 / 360.0));
   vUc = sin(degrees * (M_PI * 2.0 / 360.0));
   
   h = *vertP * -vUc + *horiP * hUc;
   v = *horiP * vUc + *vertP * hUc;
   *horiP = h;
   *vertP = v;
}

void rotate(float *verts, int cnt, float x, float y, float z)
{
   int i;
   
   for (i=0; i < cnt; i+=3)
   {
      rotVert(&verts[i+1], &verts[i+2], x);
      rotVert(&verts[i+0], &verts[i+2], y);
      rotVert(&verts[i+0], &verts[i+1], z);
   }
}

void copy(float *vertsA, float *vertsB, int cnt)
{
   int i;
   
   for (i=0; i < cnt; i++)
   {
      vertsB[i] = vertsA[i];
   }
}

void eventCallback(int evType, int keyCode, int xM, int yM)
{
   if (evType == MOUSE_MOVE)
   {
      setColor(bgImg, sizeof(bgImg));
      clearScreen();
      
      setColor("\xFF\x0\x0", 3);
      copy(yarn_static, yarn, yarnSz);
      rotate(yarn, yarnSz, yM, -xM, 0);
      drawPath(yarn, yarnSz);
      
      showContent();
   }
   
   if (keyCode == KC_ESC)
   unblockAvecisDisconnect();
}

int main(int argc, char **argv) 
{
   int i;
   float percent;
   
   if (avecisConnect(HOSTNAME, PORT))
   return 1;
   
   if (loadImg("cat.bmp", bgImg, win_width, win_height))
   return 1;
   
   antialiasingMode(ON);
   orthographicMode(OFF);
   setPerspective(210);
   viewStart(200);
   viewEnd(-200);
   fogStart(100);
   fogEnd(-75);
   fogMode(ON);
   
   setColor("\x40\x40\x40", 3);
   fogColor();
   
   setColor(bgImg, sizeof(bgImg));
   clearScreen();
   
   // create yarn
   for (i=0; i < yarnSz; i+=3)
   {
      percent = (float)i / (yarnSz-1);
      yarn_static[i+0] = sin(percent * (M_PI * 2.0 / (1.0/9))) * 100 * sin(percent * (M_PI * 2.0 / 2.0));
      yarn_static[i+1] = cos(percent * (M_PI * 2.0 / (1.0/9))) * 100 * sin(percent * (M_PI * 2.0 / 2.0));
      yarn_static[i+2] = cos(percent * (M_PI * 2.0 / 2.0)) * 100;
   }
   
   setColor("\xFF\x0\x0", 3);
   copy(yarn_static, yarn, yarnSz);
   drawPath(yarn, yarnSz);
   
   showContent();
   
   printSB("Hello Cat!");

   blockAvecisDisconnect();
   return 0;
}
