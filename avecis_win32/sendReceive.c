#include <winsock2.h>
#include <stdio.h>
#include <stdint.h>
#include <process.h>
#include "sendReceive.h"
#include "server.h"

#define TRUE 1
#define FALSE 0
#define CONNECTION_ESTABLISHED 17


SOCKET ListenSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;
int closeDataReceiver = FALSE;
HANDLE receiveData_Thread = NULL;


void receiveData(void *port_addr)
{
   int iResult;
   char recvBuff[4096];
   char *port;
   int initializeServer = TRUE;
   
   port = port_addr;
   
   while (closeDataReceiver == FALSE)
   {
      if (initializeServer)
      {
         if (iniServer(port, &ListenSocket, &ClientSocket))
         _endthread();
         
         // the initial 4 bytes represent a data size variable used by receiveCallback()
         // no data is needed for the connection established message
         recvBuff[0] = 0;
         recvBuff[1] = 0;
         recvBuff[2] = 0;
         recvBuff[3] = 0;
         recvBuff[4] = CONNECTION_ESTABLISHED;
         receiveCallback(&recvBuff[0], 5);
         
         initializeServer = FALSE;
      }
      
      iResult = recv(ClientSocket, (char *)&recvBuff[0], 4096, 0);
      
      if (iResult < 0)
      {
         printf("recv failed with error: %d\n", WSAGetLastError());
         endServer(ListenSocket, ClientSocket);
         _endthread();
      }
      
      receiveCallback(&recvBuff[0], iResult);
      
      if (iResult == 0)
      {
         initializeServer = TRUE;
         endServer(ListenSocket, ClientSocket);
      }
   }
   
   _endthread();
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
   void *port_addr;
   
   port_addr = port;
   
   closeDataReceiver = FALSE;
   receiveData_Thread = (HANDLE)_beginthread(receiveData, 0, port_addr);
   
   return 0;
}


void endSendReceiveServer()
{
   closeDataReceiver = TRUE;
   endServer(ListenSocket, ClientSocket);
   WaitForSingleObject(receiveData_Thread, INFINITE);
}
