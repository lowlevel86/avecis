#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>

#define TRUE 1
#define FALSE 0

int ListenSocket = -1;
socklen_t clilen;
struct sockaddr_in cli_addr;

int iniServer(int portNum)
{
   struct sockaddr_in serv_addr;
   
   signal(SIGPIPE, SIG_IGN);
   
   ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
   
   if (ListenSocket < 0)
   {
      perror("ERROR opening socket\n");
      return 1;
   }
   
   int enable = 1;
   if (setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
   perror("setsockopt(SO_REUSEADDR) failed\n");

   bzero((char *)&serv_addr, sizeof(serv_addr));
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portNum);
   
   if (bind(ListenSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
   {
      perror("ERROR on binding\n");
      return 1;
   }
   
   listen(ListenSocket, 5);
   clilen = sizeof(cli_addr);
   
   return 0;
}

int acceptClient(int *ClientSocket)
{
   *ClientSocket = accept(ListenSocket, (struct sockaddr *)&cli_addr, &clilen);
   
   if (*ClientSocket < 0)
   {
      perror("Client socket not returned\n");
      return 1;
   }
   
   return 0;
}

void endServer(int ClientSocket)
{
   close(ClientSocket);
   shutdown(ListenSocket, SHUT_RD);
   close(ListenSocket);
}
