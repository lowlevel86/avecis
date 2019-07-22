#include <sys/time.h>
#include "avecis.h"
#include "ucrot.h"
#include "controls.h"
#include "vector_ascii.h"
#include "shapes.h"

#define HOSTNAME "127.0.0.1"
#define PORT 27015
#define win_width 640
#define win_height 360
#define ball_radius 120


struct rendVars rv;

struct buttonCtrl aa_btn;
struct buttonCtrl ortho_btn;
struct buttonCtrl fog_btn;
struct sliderCtrl fog_start_sl;
struct sliderCtrl fog_end_sl;
struct sliderCtrl perspective_sl;
struct sliderCtrl view_start_sl;
struct sliderCtrl view_end_sl;
struct labelCtrl fog_start_label;
struct labelCtrl fog_end_label;
struct labelCtrl view_start_label;
struct labelCtrl view_end_label;
struct labelCtrl fog_label;
struct labelCtrl perspective_label;
struct labelCtrl view_label;

float *torus_static;
float *torus;
int torusSz;
float torus_UC[6];

float *ball_static;
float *ball;
int ballSz;
float ball_UC[6];


void rot(float *verts, int cnt, float *UC)
{
   int i;
   
   for (i=0; i < cnt; i+=3)
   {
      ucRotVertex(&UC[0], &verts[i+0], &verts[i+1], &verts[i+2]);
   }
}

void scale(float *verts, int cnt, float x, float y, float z)
{
   int i;
   
   for (i=0; i < cnt; i+=3)
   {
      verts[i+0] *= x;
      verts[i+1] *= y;
      verts[i+2] *= z;
   }
}

