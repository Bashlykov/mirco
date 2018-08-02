
#include <conio.h>
#include <string.h>
#include <process.h>
#include <time.h>
#include "net.h"

#define SIZE_BUFF 10000
#define LEN_STR 80

void runThread(void *);
void signalHandler(int);
int sendNick();
int sendText( char *);
int getText( void );
void pingPong(char *);
void privateMsg( char * );
char* getUser();
char* getNick();
char* getHost();
char* getPort();
void setHost(const char* p);
void setPort(const char* p);
char* getServerBuffer();
void setServerBuffer(const char* b);
unsigned int is_running();
void set_running(unsigned int flag);
