#include "avecis.h"
#include "vector_ascii.h"
#include "controls.h"


static void scale(float *verts, int cnt, float x, float y, float z)
{
   int i;
   
   for (i=0; i < cnt; i+=3)
   {
      verts[i+0] *= x;
      verts[i+1] *= y;
      verts[i+2] *= z;
   }
}

static void move(float *verts, int cnt, float x, float y, float z)
{
   int i;
   
   for (i=0; i < cnt; i+=3)
   {
      verts[i+0] += x;
      verts[i+1] += y;
      verts[i+2] += z;
   }
}

static void copy(float *vertsA, float *vertsB, int cnt)
{
   int i;
   
   for (i=0; i < cnt; i++)
   {
      vertsB[i] = vertsA[i];
   }
}


void createButton(struct buttonCtrl *btn, char *text, float size, float x, float y)
{
   btn->text = text;
   btn->textSz = strlen(text);
   btn->size = size;
   btn->x = x;
   btn->y = y;
   btn->down = FALSE;
}

int clickButton(struct buttonCtrl *btn, int evType, float xMouse, float yMouse)
{
   float width, height;
   float left, right, top, bottom;
   
   width = (btn->textSz + 2.0) * (btn->size / 2.0);
   height = btn->size;
   
   left = btn->x;
   right = btn->x + width;
   top = btn->y + height;
   bottom = btn->y;
   
   if (btn->down == TRUE)
   return FALSE;
   
   if (evType == L_MOUSE_DOWN)
   {
      if ((xMouse >= left) && (xMouse <= right) && (yMouse >= bottom) && (yMouse <= top))
      {
         btn->down = TRUE;
         return TRUE;
      }
   }
   
   return FALSE;
}

int onButton(struct buttonCtrl *btn, float xMouse, float yMouse)
{
   float width, height;
   float left, right, top, bottom;
   
   width = (btn->textSz + 2.0) * (btn->size / 2.0);
   height = btn->size;
   
   left = btn->x;
   right = btn->x + width;
   top = btn->y + height;
   bottom = btn->y;
   
   if ((xMouse >= left) && (xMouse <= right) && (yMouse >= bottom) && (yMouse <= top))
   return TRUE;
   
   return FALSE;
}

void drawButton(struct buttonCtrl *btn, struct rendVars rv)
{
   int i, j, ch;
   float chrLoc, chSz;
   float length;
   float asciiPathBuff[512];
   float box[15] = {0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0};
   float topMost = 10000;
   
   // set renderer variables for drawing the control
   viewStart(topMost+3.0);
   viewEnd(topMost-1.0);
   orthographicMode(ON);
   fogMode(ON);
   fogStart(topMost+1.0);
   fogEnd(topMost-1.0);
   
   // draw button
   length = (btn->textSz + 2.0) * (btn->size / 2.0);

   scale(&box[0], 15, length, btn->size, 0.0);
   move(&box[0], 15, btn->x, btn->y, topMost+2.0);
   
   if (btn->down == FALSE) // if button is not down
   drawPath(&box[0], 15); // draw front frame
   
   move(&box[0], 15, -2.0, -2.0, -2.0);
   drawPath(&box[0], 15); // draw back frame
   
   // draw button text
   chrLoc = 0;
   chSz = btn->size / 2.0;
   for (i=0; i < btn->textSz; i++)
   {
      ch = btn->text[i];
      if (ch == (int)' ')
      {
         chrLoc++;
         continue;
      }
      
      ch = btn->text[i] - 33;
      for (j=0; j < asciiPathCnts[ch]; j++)
      {
         copy(&asciiPaths[ch][j][0], &asciiPathBuff[0], asciiCoorCnts[ch][j]);
         move(&asciiPathBuff[0], asciiCoorCnts[ch][j], chrLoc+1.4, 1.0, 0.0);
         scale(&asciiPathBuff[0], asciiCoorCnts[ch][j], chSz, chSz, chSz);
         
         if (btn->down == FALSE) // if button is not down
         move(&asciiPathBuff[0], asciiCoorCnts[ch][j], btn->x+1.0, btn->y+1.0, topMost+2.0);
         else
         move(&asciiPathBuff[0], asciiCoorCnts[ch][j], btn->x-1.0, btn->y-1.0, topMost);
         
         drawPath(&asciiPathBuff[0], asciiCoorCnts[ch][j]);
      }
      
      chrLoc++;
   }

   // set renderer variables back to their original values
   viewStart(rv.viewStart);
   viewEnd(rv.viewEnd);
   orthographicMode(rv.orthographic);
   fogMode(rv.fog);
   fogStart(rv.fogStart);
   fogEnd(rv.fogEnd);

   btn->down = FALSE; // reset button if down
}


