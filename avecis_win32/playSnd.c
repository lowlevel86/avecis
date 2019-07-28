#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <mmsystem.h>
#include "avecisHandler.h"

#define UNDEFINED -1
#define SOUND_DONE_SIGNAL 11

HWAVEOUT hWaveOut;
PWAVEHDR pWaveHdrA, pWaveHdrB;
char *pBufferA, *pBufferB;

int bufferSize = 0;
int bufferSampleCnt = 0;

int soundSpanBufferCnt = UNDEFINED;
int soundSpanBufferInc = 0;

struct samples
{
   float *leftBuff;
   float *rightBuff;
   int read;
   int cnt;
   int busy;
};

struct samples sampA;
struct samples sampB;
struct samples *samp;

int stopSoundPlay = FALSE;


// note: use before closing program
// to insure sound buffers are freed
void stopSound()
{
   stopSoundPlay = TRUE;
}


void playSound(float *leftSnd, float *rightSnd, int sndSamples)
{
   int i;
   int buffSamples;
   int remainder;
   
   // make sure the sound samples fit into buffer units
   buffSamples = sndSamples;
   
   remainder = sndSamples % bufferSampleCnt;
   
   if (remainder)
   buffSamples = sndSamples + (bufferSampleCnt - remainder);
   
   // load the first buffer if both buffers are not busy
   if ((sampA.busy == FALSE) && (sampB.busy == FALSE))
   {
      sampA.leftBuff = (float *)malloc(buffSamples * sizeof(float));
      sampA.rightBuff = (float *)malloc(buffSamples * sizeof(float));
      
      for (i=0; i < buffSamples; i++)
      {
         if (i < sndSamples)
         {
            sampA.leftBuff[i] = leftSnd[i];
            sampA.rightBuff[i] = rightSnd[i];
         }
         else
         {
            sampA.leftBuff[i] = 0;
            sampA.rightBuff[i] = 0;
         }
      }
      
      sampA.read = 0;
      sampA.cnt = buffSamples;
      sampA.busy = TRUE;
      
      samp = &sampA;
      return;
   }
   
   // load the second buffer if only the first buffer is busy
   if ((sampA.busy == TRUE) && (sampB.busy == FALSE))
   {
      sampB.leftBuff = (float *)malloc(buffSamples * sizeof(float));
      sampB.rightBuff = (float *)malloc(buffSamples * sizeof(float));
      
      for (i=0; i < buffSamples; i++)
      {
         if (i < sndSamples)
         {
            sampB.leftBuff[i] = leftSnd[i];
            sampB.rightBuff[i] = rightSnd[i];
         }
         else
         {
            sampB.leftBuff[i] = 0;
            sampB.rightBuff[i] = 0;
         }
      }
      
      sampB.read = 0;
      sampB.cnt = buffSamples;
      sampB.busy = TRUE;
      
      return;
   }
   
   // load the first buffer if only the second buffer is busy
   if ((sampA.busy == FALSE) && (sampB.busy == TRUE))
   {
      sampA.leftBuff = (float *)malloc(buffSamples * sizeof(float));
      sampA.rightBuff = (float *)malloc(buffSamples * sizeof(float));
      
      for (i=0; i < buffSamples; i++)
      {
         if (i < sndSamples)
         {
            sampA.leftBuff[i] = leftSnd[i];
            sampA.rightBuff[i] = rightSnd[i];
         }
         else
         {
            sampA.leftBuff[i] = 0;
            sampA.rightBuff[i] = 0;
         }
      }
      
      sampA.read = 0;
      sampA.cnt = buffSamples;
      sampA.busy = TRUE;
      
      return;
   }
}


