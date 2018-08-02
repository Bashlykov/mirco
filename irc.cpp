#include "irc.h"

char 	user[LEN_STR],
		nick[LEN_STR],
		host[LEN_STR],
		port[6],
		serverBuffer[SIZE_BUFF]; // msg from server

volatile unsigned int running;

unsigned int is_running()
{
    return running;
}

void set_running(unsigned int flag)
{
    running = flag;
}

char* getUser()
{
    return user;
}

char* getNick()
{
    return nick;
}

char* getHost()
{
    return host;
}

char* getPort()
{
    return port;
}

void setHost(const char* h)
{
    strcpy(host, h);
}

void setPort(const char* p)
{
    strcpy(port, p);
}

char* getServerBuffer()
{
    return serverBuffer;
}

void setServerBuffer(const char* b)
{
    strcpy(serverBuffer, b);
}

void runThread(void *)
{
	char sendMsg[80]; // msg to server

	while ( 1 )
	{
		gets( sendMsg );

		if ( !strcmp( sendMsg, "\n" ) )
			continue;

		if ( strstr( sendMsg, "quit" ) )
		{
			sendText( sendMsg );
			running = 0;
			break;
		}

		sendText( sendMsg );
		memset( sendMsg, 0, sizeof(sendMsg) );
	}

	_endthread();
}


int sendText(char * sendBuffer)
{
	if ( strlen( sendBuffer ) < 0 )
		return 0;

	strcat( sendBuffer, "\n" );

	if ( send( get_socket(), sendBuffer, strlen( sendBuffer ), 0) == SOCKET_ERROR )
	{
		printf( "Error send data: %d\n", SOCKET_ERROR );
		return 0;
	}

	return 1;
}


int getText( void )
{
	int len; // length msg from server

	if ( !is_connected() )
		return 0;

	if ( ( len = recv( get_socket(), serverBuffer, sizeof(serverBuffer), 0 ) ) == SOCKET_ERROR )
	{
		printf( "Error recv data: %d\n", SOCKET_ERROR );
		return 0;
	}

	return 1;
}


int sendNick( void )
{
	printf( "Input User Name: "	);
	scanf("%s", user);
	printf( "Input Nick Name: "	);
	scanf("%s", nick);
	char clientMsg[] = "mirco IRC client";

	// send user and nick to server
	char *msgNickName = ( char * ) malloc ( strlen(user) + strlen(nick) + strlen(clientMsg) + 24 );
	sprintf( msgNickName, "USER %s 8 * :%s\nNICK %s\n", user, clientMsg, nick);
	sendText( msgNickName );
	printf( "%s", msgNickName );
	free ( msgNickName );

	return 1;
}


void pingPong( char *serverBuffer )
{
	char *sendPing; // send ping to server
	char pPong[] = "PONG ";
	int lenPong = strlen ( pPong );
	char usnd[60], srvcmd[10], snick[20]; // vars for parser msg from server

	sscanf( serverBuffer, "%s%s%s", usnd, srvcmd, snick ); // parse for: userfrom@ip servercmd userto :

	// for stay on a server we play the game ping pong with server  ))
	if ( !strncmp ( "PING", serverBuffer, 4 ) )
	{
		//printf( "%s", getBuffer );
		sendPing = ( char * ) malloc ( strlen(serverBuffer) + lenPong );
		char *subGetBuf = strchr ( serverBuffer,  ':' );
		sprintf( sendPing, "%s%s", pPong, subGetBuf );
		printf( "%s", sendPing );
		sendText( sendPing );
		free ( sendPing );
	}
}


void privateMsg( char *serverBuffer )
{
	char usnd[60], srvcmd[10], snick[20]; // vars for parser msg from server
	sscanf( serverBuffer, "%s%s%s", usnd, srvcmd, snick ); // parse for ':nickname!username@server servercmd userto :'

	// private message to me
	if ( !strcmp(srvcmd, "PRIVMSG") && !strcmp(snick, nick) )
	{
		/*
			read data before char '!'
			IN => ':nickname!username@server'
			OUT => 'nickname'
		*/
		unsigned char i = 1, j = 0;
		char *privUser = ( char * ) malloc( strlen( usnd ) );

		while ( usnd[i] != '!' )
			privUser[j++] = usnd[i++];

		/*
			read data after second char ':'
			IN => ':nickname!username@server servercmd userto : command param ...'
            OUT => 'command param' (this subsrtring is write to pcmd)
		*/
		char *pCmd = ( char *) malloc( strlen(serverBuffer) );
		pCmd = serverBuffer;

		while ( pCmd[1] != ':' )
		{
			pCmd++;
		}
		pCmd+=2;

		printf( "private message <%s> %s\n", privUser, pCmd );
	}
}
