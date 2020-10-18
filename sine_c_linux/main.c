#include <math.h>
#include "avecis.h"

#define HOSTNAME "127.0.0.1"
#define PORT 27015

//compile with:
//gcc main.c avecis.c -lm -pthread -o sine

// Buffer size of avecis window can be increased
// Enter "avecis -h" into the terminal for usage
#define FRAMES_PER_BUFF 2048
#define SAMPLE_RATE 11025

float buffL[FRAMES_PER_BUFF];
float buffR[FRAMES_PER_BUFF];
int exit_prog = FALSE;

struct sineVars
{
   int sampRate;
   double vPrior;
   double hPrior;
};

void initGenSine(struct sineVars *sv, int sampleRate)
{
   sv->sampRate = sampleRate;
   sv->vPrior = 0.0;
   sv->hPrior = 1.0;
}

// generates smooth pitch changing sine wave
void genSine(struct sineVars *sv, float *output, float freqA, float freqB, int sampleCnt)
{
   int i;
   double degreesPi2A;
   double degreesPi2B;
   double degreesPi2;
   double distance;
   double c, s, v, h;
   
   degreesPi2A = 1.0 / sv->sampRate * freqA * M_PI * 2.0;
   degreesPi2B = 1.0 / sv->sampRate * freqB * M_PI * 2.0;
   
   distance = (degreesPi2B - degreesPi2A) / sampleCnt;
   
   for (i=0; i < sampleCnt; i++)
	{
      degreesPi2 = degreesPi2A + distance * i;
      
      c = cos(degreesPi2);
      s = sin(degreesPi2);
      
      v = sv->hPrior * s + sv->vPrior * c;
      h = sv->vPrior * -s + sv->hPrior * c;
      
      output[i] = sv->vPrior;
      sv->vPrior = v;
      sv->hPrior = h;
   }
}

void eventCallback(int evType, int keyCode, int xM, int yM)
{
   static int freqL = 0;
   static int freqR = 0;
   static float freqPriorL = 0;
   static float freqPriorR = 0;
   static struct sineVars svL, svR;
   
   if (evType == SOUND_DONE_SIGNAL)
   {
      genSine(&svL, &buffL[0], freqPriorL, freqL, FRAMES_PER_BUFF);
      genSine(&svR, &buffR[0], freqPriorR, freqR, FRAMES_PER_BUFF);
      freqPriorL = freqL;
      freqPriorR = freqR;
      playSound(&buffL[0], &buffR[0], FRAMES_PER_BUFF);
   }
   
   if (evType == MOUSE_MOVE)
   {
      freqL = xM;
      freqR = yM;
      printSB("LMB to play sound | RMB to stop | xM:%i yM:%i", xM, yM);
   }
   
   if (evType == L_MOUSE_DOWN)
   {
      initGenSine(&svL, SAMPLE_RATE);
      initGenSine(&svR, SAMPLE_RATE);
      genSine(&svL, &buffL[0], 0, 0, FRAMES_PER_BUFF);
      genSine(&svR, &buffR[0], 0, 0, FRAMES_PER_BUFF);
      playSound(&buffL[0], &buffR[0], FRAMES_PER_BUFF);
   }
   
   if (evType == R_MOUSE_DOWN)
   stopSound();
   
   if (keyCode == KC_ESC)
   exit_prog = TRUE;
}

int main(int argc, char *argv[])
{
   #define sine_sz 100*3
   int i;
   float sineWaveL[sine_sz];
   float sineWaveR[sine_sz];
   
   if (avecisConnect(HOSTNAME, PORT))
   return 1;
   
   antialiasingMode(OFF);
   
   printSB("LMB to play sound | RMB to stop");
   
   for (i=0; i < sine_sz; i+=3)
   {
      sineWaveL[i+0] = i - sine_sz/2;
      sineWaveL[i+2] = 0.0;
      sineWaveR[i+0] = i - sine_sz/2;
      sineWaveR[i+2] = 0.0;
   }
   
   while (exit_prog == FALSE)
   {
      for (i=0; i < sine_sz; i+=3) // sine_sz <= FRAMES_PER_BUFF
      {
         sineWaveL[i+1] = buffL[i] * 100;
         sineWaveR[i+1] = buffR[i] * 100;
      }
      
      setColor("\x55\x55\x55", 3);
      clearScreen();
      
      setColor("\x0\xFF\xFF", 3);
      drawPath(&sineWaveL[0], sine_sz);
      
      setColor("\xFF\x0\xFF", 3);
      drawPath(&sineWaveR[0], sine_sz);
      
      showContent();
      usleep(100000);
   }
   
   avecisDisconnect();
   return 0;
}
