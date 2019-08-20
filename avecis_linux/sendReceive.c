#include <stdio.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include "sendReceive.h"
#include "server.h"

#define TRUE 1
#define FALSE 0


int ClientSocket = -1;
int await_client_connect;
int receive_client_data;
pthread_t awaitClientConnection_Thread = -1;
pthread_t receiveData_Thread = -1;
int client_exists;


void *receiveData()
{
   int result;
   char recvBuff[4096];
   
   while (receive_client_data)
   {
      result = read(ClientSocket, (char *)&recvBuff[0], 4096);
      
      if (result < 0)
      printf("ERROR reading from socket\n");
      
      if (result > 0)
      receiveCallback(&recvBuff[0], result);
      
      if (result == 0)
      return NULL;
   }
   
   return NULL;
}


void *awaitClientConnection()
{
   int ClientSocketTemp = -1;
   char new_client_connect = 0;
   char end_client_connect = 1;
   
   if (acceptClient(&ClientSocket))
   return NULL;
   
   receiveCallback(&new_client_connect, 0);
   
   
   while (await_client_connect)
   {
      receive_client_data = TRUE;
      
      if (pthread_create(&receiveData_Thread, NULL, receiveData, NULL))
      {
         printf("Receive thread did not initialize.\n");
         return NULL;
      }
      
      if (acceptClient(&ClientSocketTemp))
      return NULL;
      
      receiveCallback(&end_client_connect, 0);
      
      if (client_exists)
      pthread_join(receiveData_Thread, NULL);
      
      close(ClientSocket);
      ClientSocket = ClientSocketTemp;
      
      receiveCallback(&new_client_connect, 0);
   }
   
   return NULL;
}


int sendData(char *bytes, int byteCnt)
{
   int result;
   
   result = write(ClientSocket, &bytes[0], byteCnt);
   
   if (result < 0)
   {
      client_exists = FALSE;
      printf("ERROR writing to socket\n");
   }
   else
   {
      client_exists = TRUE;
   }

   return result;
}


int iniSendReceiveServer(int port)
{
   iniServer(port);
   
   await_client_connect = TRUE;
   
   if (pthread_create(&awaitClientConnection_Thread, NULL, awaitClientConnection, NULL))
   {
      printf("Receive thread did not initialize.\n");
      return 1;
   }
   
   return 0;
}


void endSendReceiveServer()
{
   await_client_connect = FALSE;
   receive_client_data = FALSE;
   endServer(ClientSocket);
}