void move(float *verts, int cnt, float x, float y, float z)
{
   int i;
   
   for (i=0; i < cnt; i+=3)
   {
      verts[i+0] += x;
      verts[i+1] += y;
      verts[i+2] += z;
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


void drawScreen()
{
   struct timeval tval;
   static unsigned long long msec = 0, msec_prior = 0;
   
   char black[3] = {0x0, 0x0, 0x0};
   char white[3] = {0xFF, 0xFF, 0xFF};
   char red[3] = {0x99, 0x77, 0x77};
   char blue[3] = {0x77, 0x77, 0x99};
   
   // draw screen at time intervals
   gettimeofday(&tval, NULL);
   msec = tval.tv_sec*1000LL + tval.tv_usec/1000;
   
   if (msec - msec_prior >= 50)
   {
      copy(&ball_static[0], &ball[0], ballSz);
      copy(&torus_static[0], &torus[0], torusSz);
      
      rot(&ball[0], ballSz, &ball_UC[0]);
      rot(&torus[0], torusSz, &torus_UC[0]);
      
      setColor(white, 3);
      clearScreen();
      
      setColor(red, 3);
      drawLine(&ball[0], ballSz);
      
      setColor(blue, 3);
      drawLine(&torus[0], torusSz);
      
      setColor(black, 3);
      
      drawButton(&aa_btn, rv);
      drawButton(&ortho_btn, rv);
      drawButton(&fog_btn, rv);
      
      drawSlider(&fog_start_sl, rv);
      drawSlider(&fog_end_sl, rv);
      drawSlider(&perspective_sl, rv);
      drawSlider(&view_start_sl, rv);
      drawSlider(&view_end_sl, rv);
      
      drawLabel(&fog_start_label, rv);
      drawLabel(&fog_end_label, rv);
      drawLabel(&view_start_label, rv);
      drawLabel(&view_end_label, rv);
      drawLabel(&fog_label, rv);
      drawLabel(&perspective_label, rv);
      drawLabel(&view_label, rv);
      
      showContent();
      msec_prior = msec;
   }
}


void eventCallback(int evType, int keyCode, int xM, int yM)
{
   static int mMouseDwn = FALSE;
   static int rMouseDwn = FALSE;
   static float xMouse = 0, yMouse = 0;
   static float xMousePrior = 0, yMousePrior = 0;
   static int updateGraphics = FALSE;
   
   // store mouse events
   if (evType == MOUSE_MOVE)
   {
      xMouse = xM - win_width/2.0;
      yMouse = -yM + win_height/2.0;
   }
   
   if (evType == L_MOUSE_DOWN)
   updateGraphics = TRUE;
   
   if (evType == L_MOUSE_UP)
   updateGraphics = TRUE;

   if (evType == M_MOUSE_DOWN)
   {
      mMouseDwn = TRUE;
      updateGraphics = TRUE;
   }
   
   if (evType == M_MOUSE_UP)
   {
      mMouseDwn = FALSE;
      updateGraphics = TRUE;
   }

   if (evType == R_MOUSE_DOWN)
   {
      rMouseDwn = TRUE;
      updateGraphics = TRUE;
   }
   
   if (evType == R_MOUSE_UP)
   {
      rMouseDwn = FALSE;
      updateGraphics = TRUE;
   }

   // press 'R' to refresh graphics
   if (keyCode == KC_R)
   updateGraphics = TRUE;

   
   if (clickButton(&aa_btn, evType, xMouse, yMouse))
   {
      rv.antialiasing = !rv.antialiasing;
      antialiasingMode(rv.antialiasing);
   }
   
   if (clickButton(&ortho_btn, evType, xMouse, yMouse))
   {
      rv.orthographic = !rv.orthographic;
      orthographicMode(rv.orthographic);
   }
   
   if (clickButton(&fog_btn, evType, xMouse, yMouse))
   {
      rv.fog = !rv.fog;
      fogMode(rv.fog);
   }
   
   if (moveSlider(&fog_start_sl, evType, xMouse, yMouse))
   {
      rv.fogStart = getSliderValue(&fog_start_sl);
      fogStart(rv.fogStart);
      updateGraphics = TRUE;
   }
   
   if (moveSlider(&fog_end_sl, evType, xMouse, yMouse))
   {
      rv.fogEnd = getSliderValue(&fog_end_sl);
      fogEnd(rv.fogEnd);
      updateGraphics = TRUE;
   }
   
   if (moveSlider(&perspective_sl, evType, xMouse, yMouse))
   {
      rv.perspective = getSliderValue(&perspective_sl);
      setPerspective(rv.perspective);
      updateGraphics = TRUE;
   }
   
   if (moveSlider(&view_start_sl, evType, xMouse, yMouse))
   {
      rv.viewStart = getSliderValue(&view_start_sl);
      viewStart(rv.viewStart);
      updateGraphics = TRUE;
   }
   
   if (moveSlider(&view_end_sl, evType, xMouse, yMouse))
   {
      rv.viewEnd = getSliderValue(&view_end_sl);
      viewEnd(rv.viewEnd);
      updateGraphics = TRUE;
   }
   
   
   if ((rMouseDwn) || (mMouseDwn))
   {
      trackBallRotUc(ball_radius, xMousePrior, yMousePrior, xMouse, yMouse, &ball_UC[0]);
      
      // copy unit circle coordinates of the z axis
      torus_UC[4] = ball_UC[4];
      torus_UC[5] = ball_UC[5];
      
      drawScreen();
   }
   
   
   if (updateGraphics)
   {
      drawScreen();
      updateGraphics = FALSE;
   }
   
   
   if (onButton(&aa_btn, xMouse, yMouse))
   {
      if (rv.antialiasing)
         printSB("Antialiasing is on.");
      else
         printSB("Antialiasing is off.");
   }
   
   if (onButton(&ortho_btn, xMouse, yMouse))
   {
      if (rv.orthographic)
         printSB("Orthographic is on.");
      else
         printSB("Orthographic is off.");
   }
   
   if (onButton(&fog_btn, xMouse, yMouse))
   {
      if (rv.fog)
         printSB("Fog is on.");
      else
         printSB("Fog is off.");
   }
   
   
   if (onSlider(&fog_start_sl, xMouse, yMouse))
   printSB("Fog starts at %.2f.", rv.fogStart);
   
   if (onSlider(&fog_end_sl, xMouse, yMouse))
   printSB("Fog ends at %.2f.", rv.fogEnd);
   
   if (onSlider(&perspective_sl, xMouse, yMouse))
   printSB("Perspective: %.2f (must be greater than view start)", rv.perspective);
   
   if (onSlider(&view_start_sl, xMouse, yMouse))
   printSB("View starts at %.2f.", rv.viewStart);
   
   if (onSlider(&view_end_sl, xMouse, yMouse))
   printSB("View ends at %.2f.", rv.viewEnd);
   
   
   if (evType == MOUSE_MOVE)
   {
      xMousePrior = xMouse;
      yMousePrior = yMouse;
   }
   
   if (keyCode == KC_ESC)
   unblockAvecisDisconnect();
}


int main(int argc, char **argv) 
{
   rv.viewStart = 250;
   rv.viewEnd = -50;
   rv.perspective = 300;
   rv.orthographic = TRUE;
   rv.fog = TRUE;
   rv.fogStart = 100;
   rv.fogEnd = -50;
   rv.antialiasing = TRUE;
   
   // create gui and geometry
   createButton(&aa_btn, "Antialiasing", 25, -win_width/2.0+8, win_height/2.0-35);
   createButton(&ortho_btn, "Orthographic Mode", 25, -win_width/2.0+8, win_height/2.0-70);
   createButton(&fog_btn, "Fog", 25, -win_width/2.0+8, win_height/2.0-105);
   
   createSlider(&fog_start_sl, -200, -80, 25, 160, -500.0, 500.0, rv.fogStart);
   createSlider(&fog_end_sl, -120, -80, 25, 160, -500.0, 500.0, rv.fogEnd);
   createSlider(&perspective_sl, -12.5, -80, 25, 160, -500.0, 500.0, rv.perspective);
   createSlider(&view_start_sl, 95, -80, 25, 160, -500.0, 500.0, rv.viewStart);
   createSlider(&view_end_sl, 175, -80, 25, 160, -500.0, 500.0, rv.viewEnd);
   
   createLabel(&fog_start_label, "start", 20, -218, -100, FALSE);
   createLabel(&fog_end_label, "end", 20, -128, -100, FALSE);
   createLabel(&view_start_label, "start", 20, 77, -100, FALSE);
   createLabel(&view_end_label, "end", 20, 167, -100, FALSE);
   
   createLabel(&fog_label, "Fog", 20, -168, -120, TRUE);
   createLabel(&perspective_label, "Perspective", 20, -60, -110, TRUE);
   createLabel(&view_label, "View", 20, 122, -120, TRUE);
   
   // create sphere
   ballSz = getSphereSize(8, 8);
   
   ball_static = (float *)malloc(ballSz * sizeof(float));
   ball = (float *)malloc(ballSz * sizeof(float));
   
   genSphere(&ball_static[0], 8, 8);
   
   scale(&ball_static[0], ballSz, ball_radius, ball_radius, ball_radius);
   
   getUc(0, 0, 0, &ball_UC[0]);
   
   copy(&ball_static[0], &ball[0], ballSz);
   
   // create torus
   torusSz = getTorusSize(16, 16);
   
   torus_static = (float *)malloc(torusSz * sizeof(float));
   torus = (float *)malloc(torusSz * sizeof(float));
   
   genTorus(&torus_static[0], 16, 16, 0.75);
   
   scale(&torus_static[0], torusSz, ball_radius*2, ball_radius*2, ball_radius*2);
   
   getUc(0, 0, 0, &torus_UC[0]);
   
   copy(&torus_static[0], &torus[0], torusSz);
   
   
   if (avecisConnect(HOSTNAME, PORT))
   {
      free(ball);
      free(ball_static);
      free(torus);
      free(torus_static);
      
      return 1;
   }
   
   // initialize graphics variables
   viewStart(rv.viewStart);
   viewEnd(rv.viewEnd);
   setPerspective(rv.perspective);
   orthographicMode(rv.orthographic);
   fogMode(rv.fog);
   fogStart(rv.fogStart);
   fogEnd(rv.fogEnd);
   antialiasingMode(rv.antialiasing);
   
   setColor("\xFF\xFF\xFF", 3);
   fogColor();
   
   drawScreen();
   
   blockAvecisDisconnect();
   
   free(ball);
   free(ball_static);
   free(torus);
   free(torus_static);
   
   return 0;
}