void createSlider(struct sliderCtrl *sldr, float x, float y,
                  float w, float h, float min, float max, float value)
{
   sldr->x = x;
   sldr->y = y;
   sldr->w = w;
   sldr->h = h;
   sldr->min = min;
   sldr->max = max;
   sldr->value = value;
   sldr->drag = FALSE;
}

int moveSlider(struct sliderCtrl *sldr, int evType, float xMouse, float yMouse)
{
   float left, right, top, bottom;
   float percent;
   
   left = sldr->x;
   right = sldr->x + sldr->w;
   top = sldr->y + sldr->h;
   bottom = sldr->y;
   
   if (evType == L_MOUSE_DOWN)
   {
      if ((xMouse >= left) && (xMouse <= right) && (yMouse >= bottom) && (yMouse <= top))
      {
         sldr->drag = TRUE;
      }
   }
   
   if (evType == L_MOUSE_UP)
   sldr->drag = FALSE;
   
   if (sldr->drag)
   {
      if (sldr->h > sldr->w)
      {
         percent = (yMouse - sldr->y) / sldr->h;
         
         if (percent > 1.0)
         percent = 1.0;
         
         if (percent < 0.0)
         percent = 0.0;
         
         sldr->value = (sldr->max - sldr->min) * percent + sldr->min;
      }
      else
      {
         percent = (xMouse - sldr->x) / sldr->w;
         
         if (percent > 1.0)
         percent = 1.0;
         
         if (percent < 0.0)
         percent = 0.0;
         
         sldr->value = (sldr->max - sldr->min) * percent + sldr->min;
      }
      
      return TRUE;
   }
   
   return FALSE;
}

float getSliderValue(struct sliderCtrl *sldr)
{
   return sldr->value;
}

int onSlider(struct sliderCtrl *sldr, float xMouse, float yMouse)
{
   float left, right, top, bottom;
   
   left = sldr->x;
   right = sldr->x + sldr->w;
   top = sldr->y + sldr->h;
   bottom = sldr->y;
   
   if ((xMouse >= left) && (xMouse <= right) && (yMouse >= bottom) && (yMouse <= top))
   return TRUE;
   
   return FALSE;
}

