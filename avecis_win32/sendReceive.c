#include <winsock2.h>
#include <stdio.h>
#include <stdint.h>
#include <process.h>
#include "sendReceive.h"
#include "server.h"

#define TRUE 1
#define FALSE 0


SOCKET ClientSocket = INVALID_SOCKET;
int await_client_connect;
int receive_client_data;
HANDLE awaitClientConnection_Thread = NULL;
HANDLE receiveData_Thread = NULL;


void receiveData()
{
   int iResult;
   char recvBuff[4096];
   
   while (receive_client_data)
   {
      iResult = recv(ClientSocket, (char *)&recvBuff[0], 4096, 0);
      
      if (iResult < 0)
      {
         printf("recv failed with error: %d\n", WSAGetLastError());
         endServer(ClientSocket);
         _endthread();
      }
      
      if (iResult > 0)
      receiveCallback(&recvBuff[0], iResult);
      
      if (iResult == 0)
      _endthread();
   }
   
   _endthread();
}


void awaitClientConnection()
{
   SOCKET ClientSocketTemp = INVALID_SOCKET;
   char new_client_connect = 0;
   char end_client_connect = 1;
   
   if (acceptClient(&ClientSocket))
   return;
   
   receiveCallback(&new_client_connect, 0);
   
   
   while (await_client_connect)
   {
      receive_client_data = TRUE;
      
      receiveData_Thread = (HANDLE)_beginthread(receiveData, 0, NULL);
      
      if (acceptClient(&ClientSocketTemp))
      return;
      
      receiveCallback(&end_client_connect, 0);
      
      WaitForSingleObject(receiveData_Thread, INFINITE);
      
      ClientSocket = ClientSocketTemp;
      
      receiveCallback(&new_client_connect, 0);
   }
   
   return;
}


int sendData(char *bytes, int byteCnt)
{
   int iResult;
   
   // send data
   iResult = send(ClientSocket, &bytes[0], byteCnt, 0);
   
   if (iResult == SOCKET_ERROR)
   printf("WSA: %d\n", WSAGetLastError());

   return iResult;
}


int iniSendReceiveServer(char *port)
{
   iniServer(port);
   
   await_client_connect = TRUE;
   
   awaitClientConnection_Thread = (HANDLE)_beginthread(awaitClientConnection, 0, NULL);
   
   return 0;
}


void endSendReceiveServer()
{
   await_client_connect = FALSE;
   receive_client_data = FALSE;
   endServer(ClientSocket);
   WaitForSingleObject(awaitClientConnection_Thread, INFINITE);
}
