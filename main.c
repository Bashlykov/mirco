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

    char *host = *++argv;
    char *port = *++argv;

    connectToServer(host, port);

	return 0;
}
