int iniSendReceiveServer(char *);
void receiveCallback(char *, int);
int sendData(char *, int);
void endSendReceiveServer();

#include <windows.h> // to send msg when a connection is established
#define CONNECTION_ESTABLISHED 1
HWND winHwnd_glob;