void iniSndBuffs(HWND hwnd, int buffSize, int sampRate) // 16 bit
{
   WAVEFORMATEX waveformat;
   
   sampA.read = 0;
   sampA.cnt = 0;
   sampA.busy = FALSE;
   
   sampB.read = 0;
   sampB.cnt = 0;
   sampB.busy = FALSE;
   
   samp = &sampA;

   if (hWaveOut == NULL)
   {
      // Allocate memory for 2 headers and 2 buffers
      pWaveHdrA = (PWAVEHDR)malloc(sizeof(WAVEHDR));
      pWaveHdrB = (PWAVEHDR)malloc(sizeof(WAVEHDR));
      pBufferA = (char *)malloc(buffSize);
      pBufferB = (char *)malloc(buffSize);
      
      bufferSize = buffSize;
      bufferSampleCnt = buffSize / sizeof(float);
      
      
      // Open waveform audio for output
      
      waveformat.wFormatTag = WAVE_FORMAT_PCM;
      waveformat.nChannels = 2;
      waveformat.nSamplesPerSec = sampRate;
      waveformat.nAvgBytesPerSec = sampRate * 2 * 2; // sample rate * 2 bytes per sample * 2 channels
      waveformat.nBlockAlign = 4;
      waveformat.wBitsPerSample = 16;
      waveformat.cbSize = 0;
          
      if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveformat,
                     (DWORD) hwnd, 0, CALLBACK_WINDOW) != MMSYSERR_NOERROR)
      {
         free(pWaveHdrA);
         free(pWaveHdrB);
         free(pBufferA);
         free(pBufferB);
         
         hWaveOut = NULL;
         
         //error opening waveform audio device
         return;
      }
      
      // Set up headers and prepare them
      
      pWaveHdrA->lpData = (LPSTR)pBufferA;
      pWaveHdrA->dwBufferLength = buffSize;
      pWaveHdrA->dwBytesRecorded = 0;
      pWaveHdrA->dwUser = 0;
      pWaveHdrA->dwFlags = 0;
      pWaveHdrA->dwLoops = 1;
      pWaveHdrA->lpNext = NULL;
      pWaveHdrA->reserved = 0;
      
      waveOutPrepareHeader(hWaveOut, pWaveHdrA, sizeof(WAVEHDR));
      
      pWaveHdrB->lpData = (LPSTR)pBufferB;
      pWaveHdrB->dwBufferLength = buffSize;
      pWaveHdrB->dwBytesRecorded = 0;
      pWaveHdrB->dwUser = 0;
      pWaveHdrB->dwFlags = 0;
      pWaveHdrB->dwLoops = 1;
      pWaveHdrB->lpNext = NULL;
      pWaveHdrB->reserved = 0;
      
      waveOutPrepareHeader(hWaveOut, pWaveHdrB, sizeof(WAVEHDR));
   }
}


