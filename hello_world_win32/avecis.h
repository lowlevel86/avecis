#define _WIN32_WINNT 0x0501

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <winsock2.h>
#include <process.h>
#include <ws2tcpip.h>

#define TRUE 1
#define FALSE 0

#define ON 1
#define OFF 0

#define KEY_DOWN 0
#define KEY_UP 1
#define L_MOUSE_DOWN 2
#define L_MOUSE_UP 3
#define R_MOUSE_DOWN 4
#define R_MOUSE_UP 5
#define M_MOUSE_DOWN 6
#define M_MOUSE_UP 7
#define MOUSE_MOVE 8
#define MOUSE_WHEEL_UP 9
#define MOUSE_WHEEL_DOWN 10
#define SOUND_DONE_SIGNAL 11
#define DISCONNECT_SIGNAL 0xFF

#define KC_BACKSPACE 0x8
#define KC_TAB 0x9
#define KC_ENTER 0xD
#define KC_NP_ENTER 0x100000D
#define KC_LEFT_SHIFT 0x10
#define KC_RIGHT_SHIFT 0x2000010
#define KC_LEFT_CTRL 0x11
#define KC_RIGHT_CTRL 0x1000011
#define KC_LEFT_ALT 0x12
#define KC_RIGHT_ALT 0x1000012
#define KC_PAUSE 0x13
#define KC_CAPS_LOCK 0x14
#define KC_ESC 0x1B
#define KC_SPACE 0x20

#define KC_PAGE_UP 0x1000021
#define KC_PAGE_DOWN 0x1000022
#define KC_END 0x1000023
#define KC_HOME 0x1000024

#define KC_LEFT 0x1000025
#define KC_UP 0x1000026
#define KC_RIGHT 0x1000027
#define KC_DOWN 0x1000028

#define KC_PRINT_SCREEN 0x100002C
#define KC_INSERT 0x100002D
#define KC_DELETE 0x100002E

#define KC_NP_DECIMAL 0x2E

#define KC_0 0x30
#define KC_1 0x31
#define KC_2 0x32
#define KC_3 0x33
#define KC_4 0x34
#define KC_5 0x35
#define KC_6 0x36
#define KC_7 0x37
#define KC_8 0x38
#define KC_9 0x39

#define KC_A 0x41
#define KC_B 0x42
#define KC_C 0x43
#define KC_D 0x44
#define KC_E 0x45
#define KC_F 0x46
#define KC_G 0x47
#define KC_H 0x48
#define KC_I 0x49
#define KC_J 0x4A
#define KC_K 0x4B
#define KC_L 0x4C
#define KC_M 0x4D
#define KC_N 0x4E
#define KC_O 0x4F
#define KC_P 0x50
#define KC_Q 0x51
#define KC_R 0x52
#define KC_S 0x53
#define KC_T 0x54
#define KC_U 0x55
#define KC_V 0x56
#define KC_W 0x57
#define KC_X 0x58
#define KC_Y 0x59
#define KC_Z 0x5A

#define KC_LEFT_MENU 0x100005B
#define KC_RIGHT_MENU 0x100005D

#define KC_NP_0 0x60
#define KC_NP_1 0x61
#define KC_NP_2 0x62
#define KC_NP_3 0x63
#define KC_NP_4 0x64
#define KC_NP_5 0x65
#define KC_NP_6 0x66
#define KC_NP_7 0x67
#define KC_NP_8 0x68
#define KC_NP_9 0x69

#define KC_NP_MULTIPLY 0x6A
#define KC_NP_PLUS 0x6B
#define KC_NP_MINUS 0x6D
#define KC_NP_DIVIDE 0x6F

#define KC_F1 0x70
#define KC_F2 0x71
#define KC_F3 0x72
#define KC_F4 0x73
#define KC_F5 0x74
#define KC_F6 0x75
#define KC_F7 0x76
#define KC_F8 0x77
#define KC_F9 0x78
#define KC_F10 0x79
#define KC_F11 0x7A
#define KC_F12 0x7B

#define KC_NUM_LOCK 0x1000090
#define KC_SCROLL_LOCK 0x91