void drawSlider(struct sliderCtrl *sldr, struct rendVars rv)
{
   float x, y, z, w, h;
   float percent, pos;
   float line[6];
   float box[15] = {0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0};
   float topMost = 10000;
   
   // set renderer variables for drawing the control
   viewStart(topMost+1.0);
   viewEnd(topMost-1.0);
   orthographicMode(ON);
   fogMode(OFF);
   
   // draw slider
   x = sldr->x;
   y = sldr->y;
   z = topMost;
   w = sldr->w;
   h = sldr->h;
   percent = (sldr->value - sldr->min) / (sldr->max - sldr->min);
   
   if (percent > 1.0)
   percent = 1.0;
   
   if (percent < 0.0)
   percent = 0.0;
   
   if (h > w) // larger height makes it vertical else horizontal
   {
      pos = (h-w/2.0) * percent;
      scale(&box[0], 15, w, w/2.0, 0.0);
      move(&box[0], 15, x, y+pos, z);
      drawPath(&box[0], 15);
      
      line[0]=x; line[1]=y; line[2]=z;
      line[3]=x+w; line[4]=y; line[5]=z;
      drawLine(&line[0], 6);
      
      line[0]=x; line[1]=y+h; line[2]=z;
      line[3]=x+w; line[4]=y+h; line[5]=z;
      drawLine(&line[0], 6);
      
      line[0]=x+w/2.0; line[1]=y; line[2]=z;
      line[3]=x+w/2.0; line[4]=y+h; line[5]=z;
      drawLine(&line[0], 6);
   }
   else
   {
      pos = (w-h/2.0) * percent;
      scale(&box[0], 15, h/2.0, h, 0.0);
      move(&box[0], 15, x+pos, y, z);
      drawPath(&box[0], 15);
      
      line[0]=x; line[1]=y; line[2]=z;
      line[3]=x; line[4]=y+h; line[5]=z;
      drawLine(&line[0], 6);
      
      line[0]=x+w; line[1]=y; line[2]=z;
      line[3]=x+w; line[4]=y+h; line[5]=z;
      drawLine(&line[0], 6);
      
      line[0]=x; line[1]=y+h/2.0; line[2]=z;
      line[3]=x+w; line[4]=y+h/2.0; line[5]=z;
      drawLine(&line[0], 6);
   }
   
   // set renderer variables back to their original values
   viewStart(rv.viewStart);
   viewEnd(rv.viewEnd);
   orthographicMode(rv.orthographic);
   fogMode(rv.fog);
}


void createLabel(struct labelCtrl *label, char *text, float size, float x, float y, int border)
{
   label->text = text;
   label->textSz = strlen(text);
   label->size = size;
   label->x = x;
   label->y = y;
   label->border = border;
}

int onLabel(struct labelCtrl *label, float xMouse, float yMouse)
{
   float width, height;
   float left, right, top, bottom;
   
   width = (label->textSz + 1.0) * (label->size / 2.0);
   height = label->size;
   
   left = label->x;
   right = label->x + width;
   top = label->y + height;
   bottom = label->y;
   
   if ((xMouse >= left) && (xMouse <= right) && (yMouse >= bottom) && (yMouse <= top))
   return TRUE;
   
   return FALSE;
}

void drawLabel(struct labelCtrl *label, struct rendVars rv)
{
   int i, j;
   float length;
   int ch, chrLoc, chSz;
   float asciiPathBuff[512];
   float box[15] = {0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0};
   float topMost = 10000;
   
   // set renderer variables for drawing the control
   viewStart(topMost+1.0);
   viewEnd(topMost-1.0);
   orthographicMode(ON);
   fogMode(OFF);
   
   // draw button
   if (label->border)
   {
      length = (label->textSz + 1.0) * (label->size / 2.0);
      scale(&box[0], 15, length, label->size, 0.0);
      move(&box[0], 15, label->x, label->y-1.0, topMost);
      drawPath(&box[0], 15); // draw frame
   }
   
   // draw button text
   chrLoc = 0;
   chSz = label->size / 2.0;
   for (i=0; i < label->textSz; i++)
   {
      ch = label->text[i];
      if (ch == (int)' ')
      {
         chrLoc++;
         continue;
      }
      
      ch = label->text[i] - 33;
      for (j=0; j < asciiPathCnts[ch]; j++)
      {
         copy(&asciiPaths[ch][j][0], &asciiPathBuff[0], asciiCoorCnts[ch][j]);
         move(&asciiPathBuff[0], asciiCoorCnts[ch][j], chrLoc+1.0, 1.0, 0.0);
         scale(&asciiPathBuff[0], asciiCoorCnts[ch][j], chSz, chSz, chSz);
         
         move(&asciiPathBuff[0], asciiCoorCnts[ch][j], label->x, label->y, topMost);
         drawPath(&asciiPathBuff[0], asciiCoorCnts[ch][j]);
      }
      
      chrLoc++;
   }

   // set renderer variables back to their original values
   viewStart(rv.viewStart);
   viewEnd(rv.viewEnd);
   orthographicMode(rv.orthographic);
   fogMode(rv.fog);
   fogStart(rv.fogStart);
   fogEnd(rv.fogEnd);
}
