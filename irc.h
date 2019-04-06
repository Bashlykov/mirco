
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <process.h>
#include <time.h>
#include "net.h"

#define BUFF_SIZE 16384
#define STR_LEN 512
#define MSG_LEN 128
#define PONG "PONG"
#define NICK "NICK"
#define NAME_APP "mirco IRC client"

void connectToServer(char* host, char* port);
void runThread(void *socket);
void setNickName(SOCKET socket, char *buffer_from, char *nick);
int sendUserNick(SOCKET socket, char *nickname);
int sendMessage(SOCKET socket, char *buffer_to);
int handlerBuffeFrom(SOCKET socket, char *nickname);
void pingPong(SOCKET socket, char *serverBuffer);
void privateMsg(char *serverBuffer, char *nickname);
