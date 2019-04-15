#include <winsock2.h>
#include <stdio.h>
#include <stdint.h>
#include <process.h>
#include "sendReceive.h"
#include "server.h"

#define TRUE 1
#define FALSE 0


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
         
         connection_status = CONNECTION_ESTABLISHED;
         SendMessage(winHwnd_glob, WM_USER, (WPARAM)0, (LPARAM)0);
         
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
         
         connection_status = CONNECTION_UNESTABLISHED;
         SendMessage(winHwnd_glob, WM_USER, (WPARAM)0, (LPARAM)0);
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
   connection_status = CONNECTION_UNESTABLISHED;
   
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