void loadNextSegment(char *sndBuffer)
{
   int i, sample;
   
   if (stopSoundPlay)
   {
      if (sampA.busy == TRUE)
      {
         free(sampA.leftBuff);
         free(sampA.rightBuff);
         
         sampA.read = 0;
         sampA.cnt = 0;
         sampA.busy = FALSE;
      }
      
      if (sampB.busy == TRUE)
      {
         free(sampB.leftBuff);
         free(sampB.rightBuff);
         
         sampB.read = 0;
         sampB.cnt = 0;
         sampB.busy = FALSE;
      }
      
      soundSpanBufferCnt = UNDEFINED;
      soundSpanBufferInc = 0;
      
      stopSoundPlay = FALSE;
   }
   
   // if two buffers are loaded
   // free one and play the other
   if ((sampA.busy == TRUE) && (sampB.busy == TRUE))
   {
      free(samp->leftBuff);
      free(samp->rightBuff);
      
      samp->read = 0;
      samp->cnt = 0;
      samp->busy = FALSE;
      
      soundSpanBufferCnt = UNDEFINED;
      soundSpanBufferInc = 0;
      
      if (sampA.busy == TRUE)
      samp = &sampA;
      
      if (sampB.busy == TRUE)
      samp = &sampB;
   }
   
   // if end of sound, free the buffer and check another buffer
   if (samp->read >= samp->cnt)
   {
      if (samp->busy == TRUE)
      {
         free(samp->leftBuff);
         free(samp->rightBuff);
         
         samp->read = 0;
         samp->cnt = 0;
         samp->busy = FALSE;
      }
      
      if (sampA.busy == TRUE)
      samp = &sampA;
      
      if (sampB.busy == TRUE)
      samp = &sampB;
   }
   
   // if no sound, fill the output sound buffer with zeros
   if ((sampA.busy == FALSE) && (sampB.busy == FALSE))
   {
      for (i=0; i < bufferSize; i++)
      sndBuffer[i] = 0;
   }
   
   if (samp->read < samp->cnt)
   {
      // reset the buffer count at the beginning
      // of each sound being played
      if (samp->read == 0)
      {
         soundSpanBufferInc = 0;
         soundSpanBufferCnt = samp->cnt / bufferSampleCnt;
      }
      
      // load the output sound buffer
      for (i=0; i < bufferSize; i+=4)
      {
         if (samp->read < samp->cnt)
         {
            sample = round(samp->leftBuff[samp->read] * 32767.0); // 32767 for 16 bits
            
            sndBuffer[i] = sample & 0x0000FF;
            sndBuffer[i+1] = (sample & 0x00FF00) >> 8;
            
            sample = round(samp->rightBuff[samp->read] * 32767.0); // 32767 for 16 bits
            
            sndBuffer[i+2] = sample & 0x0000FF;
            sndBuffer[i+3] = (sample & 0x00FF00) >> 8;
         }
         else
         {
            sndBuffer[i] = 0;
            sndBuffer[i+1] = 0;
            sndBuffer[i+2] = 0;
            sndBuffer[i+3] = 0;
         }
         
         samp->read++;
      }
   }
}


void loadPlaySnd(UINT message, LPARAM lParam, char *sndBuffer)
{
   static int loadBuffer = 1;
   
   // message generated from waveOutOpen call
   if (MM_WOM_OPEN == message)
   {
      // send two buffers to waveform output device
      
      if (loadBuffer == 1)
      {
         loadNextSegment(sndBuffer);
         waveOutWrite(hWaveOut, pWaveHdrA, sizeof(WAVEHDR));
         
         loadBuffer = 2;
         return;
      }
      
      if (loadBuffer == 2)
      {
         loadNextSegment(sndBuffer);
         waveOutWrite(hWaveOut, pWaveHdrB, sizeof(WAVEHDR));
         
         loadBuffer = 1;
         return;
      }
   }
   
   // message generated when a buffer is finished
   if (MM_WOM_DONE == message)
   {
      if (hWaveOut != NULL)
      {
         loadNextSegment(sndBuffer);
         waveOutWrite(hWaveOut, (PWAVEHDR)lParam, sizeof(WAVEHDR));
      }
   }
}


void soundToDevice(UINT message, LPARAM lParam)
{
   if (MM_WOM_CLOSE == message)
   {
      if (hWaveOut != NULL)
      {
         bufferSize = 0;
         
         waveOutReset(hWaveOut);
         waveOutClose(hWaveOut);
         
         waveOutUnprepareHeader(hWaveOut, pWaveHdrA, sizeof(WAVEHDR));
         waveOutUnprepareHeader(hWaveOut, pWaveHdrB, sizeof(WAVEHDR));
         
         free(pWaveHdrA);
         free(pWaveHdrB);
         free(pBufferA);
         free(pBufferB);
         
         hWaveOut = NULL;
      }
      
      return;
   }
   
   if (MM_WOM_OPEN == message)// load and play the first two sound segments
   {
      loadPlaySnd(message, lParam, pBufferA);
      loadPlaySnd(message, lParam, pBufferB);
   }

   if (MM_WOM_DONE == message)// load next sound segment after one is finished
   {
      loadPlaySnd(message, lParam, (char *)((PWAVEHDR) lParam)->lpData);
      
      if (soundSpanBufferInc <= soundSpanBufferCnt)
      soundSpanBufferInc++;
      
      // send a signal if at the last buffer load
      if (soundSpanBufferInc == soundSpanBufferCnt)
      sendInputEvent(SOUND_DONE_SIGNAL, 0);
   }
}
