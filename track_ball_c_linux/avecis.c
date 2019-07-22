#include "avecis.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// SERVER CONNECTION FUNCTIONS /////////////////////////////////

int iniClient(char *hostname, int portNum, int *connectSocket)
{
   struct sockaddr_in serv_addr;
   struct hostent *server;
   
   *connectSocket = socket(AF_INET, SOCK_STREAM, 0);
   
   if (*connectSocket < 0)
   {
      perror("ERROR opening socket");
      return 1;
   }
   
   server = gethostbyname(hostname);
   
   if (server == NULL)
   {
      fprintf(stderr, "ERROR, no such host\n");
      return 1;
   }
   
   bzero((char *)&serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   
   serv_addr.sin_port = htons(portNum);
   
   if (connect(*connectSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
   {
      perror("ERROR connecting");
      return 1;
   }
   
   return 0;
}

void endClient(int connectSocket)
{
   close(connectSocket);
}

///////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// DATA TRANSFER FUNCTIONS /////////////////////////////////

int sendReceive_ConnectSocket = -1;
int sendReceive_CloseDataReceiver = FALSE;
pthread_t sendReceive_ReceiveData_Thread = -1;

void receiveCallback(char *, int);

void *receiveData(void *receiveCallback_addr)
{
   int result;
   char recvBuff[4096];
   void (*receiveCallback)(char *, int);
   
   receiveCallback = receiveCallback_addr;
   
   while (sendReceive_CloseDataReceiver == FALSE)
   {
      result = read(sendReceive_ConnectSocket, (char *)&recvBuff[0], 4096);
      
      if (result < 0)
      {
         printf("ERROR reading from socket");
         endClient(sendReceive_ConnectSocket);
         return NULL;
      }
      
      receiveCallback(&recvBuff[0], result);
      
      if (result == 0)
      {
         sendReceive_CloseDataReceiver = TRUE;
         endClient(sendReceive_ConnectSocket);
      }
   }
   
   return NULL;
}

int sendData(char *bytes, int byteCnt)
{
   int result;
   
   // send data
   result = write(sendReceive_ConnectSocket, &bytes[0], byteCnt);
   
   if (result < 0)
   printf("ERROR writing to socket\n");
   
   return result;
}

int iniSendReceiveClient(char *hostname, int port)
{
   void *receiveCallback_addr;
   
   receiveCallback_addr = receiveCallback;
   
   if (iniClient(hostname, port, &sendReceive_ConnectSocket))
   {
      printf("Client did not initialize.\n");
      return 1;
   }
   
   if(pthread_create(&sendReceive_ReceiveData_Thread, NULL, receiveData, (void *)receiveCallback_addr))
   {
      printf("Receive thread did not initialize.\n");
      return 1;
   }
   
   return 0;
}

void waitUntilReceiverEnd()
{
   pthread_join(sendReceive_ReceiveData_Thread, NULL);
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

pthread_mutex_t avecis_sendData_lock = PTHREAD_MUTEX_INITIALIZER;

void eventCallback(int, int, int, int);

void viewStart(float value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_VIEW_START;
   opDataHead[1] = 4;
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 4);
   
   pthread_mutex_unlock(&avecis_sendData_lock);
}

void viewEnd(float value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_VIEW_END;
   opDataHead[1] = 4;
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 4);
   
   pthread_mutex_unlock(&avecis_sendData_lock);
}

void setPerspective(float value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_PERSPECTIVE;
   opDataHead[1] = 4;
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 4);
   
   pthread_mutex_unlock(&avecis_sendData_lock);
}

void orthographicMode(int value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_ORTHOGRAPHIC_MODE;
   opDataHead[1] = 1;
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 1);
   
   pthread_mutex_unlock(&avecis_sendData_lock);
}

void fogMode(int value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_FOG_MODE;
   opDataHead[1] = 1;
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 1);
   
   pthread_mutex_unlock(&avecis_sendData_lock);
}

void fogColor()
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_FOG_COLOR;
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   pthread_mutex_unlock(&avecis_sendData_lock);
}

void fogStart(float value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_FOG_START;
   opDataHead[1] = 4;
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 4);
   
   pthread_mutex_unlock(&avecis_sendData_lock);
}

void fogEnd(float value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_FOG_END;
   opDataHead[1] = 4;
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 4);
   
   pthread_mutex_unlock(&avecis_sendData_lock);
}

void antialiasingMode(int value)
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SET_ANTIALIASING_MODE;
   opDataHead[1] = 1;
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&value, 1);
   
   pthread_mutex_unlock(&avecis_sendData_lock);
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
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&colorData[0], size);
   
   pthread_mutex_unlock(&avecis_sendData_lock);
}

void clearScreen()
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = CLEAR_SCREEN;
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   pthread_mutex_unlock(&avecis_sendData_lock);
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
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&coordData[0], size);
   
   pthread_mutex_unlock(&avecis_sendData_lock);
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
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&coordData[0], size);
   
   pthread_mutex_unlock(&avecis_sendData_lock);
}

void showContent()
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = SHOW_CONTENT;
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   pthread_mutex_unlock(&avecis_sendData_lock);
}

void printSB(char *fmtStr, ...)
{
   va_list arg;
   char opDataHead[5];
   char *buff;
   int size;
   
   va_start(arg, fmtStr);
   
   size = vsnprintf(NULL, 0, fmtStr, arg) + 1;
   buff = malloc(size);
   vsnprintf(buff, size, fmtStr, arg);
   
   va_end(arg);
   
   // place the type and size into the data head array
   opDataHead[0] = PRINT_STATUS;
   opDataHead[1] = size & 0xFF;
   opDataHead[2] = (size >> 8) & 0xFF;
   opDataHead[3] = (size >> 16) & 0xFF;
   opDataHead[4] = (size >> 24) & 0xFF;
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData(&buff[0], size);
   
   pthread_mutex_unlock(&avecis_sendData_lock);
   
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
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   // send the data
   sendData((char *)&leftSnd[0], sampleCnt * sizeof(float));
   sendData((char *)&rightSnd[0], sampleCnt * sizeof(float));
   
   pthread_mutex_unlock(&avecis_sendData_lock);
}

void stopSound()
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = STOP_SOUND;
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   pthread_mutex_unlock(&avecis_sendData_lock);
}

void endTransmission()
{
   char opDataHead[5] = {0};
   
   // place the type and size into the data head array
   opDataHead[0] = END_TRANSMISSION;
   
   pthread_mutex_lock(&avecis_sendData_lock);
   
   // send the data head array
   sendData(&opDataHead[0], sizeof(opDataHead));
   
   pthread_mutex_unlock(&avecis_sendData_lock);
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
         exit(-1);
         
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
   endSendReceiveClient();
}

// this function is not to be used if
// blockAvecisDisconnect() and
// unblockAvecisDisconnect() is used
void avecisDisconnect()
{
   closeDataReceiver();
   endTransmission();
   waitUntilReceiverEnd();
   endSendReceiveClient();
}

/////////////////////////////////////////////////////////////////////////////////////////
