
#include <winsock2.h>
#include <stdio.h>

int createSocket();
void deleteSocket();
int connectServer( char *, short );
int get_socket();
unsigned char is_connected();
