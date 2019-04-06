#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

SOCKET createSocket(char *server, char *port);
void deleteSocket(SOCKET socket);
int connectServer(SOCKET socket, char *, char * );
unsigned char is_connected(void);
