#include <alsa/asoundlib.h>
#include <math.h>
#include <pthread.h>
#include "playSnd.h"

#define TRUE 1
#define FALSE 0
#define UNDEFINED -1

// Stream parameters: S16_LE, 2 channels
// Using transfer method: write_and_poll

#define FAILED_TO_INITIALIZE_AUDIO 1
#define PLAYBACK_ERROR 2

snd_pcm_t *handle;
int handle_valid = FALSE;
signed short *samples;
int sample_allocated = FALSE;
int sound_buffers_active = TRUE;

struct iniSndVars
{
   int sampRate;
   char *device;
};
pthread_t iniSndThread_HND;


int sampleCnt16bit = 0;
int framesPerBuffer = 0;

int bufferSpanCnt = UNDEFINED;
int bufferSpanInc = 0;

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


void stopSound()
{
   stopSoundPlay = TRUE;
}


void playSound(float *leftSnd, float *rightSnd, int samples)
{
   int i;
   
   // load the first buffer if both buffers are not busy
   if ((sampA.busy == FALSE) && (sampB.busy == FALSE))
   {
      sampA.leftBuff = (float *)malloc(samples * sizeof(float));
      sampA.rightBuff = (float *)malloc(samples * sizeof(float));
      
      for (i=0; i < samples; i++)
      {
         sampA.leftBuff[i] = leftSnd[i];
         sampA.rightBuff[i] = rightSnd[i];
      }
      
      sampA.read = 0;
      sampA.cnt = samples;
      sampA.busy = TRUE;
      
      samp = &sampA;
      return;
   }
   
   // load the second buffer if only the first buffer is busy
   if ((sampA.busy == TRUE) && (sampB.busy == FALSE))
   {
      sampB.leftBuff = (float *)malloc(samples * sizeof(float));
      sampB.rightBuff = (float *)malloc(samples * sizeof(float));
      
      for (i=0; i < samples; i++)
      {
         sampB.leftBuff[i] = leftSnd[i];
         sampB.rightBuff[i] = rightSnd[i];
      }
      
      sampB.read = 0;
      sampB.cnt = samples;
      sampB.busy = TRUE;
      
      return;
   }
   
   // load the first buffer if only the second buffer is busy
   if ((sampA.busy == FALSE) && (sampB.busy == TRUE))
   {
      sampA.leftBuff = (float *)malloc(samples * sizeof(float));
      sampA.rightBuff = (float *)malloc(samples * sizeof(float));
      
      for (i=0; i < samples; i++)
      {
         sampA.leftBuff[i] = leftSnd[i];
         sampA.rightBuff[i] = rightSnd[i];
      }
      
      sampA.read = 0;
      sampA.cnt = samples;
      sampA.busy = TRUE;
      
      return;
   }
}


void loadNextSegment(short *sndBuffer)
{
   int i;
   
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
      
      bufferSpanCnt = UNDEFINED;
      bufferSpanInc = 0;
      
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
      
      bufferSpanCnt = UNDEFINED;
      bufferSpanInc = 0;
      
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
      for (i=0; i < sampleCnt16bit; i++)
      sndBuffer[i] = 0;
   }
   
   if (samp->read < samp->cnt)
   {
      // reset the buffer count at the beginning
      // of each sound being played
      if (samp->read == 0)
      {
         bufferSpanInc = 0;
         bufferSpanCnt = samp->cnt / framesPerBuffer;
      }
      
      // load the output sound buffer
      for (i=0; i < sampleCnt16bit; i+=2)
      {
         if (samp->read < samp->cnt)
         {
            sndBuffer[i] = round(samp->leftBuff[samp->read] * 32767.0); // 32767 for 16 bits
            sndBuffer[i+1] = round(samp->rightBuff[samp->read] * 32767.0); // 32767 for 16 bits
         }
         else
         {
            sndBuffer[i] = 0;
            sndBuffer[i+1] = 0;
         }
         
         samp->read++;
      }
   }
}


int setParams(snd_pcm_t *handle, snd_pcm_hw_params_t *params, snd_pcm_sw_params_t *swparams,
              snd_pcm_access_t access, int framesPerBuff, int sampRate)
{
   int dir;
   unsigned int rate;
   unsigned int ringBuffLgth_uSec;
   unsigned int periodLgth_uSec;
   snd_pcm_uframes_t ringBuffSz;
   snd_pcm_sframes_t periodBuffSz;
   
   ringBuffLgth_uSec = (float)framesPerBuff / sampRate * 5000000; // ring buffer length in us
   periodLgth_uSec = (float)framesPerBuff / sampRate * 1000000; // period time in us

   // choose all parameters
   if (snd_pcm_hw_params_any(handle, params) < 0)
   return 1;
   
   // set hardware resampling
   if (snd_pcm_hw_params_set_rate_resample(handle, params, 1) < 0) // 1 = enable alsa-lib resampling
   return 1;
   
   // set the interleaved read/write format
   if (snd_pcm_hw_params_set_access(handle, params, access) < 0)
   return 1;
   
   // set the sample format
   if (snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16) < 0)
   return 1;
   
   // set the count of channels
   if (snd_pcm_hw_params_set_channels(handle, params, 2) < 0) // 2 channels
   return 1;
   
   // set the stream rate
   rate = sampRate;
   if (snd_pcm_hw_params_set_rate_near(handle, params, &rate, 0) < 0)
   return 1;
   
   if (rate != sampRate)
   return 1;
   
   // set the buffer time
   if (snd_pcm_hw_params_set_buffer_time_near(handle, params, &ringBuffLgth_uSec, &dir) < 0)
   return 1;
   
   if (snd_pcm_hw_params_get_buffer_size(params, &ringBuffSz) < 0)
   return 1;
   
   // set the period time
   if (snd_pcm_hw_params_set_period_time_near(handle, params, &periodLgth_uSec, &dir) < 0)
   return 1;
   
   if (snd_pcm_hw_params_get_period_size(params, (snd_pcm_uframes_t *)&periodBuffSz, &dir) < 0)
   return 1;
   
   if (periodBuffSz != framesPerBuff)
   return 1;
   
   // write the parameters to device
   if (snd_pcm_hw_params(handle, params) < 0)
   return 1;
   
   // get the current swparams
   if (snd_pcm_sw_params_current(handle, swparams) < 0)
   return 1;

   // start the transfer when the buffer is almost full:
   // (buffer_size / avail_min) * avail_min
   if (snd_pcm_sw_params_set_start_threshold(handle, swparams, (ringBuffSz / periodBuffSz) * periodBuffSz) < 0)
   return 1;

   // allow the transfer when at least period_size samples can be processed
   // or disable this mechanism when period event is enabled (aka interrupt like style processing)
   if (snd_pcm_sw_params_set_avail_min(handle, swparams, periodBuffSz) < 0)
   return 1;

   // write the parameters to the playback device
   if (snd_pcm_sw_params(handle, swparams) < 0)
   return 1;

   return 0;
}


