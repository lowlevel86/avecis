#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "sendReceive.h"
#include "server.h"

#define TRUE 1
#define FALSE 0


int ListenSocket = -1;
int ClientSocket = -1;
int closeDataReceiver = FALSE;
pthread_t receiveData_Thread = -1;


void *receiveData(void *port_addr)
{
   int result;
   char recvBuff[4096];
   int port;
   int initializeServer = TRUE;
   
   port = (int)port_addr;
   
   while (closeDataReceiver == FALSE)
   {
      if (initializeServer)
      {
         if (iniServer(port, &ListenSocket, &ClientSocket))
         return NULL;
         
         // init/reset
         receiveCallback(&recvBuff[0], 0);
         
         initializeServer = FALSE;
      }
      
      result = read(ClientSocket, (char *)&recvBuff[0], 4096);
      
      if (result < 0)
      {
         printf("ERROR reading from socket\n");
         endServer(ListenSocket, ClientSocket);
         return NULL;
      }
      
      if (result > 0)
      receiveCallback(&recvBuff[0], result);
      
      if (result == 0)
      {
         initializeServer = TRUE;
         endServer(ListenSocket, ClientSocket);
      }
   }
   
   return NULL;
}


int sendData(char *bytes, int byteCnt)
{
   int result;
   
   // send data
   result = write(ClientSocket, &bytes[0], byteCnt);
   
   if (result < 0)
   printf("ERROR writing to socket\n");
   
   return result;
}


int iniSendReceiveServer(int port)
{
   void *port_addr;
   
   port_addr = (void *)port;
   
   closeDataReceiver = FALSE;
   if (pthread_create(&receiveData_Thread, NULL, receiveData, port_addr))
   {
      printf("Receive thread did not initialize.\n");
      return 1;
   }
   
   return 0;
}


void endSendReceiveServer()
{
   closeDataReceiver = TRUE;
   endServer(ListenSocket, ClientSocket);
   pthread_join(receiveData_Thread, NULL);
}
