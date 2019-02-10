#define _WIN32_WINNT 0x0501

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


int iniServer(char *portNumStr, SOCKET *ListenSocket, SOCKET *ClientSocket)
{
   WSADATA wsaData;
   struct addrinfo *result = NULL;
   struct addrinfo hints;
   int iResult;

   // Initialize Winsock
   iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if (iResult != 0)
   {
      printf("WSAStartup failed with error: %d\n", iResult);
      return 1;
   }

   ZeroMemory(&hints, sizeof(hints));
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;
   hints.ai_flags = AI_PASSIVE;

   // Resolve the server address and port
   iResult = getaddrinfo(NULL, portNumStr, &hints, &result);
   if (iResult != 0)
   {
      printf("getaddrinfo failed with error: %d\n", iResult);
      WSACleanup();
      return 1;
   }

   // Create a SOCKET for connecting to server
   *ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
   if (*ListenSocket == INVALID_SOCKET)
   {
      printf("socket failed with error: %d\n", WSAGetLastError());
      freeaddrinfo(result);
      WSACleanup();
      return 1;
   }

   int enable = 1;
   if (setsockopt(*ListenSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&enable, sizeof(int)) < 0)
   printf("setsockopt(SO_REUSEADDR) failed");
   
   // Setup the TCP listening socket
   iResult = bind(*ListenSocket, result->ai_addr, (int)result->ai_addrlen);
   if (iResult == SOCKET_ERROR)
   {
      printf("bind failed with error: %d\n", WSAGetLastError());
      freeaddrinfo(result);
      closesocket(*ListenSocket);
      WSACleanup();
      return 1;
   }

   freeaddrinfo(result);

   iResult = listen(*ListenSocket, SOMAXCONN);
   if (iResult == SOCKET_ERROR)
   {
      printf("listen failed with error: %d\n", WSAGetLastError());
      closesocket(*ListenSocket);
      WSACleanup();
      return 1;
   }

   // Accept a client socket
   *ClientSocket = accept(*ListenSocket, NULL, NULL);

   if (*ClientSocket == INVALID_SOCKET)
   {
      printf("WSA:%d\n", WSAGetLastError());
      closesocket(*ListenSocket);
      WSACleanup();
      return 1;
   }

   // No longer need server socket
   closesocket(*ListenSocket);

   return 0;
}


void endServer(SOCKET ListenSocket, SOCKET ClientSocket)
{
   // cleanup
   closesocket(ClientSocket);
   closesocket(ListenSocket);
   WSACleanup();
}