int write_and_poll_loop(snd_pcm_t *handle, int sampRate, signed short *samples,
                        snd_pcm_sframes_t period_size, struct pollfd *ufd)
{
   double t;
   int tHalf;
   int wait_for_poll = FALSE;
   unsigned short revents;
   
   // find the amount of time it takes to play an audio segment half
   t = (double)period_size / sampRate;
   t = t * 1000000; // convert to microseconds
   tHalf = t / 2;
   
   while (sound_buffers_active)
   {
      usleep(tHalf);
      
      loadNextSegment(samples);
      
      if (bufferSpanInc <= bufferSpanCnt)// don't increment if greater than bufferSpanCnt
      bufferSpanInc++;
      
      // send a signal if at the last buffer load
      if (bufferSpanInc == bufferSpanCnt)
      lastAudioSegmentEventCallback();
      
      if (wait_for_poll)
      {
         // wait until buffer is ready roughly "tHalf"
         poll(ufd, 1, -1);
         snd_pcm_poll_descriptors_revents(handle, ufd, 1, &revents);
         
         if (revents & POLLERR)
         return PLAYBACK_ERROR;
      }
      
      if (snd_pcm_writei(handle, samples, period_size) < 0)
      return PLAYBACK_ERROR;
      
      if (snd_pcm_state(handle) == SND_PCM_STATE_RUNNING)
      wait_for_poll = TRUE;
   }
   
   return 0;
}


void closeSndBuffs()
{
   sound_buffers_active = FALSE;
   pthread_join(iniSndThread_HND, NULL);
}


void *iniSndThread(void *iniSndVarsPtr)
{
   snd_pcm_hw_params_t *hwparams;
   snd_pcm_sw_params_t *swparams;
   struct pollfd *ufd;
   int ufd_allocated;
   snd_output_t *output = NULL;
   struct iniSndVars *sv;
   
   sv = iniSndVarsPtr;
   
   snd_pcm_hw_params_alloca(&hwparams);
   snd_pcm_sw_params_alloca(&swparams);
   
   while (sound_buffers_active)
   {
      if (snd_output_stdio_attach(&output, stdout, 0) < 0)
      return (void *)FAILED_TO_INITIALIZE_AUDIO;
      
      if (snd_pcm_open(&handle, sv->device, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK) < 0)
      return (void *)FAILED_TO_INITIALIZE_AUDIO;
      else
      handle_valid = TRUE;
      
      if (setParams(handle, hwparams, swparams, SND_PCM_ACCESS_RW_INTERLEAVED, framesPerBuffer, sv->sampRate))
      return (void *)FAILED_TO_INITIALIZE_AUDIO;
      
      samples = malloc((framesPerBuffer * snd_pcm_format_physical_width(SND_PCM_FORMAT_S16) * 2) / 8); // 2 channels
      
      if (samples == NULL)
      return (void *)FAILED_TO_INITIALIZE_AUDIO;
      else
      sample_allocated = TRUE;
      
      ufd = malloc(sizeof(struct pollfd));
      
      if (ufd == NULL)
      return (void *)FAILED_TO_INITIALIZE_AUDIO;
      else
      ufd_allocated = TRUE;
      
      if ((snd_pcm_poll_descriptors(handle, ufd, 1)) < 0)
      return (void *)FAILED_TO_INITIALIZE_AUDIO;
      
      write_and_poll_loop(handle, sv->sampRate, samples, (snd_pcm_sframes_t)framesPerBuffer, ufd);
      
      if (sample_allocated)
      free(samples);
      
      if (ufd_allocated)
      free(ufd);
      
      if (handle_valid)
      snd_pcm_close(handle);
      
      sample_allocated = FALSE;
      handle_valid = FALSE;
   }
   
   return (void *)0;
}


int iniSndBuffs(int buffSize, int sampRate, char *device)
{
   static struct iniSndVars sv;
   
   // initiate global variables
   sampleCnt16bit = buffSize / 2; // 2 bytes per sample
   framesPerBuffer = buffSize / (2 * 2); // 2 bytes per sample and 2 channels
   
   sampA.read = 0;
   sampA.cnt = 0;
   sampA.busy = FALSE;
   
   sampB.read = 0;
   sampB.cnt = 0;
   sampB.busy = FALSE;
   
   samp = &sampA;

   // initiate thread
   sv.sampRate = sampRate;
   sv.device = device;
   
   if (pthread_create(&iniSndThread_HND, NULL, iniSndThread, (void *)&sv))
   return FAILED_TO_INITIALIZE_AUDIO;
   
   return 0;
}
