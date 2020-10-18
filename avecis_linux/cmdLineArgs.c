#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "cmdLineArgs.h"

#define TRUE 1
#define FALSE 0


int stringCnt = 0;
char **strings;
int strings_allocated = FALSE;


int getArg(char *cmdLineStr, char flag, int argType, char **retArg)
{
   int i;
   int chInc;
   int chDec;
   int strInc;
   int cmdLineStrLen;
   int cmdLStrInc;
   int flagExists = FALSE;
   
   cmdLineStrLen = strlen(cmdLineStr);
   
   // does the flag exist
   for (i=0; i < cmdLineStrLen; i++)
   {
      if (cmdLineStr[i] == '-')
      if (cmdLineStr[i+1] == flag)
      {
         flagExists = TRUE;
         cmdLStrInc = i+2;
         break;
      }
   }
   
   if (flagExists == FALSE)
   return -1;
   
   // return a string with a one or zero if it is only a flag
   if (argType == only_flag)
   {
      strInc = stringCnt;
      strings[strInc] = (char *)malloc(2 * sizeof(char));
      stringCnt++;
      
      if (flagExists)
      strings[strInc][0] = '1';
      else
      strings[strInc][0] = '0';
      
      strings[strInc][1] = 0;
      
      *retArg = strings[strInc];
      return 0;
   }
   
   // ignore any white space at the beginning of the argument string
   for (i=cmdLStrInc; i < cmdLineStrLen; i++)
   {
      if ((cmdLineStr[i] == ' ') || (cmdLineStr[i] == '"') || (cmdLineStr[i] == '\''))
      continue;
      
      cmdLStrInc = i;
      break;
   }
   
   // get the length of the argument string
   chInc = 0;
   for (i=cmdLStrInc; i < cmdLineStrLen; i++)
   {
      if ((cmdLineStr[i] == '-') && (cmdLineStr[i+1] >= 'A') && (cmdLineStr[i+1] <= 'Z'))
      break;
      
      if ((cmdLineStr[i] == '-') && (cmdLineStr[i+1] >= 'a') && (cmdLineStr[i+1] <= 'z'))
      break;
      
      if ((cmdLineStr[i] == '"') || (cmdLineStr[i] == '\''))
      break;
      
      chInc++;
   }
   
   // allocate memory for the argument string
   strInc = stringCnt;
   strings[strInc] = (char *)malloc((chInc+1) * sizeof(char));
   stringCnt++;
   
   // get the argument string
   chInc = 0;
   for (i=cmdLStrInc; i < cmdLineStrLen; i++)
   {
      if ((cmdLineStr[i] == '-') && (cmdLineStr[i+1] >= 'A') && (cmdLineStr[i+1] <= 'Z'))
      break;
      
      if ((cmdLineStr[i] == '-') && (cmdLineStr[i+1] >= 'a') && (cmdLineStr[i+1] <= 'z'))
      break;
      
      if ((cmdLineStr[i] == '"') || (cmdLineStr[i] == '\''))
      break;
      
      strings[strInc][chInc] = cmdLineStr[i];
      chInc++;
   }
   
   // zero at the end of string
   strings[strInc][chInc] = 0;
   
   // erase any white space after the argument string
   chDec = 1;
   while (strings[strInc][chInc-chDec] == ' ')
   {
      strings[strInc][chInc-chDec] = 0;
      chDec--;
   }
   
   *retArg = strings[strInc];
   
   return 0;
}


void readCmdLineArgs(char *cmdLineStr, char *argFlags,
                     int *argTypes, int argCnt, void ***argList)
{
   int i;
   char *arg;
   float floatArg;
   
   // allocate memory for the arguments
   strings = (char **)malloc(argCnt * sizeof(char *));
   strings_allocated = TRUE;
   
   // get and format arguments depending on the type
   for (i=0; i < argCnt; i++)
   {
      if (argTypes[i] == string_arg)
      if (getArg(cmdLineStr, argFlags[i], argTypes[i], &arg) != -1)
      argList[i] = (void **)arg;
      
      if (argTypes[i] == int_arg)
      if (getArg(cmdLineStr, argFlags[i], argTypes[i], &arg) != -1)
      argList[i] = (void **)(uintptr_t)atoi(arg);
      
      if (argTypes[i] == float_arg)
      if (getArg(cmdLineStr, argFlags[i], argTypes[i], &arg) != -1)
      {
         floatArg = atof(arg);
         argList[i] = *(void **)&floatArg;
      }
      
      if (argTypes[i] == only_flag)
      if (getArg(cmdLineStr, argFlags[i], argTypes[i], &arg) != -1)
      argList[i] = (void **)(uintptr_t)atoi(arg);
   }
}


char *cmdLineStr_addr;
int cmdLineStr_allocated = FALSE;


void mergeCmdLineArgs(char *args[], int argCnt, char **cmdLineStr)
{
   int i;
   int strLength = 0;
   
   for (i=0; i < argCnt; i++)
   {
      strLength += strlen(args[i])+1;
   }
   
   *cmdLineStr = (char *)malloc(strLength+1);
   cmdLineStr_addr = *cmdLineStr;
   cmdLineStr_allocated = TRUE;
   
   for (i=0; i < argCnt; i++)
   {
      strcat(*cmdLineStr, " ");
      strcat(*cmdLineStr, args[i]);
   }
}


void freeCmdLineArgs()
{
   int i;
   
   if (strings_allocated)
   {
      for (i = 0; i < stringCnt; i++)
      {
         free(strings[i]);
      }
      
      free(strings);
      
      stringCnt = 0;
      strings_allocated = FALSE;
   }
   
   if (cmdLineStr_allocated)
   free(cmdLineStr_addr);
}
