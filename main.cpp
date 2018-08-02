/*************[ mirco IRC client ]*********
*
*	Written By Bashlykov Sergey
*	Copyright (c) 2013 Bashlykov Sergey
*	All rights reserved.
*
*******************************************
*
*  It's simple console IRC client
*  for use protocol as is.
*  For connecting, use like this:
*  'mirco.exe [server] [port]'
*  Other command like protocol IRC command.
*
*******************************************/

#include "irc.h"

int main( int argc, char **argv )
{
	//  CONTROL COUNT OF COMMAND LINE ARGUMENTS
	if (argc > 3 || argc < 3)
	{
		printf( "Use like this: [server] [port]\n" );
		return 0;
	}
	else
	{
		setHost( *++argv );
		setPort( *++argv );
	}

	HANDLE hThread;
	char* serverBuffer;

	if ( createSocket() )
	{
		printf( "Connecting with server...\n" );

		if ( connectServer( getHost(), atoi(getPort()) ) )
		{
			printf( "Connected!\n" );

			if ( sendNick() )
			{
				printf( "Logged!\n" );
				set_running(1);
				hThread = (HANDLE)_beginthread( runThread, 0, NULL );
				WaitForSingleObject( hThread, 0 );

				while ( is_connected() && is_running() )
				{
					getText();
					serverBuffer = getServerBuffer();
					printf( "%s", serverBuffer );
					pingPong( serverBuffer );
					privateMsg( serverBuffer );
					memset( serverBuffer, 0, SIZE_BUFF );
				}

				if ( is_connected() )
					deleteSocket();

				printf( "Disconnected!\n" );
			}
		}
	}

	return 0;
}