#define KC_SEMICOLON 0xBA
#define KC_PLUS 0xBB
#define KC_COMMA 0xBC
#define KC_MINUS 0xBD
#define KC_PERIOD 0xBE
#define KC_SLASH 0xBF
#define KC_BACKQUOTE 0xC0
#define KC_LEFT_BOX_BRACKET 0xDB
#define KC_BACKSLASH 0xDC
#define KC_RIGHT_BOX_BRACKET 0xDD
#define KC_SINGLE_QUOTE 0xDE


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
   
   // place the size and type into the data head array
   opDataHead[0] = 4;
   opDataHead[4] = SET_VIEW_START;
   
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
   
   // place the size and type into the data head array
   opDataHead[0] = 4;
   opDataHead[4] = SET_VIEW_END;
   
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
   
   // place the size and type into the data head array
   opDataHead[0] = 4;
   opDataHead[4] = SET_PERSPECTIVE;
   
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
   
   // place the size and type into the data head array
   opDataHead[0] = 1;
   opDataHead[4] = SET_ORTHOGRAPHIC_MODE;
   
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
   
   // place the size and type into the data head array
   opDataHead[0] = 1;
   opDataHead[4] = SET_FOG_MODE;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 1);
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void fogColor(int value)
{
   char opDataHead[5] = {0};
   
   // place the size and type into the data head array
   opDataHead[0] = 4;
   opDataHead[4] = SET_FOG_COLOR;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 4);
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void fogStart(float value)
{
   char opDataHead[5] = {0};
   
   // place the size and type into the data head array
   opDataHead[0] = 4;
   opDataHead[4] = SET_FOG_START;
   
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
   
   // place the size and type into the data head array
   opDataHead[0] = 4;
   opDataHead[4] = SET_FOG_END;
   
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
   
   // place the size and type into the data head array
   opDataHead[0] = 1;
   opDataHead[4] = SET_ANTIALIASING_MODE;
   
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
   
   // place the size and type into the data head array
   opDataHead[0] = size & 0xFF;
   opDataHead[1] = (size >> 8) & 0xFF;
   opDataHead[2] = (size >> 16) & 0xFF;
   opDataHead[3] = (size >> 24) & 0xFF;
   opDataHead[4] = SET_COLOR;
   
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
   
   // place the size and type into the data head array
   opDataHead[4] = CLEAR_SCREEN;
   
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
   
   // place the size and type into the data head array
   opDataHead[0] = size & 0xFF;
   opDataHead[1] = (size >> 8) & 0xFF;
   opDataHead[2] = (size >> 16) & 0xFF;
   opDataHead[3] = (size >> 24) & 0xFF;
   opDataHead[4] = DRAW_LINE;
   
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
   
   // place the size and type into the data head array
   opDataHead[0] = size & 0xFF;
   opDataHead[1] = (size >> 8) & 0xFF;
   opDataHead[2] = (size >> 16) & 0xFF;
   opDataHead[3] = (size >> 24) & 0xFF;
   opDataHead[4] = DRAW_PATH;
   
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
   
   // place the size and type into the data head array
   opDataHead[4] = SHOW_CONTENT;
   
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
   
   size = snprintf(NULL, 0, fmtStr, arg) + 1;
   buff = malloc(size);
   vsprintf(buff, fmtStr, arg);
   
   va_end(arg);
   
   // place the size and type into the data head array
   opDataHead[0] = size & 0xFF;
   opDataHead[1] = (size >> 8) & 0xFF;
   opDataHead[2] = (size >> 16) & 0xFF;
   opDataHead[3] = (size >> 24) & 0xFF;
   opDataHead[4] = PRINT_STATUS;
   
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
   
   // place the size and type into the data head array
   opDataHead[0] = size & 0xFF;
   opDataHead[1] = (size >> 8) & 0xFF;
   opDataHead[2] = (size >> 16) & 0xFF;
   opDataHead[3] = (size >> 24) & 0xFF;
   opDataHead[4] = PLAY_SOUND;
   
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
   
   // place the size and type into the data head array
   opDataHead[4] = STOP_SOUND;
   
   WaitForSingleObject(avecis_sendData_lock, INFINITE);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   ReleaseSemaphore(avecis_sendData_lock, 1, NULL);
}

void endTransmission()
{
   char opDataHead[5] = {0};
   
   // place the size and type into the data head array
   opDataHead[4] = END_TRANSMISSION;
   
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
