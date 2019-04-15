#define CONNECTION_ESTABLISHED 1
#define CONNECTION_UNESTABLISHED 0

int iniSendReceiveServer(char *);
void receiveCallback(char *, int);
int sendData(char *, int);
void endSendReceiveServer();

#include <windows.h> // to send msg when connection status has changed
HWND winHwnd_glob;
int connection_status;
