#include "avecis.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// SERVER CONNECTION FUNCTIONS /////////////////////////////////

int iniClient(char *hostname, int portNum, SOCKET *ConnectSocket)
{
   WSADATA wsaData;
   struct addrinfo *result = NULL;
   struct addrinfo *ptr = NULL;
   struct addrinfo hints;
   int iResult;
   char portNumStr[16];

   // Initialize Winsock
   iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
   if (iResult != 0)
   {
      printf("WSAStartup failed with error: %d\n", iResult);
      return 1;
   }

   ZeroMemory(&hints, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = IPPROTO_TCP;

   // Resolve the server address and port
   sprintf(portNumStr, "%d", portNum);
   iResult = getaddrinfo(hostname, portNumStr, &hints, &result);
   if (iResult != 0)
   {
      printf("getaddrinfo failed with error: %d\n", iResult);
      WSACleanup();
      return 1;
   }

   // Attempt to connect to an address until one succeeds
   for(ptr=result; ptr != NULL ;ptr=ptr->ai_next)
   {
      // Create a SOCKET for connecting to server
      *ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
      
      if (*ConnectSocket == INVALID_SOCKET)
      {
         printf("socket failed with error: %d\n", WSAGetLastError());
         WSACleanup();
         return 1;
      }
      
      // Connect to server.
      iResult = connect(*ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
      
      if (iResult == SOCKET_ERROR)
      {
         closesocket(*ConnectSocket);
         *ConnectSocket = INVALID_SOCKET;
         continue;
      }
      
      break;
   }

   freeaddrinfo(result);

   if (*ConnectSocket == INVALID_SOCKET)
   {
      printf("Unable to connect to server!\n");
      WSACleanup();
      return 1;
   }

   return 0;
}

void endClient(SOCKET ConnectSocket)
{
   // cleanup
   closesocket(ConnectSocket);
   WSACleanup();
}

///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// DATA TRANSFER FUNCTIONS /////////////////////////////////

SOCKET sendReceive_ConnectSocket = INVALID_SOCKET;
int sendReceive_CloseDataReceiver = FALSE;
HANDLE sendReceive_ReceiveData_Thread = NULL;

void receiveCallback(char *, int);

unsigned int __stdcall receiveData(void *receiveCallback_addr)
{
   int iResult;
   char recvBuff[4096];
   void (*receiveCallback)(char *, int);
   
   receiveCallback = receiveCallback_addr;
   
   while (sendReceive_CloseDataReceiver == FALSE)
   {
      iResult = recv(sendReceive_ConnectSocket, (char *)&recvBuff[0], 4096, 0);
      
      if (iResult < 0)
      {
         printf("recv failed with error: %d\n", WSAGetLastError());
         endClient(sendReceive_ConnectSocket);
         return 1;
      }
      
      receiveCallback(&recvBuff[0], iResult);
      
      if (iResult == 0)
      {
         sendReceive_CloseDataReceiver = TRUE;
         endClient(sendReceive_ConnectSocket);
      }
   }
   
   return 0;
}

int sendData(char *bytes, int byteCnt)
{
   int iResult;
   
   // send data
   iResult = send(sendReceive_ConnectSocket, &bytes[0], byteCnt, 0);
   
   if (iResult == SOCKET_ERROR)
   printf("send failed with error: %d\n", WSAGetLastError());

   return iResult;
}

int iniSendReceiveClient(char *hostname, int port)
{
   unsigned ThreadId;
   void *receiveCallback_addr;
   
   receiveCallback_addr = receiveCallback;
   
   if (iniClient(hostname, port, &sendReceive_ConnectSocket))
   {
      printf("Client did not initialize.\n");
      return 1;
   }
   
   sendReceive_ReceiveData_Thread = (HANDLE)_beginthreadex(NULL, 0, &receiveData, receiveCallback_addr, 0, &ThreadId);
   
   return 0;
}

void waitUntilReceiverEnd()
{
   WaitForSingleObject(sendReceive_ReceiveData_Thread, INFINITE);
}

void closeDataReceiver()
{
   sendReceive_CloseDataReceiver = TRUE;
}

void endSendReceiveClient()
{
   sendReceive_CloseDataReceiver = TRUE;
   endClient(sendReceive_ConnectSocket);
}

///////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// MAIN AVECIS FUNCTIONS /////////////////////////////////

#define SET_VIEW_START 0
#define SET_VIEW_END 1
#define SET_PERSPECTIVE 2
#define SET_ORTHOGRAPHIC_MODE 3
#define SET_FOG_MODE 4
#define SET_FOG_COLOR 5
#define SET_FOG_START 6
#define SET_FOG_END 7
#define SET_ANTIALIASING_MODE 8
#define SET_COLOR 9
#define CLEAR_SCREEN 10
#define DRAW_LINE 11
#define DRAW_PATH 12
#define SHOW_CONTENT 13
#define PRINT_STATUS 14
#define PLAY_SOUND 15
#define STOP_SOUND 16
#define END_TRANSMISSION 0xFF

HANDLE avecis_sendData_lock;

void eventCallback(int, int, int, int);

void viewStart(float value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_VIEW_START;
   opDataHead[1] = 4;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 4);
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void viewEnd(float value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_VIEW_END;
   opDataHead[1] = 4;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 4);
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void setPerspective(float value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_PERSPECTIVE;
   opDataHead[1] = 4;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 4);
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void orthographicMode(int value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_ORTHOGRAPHIC_MODE;
   opDataHead[1] = 1;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 1);
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void fogMode(int value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_FOG_MODE;
   opDataHead[1] = 1;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 1);
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void fogColor()
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_FOG_COLOR;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void fogStart(float value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_FOG_START;
   opDataHead[1] = 4;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 4);
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void fogEnd(float value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_FOG_END;
   opDataHead[1] = 4;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 4);
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void antialiasingMode(int value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_ANTIALIASING_MODE;
   opDataHead[1] = 1;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 1);
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void setColor(char *colorData, int byteDataCnt)
{
   char opDataHead[5];
   int size;
   
   // size in bytes
   size = byteDataCnt;
   
   // place the type and size into the data head array
   opDataHead[0] = SET_COLOR;
   opDataHead[1] = size & 0xFF;
   opDataHead[2] = (size >> 8) & 0xFF;
   opDataHead[3] = (size >> 16) & 0xFF;
   opDataHead[4] = (size >> 24) & 0xFF;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&colorData[0], size);
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void clearScreen()
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = CLEAR_SCREEN;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void drawLine(float *coordData, int floatDataCnt)
{
   char opDataHead[5];
   int size;
   
   // size in bytes
   size = floatDataCnt * sizeof(float);
   
   // place the type and size into the data head array
   opDataHead[0] = DRAW_LINE;
   opDataHead[1] = size & 0xFF;
   opDataHead[2] = (size >> 8) & 0xFF;
   opDataHead[3] = (size >> 16) & 0xFF;
   opDataHead[4] = (size >> 24) & 0xFF;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&coordData[0], size);
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void drawPath(float *coordData, int floatDataCnt)
{
   char opDataHead[5];
   int size;
   
   // size in bytes
   size = floatDataCnt * sizeof(float);
   
   // place the type and size into the data head array
   opDataHead[0] = DRAW_PATH;
   opDataHead[1] = size & 0xFF;
   opDataHead[2] = (size >> 8) & 0xFF;
   opDataHead[3] = (size >> 16) & 0xFF;
   opDataHead[4] = (size >> 24) & 0xFF;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&coordData[0], size);
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void showContent()
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SHOW_CONTENT;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void printSB(char *fmtStr, ...)
{
   va_list arg;
   char opDataHead[5];
   char *buff;
   int size;
   
   va_start(arg, fmtStr);
   size = vsnprintf(NULL, 0, fmtStr, arg) + 1;
   va_end(arg);
   
   buff = malloc(size);
   
   va_start(arg, fmtStr);
   vsnprintf(buff, size, fmtStr, arg);
   va_end(arg);
   
   // place the type and size into the data head array
   opDataHead[0] = PRINT_STATUS;
   opDataHead[1] = size & 0xFF;
   opDataHead[2] = (size >> 8) & 0xFF;
   opDataHead[3] = (size >> 16) & 0xFF;
   opDataHead[4] = (size >> 24) & 0xFF;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData(&buff[0], size);
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
   
   free(buff);
}

void playSound(float *leftSnd, float *rightSnd, int sampleCnt)
{
   char opDataHead[5];
   int size;
   
   // size in bytes
   size = sampleCnt * sizeof(float) * 2;
   
   // place the type and size into the data head array
   opDataHead[0] = PLAY_SOUND;
   opDataHead[1] = size & 0xFF;
   opDataHead[2] = (size >> 8) & 0xFF;
   opDataHead[3] = (size >> 16) & 0xFF;
   opDataHead[4] = (size >> 24) & 0xFF;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&leftSnd[0], sampleCnt * sizeof(float));
   sendData((char *)&rightSnd[0], sampleCnt * sizeof(float));
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void stopSound()
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = STOP_SOUND;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void endTransmission()
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = END_TRANSMISSION;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void receiveCallback(char *bytes, int byteCnt)
{
   int byteInc = 0;
   static int varsDataInc = 0;
   static int eventType, eventData;
   
   if (byteCnt == 0)
   {
      varsDataInc = 0;
      return;
   }
   
   while (TRUE)
   {
      if (varsDataInc == 0)
      {
         eventType = bytes[byteInc] & 0xFF;
         byteInc++;
         varsDataInc++;
      }
      if (byteInc >= byteCnt)
      return;
      
      if (varsDataInc == 1)
      {
         eventData = bytes[byteInc] & 0xFF;
         byteInc++;
         varsDataInc++;
      }
      if (byteInc >= byteCnt)
      return;
      
      if (varsDataInc == 2)
      {
         eventData |= ((int)bytes[byteInc] << 8) & 0xFF00;
         byteInc++;
         varsDataInc++;
      }
      if (byteInc >= byteCnt)
      return;
      
      if (varsDataInc == 3)
      {
         eventData |= ((int)bytes[byteInc] << 16) & 0xFF0000;
         byteInc++;
         varsDataInc++;
      }
      if (byteInc >= byteCnt)
      return;
      
      if (varsDataInc == 4)
      {
         eventData |= ((int)bytes[byteInc] << 24) & 0xFF000000;
         byteInc++;
         
         if (eventType == DISCONNECT_SIGNAL)
         {
            CloseHandle(avecis_sendData_lock);
            endSendReceiveClient();
            exit(0);
         }
         
         if (eventType == MOUSE_MOVE)
         eventCallback(eventType, 0, eventData&0xFFFF, eventData>>16&0xFFFF);
         else
         eventCallback(eventType, eventData, 0, 0);
         
         varsDataInc = 0;
      }
      if (byteInc >= byteCnt)
      return;
   }
}

int avecisConnect(char *hostname, int port)
{
   avecis_sendData_lock = CreateSemaphore(NULL, 1, 1, NULL);
   
   if (iniSendReceiveClient(hostname, port))
   return 1;
   
   return 0;
}

// this function is not to be used if avecisDisconnect() is used
void blockAvecisDisconnect()
{
   waitUntilReceiverEnd();
}

// this function is not to be used if avecisDisconnect() is used
void unblockAvecisDisconnect()
{
   // blockAvecisDisconnect() must be called first
   CloseHandle(avecis_sendData_lock);
   endSendReceiveClient();
}

// this function is not to be used if
// blockAvecisDisconnect() and
// unblockAvecisDisconnect() is used
void avecisDisconnect()
{
   CloseHandle(avecis_sendData_lock);
   closeDataReceiver();
   endTransmission();
   waitUntilReceiverEnd();
   endSendReceiveClient();
}

/////////////////////////////////////////////////////////////////////////////////////////
