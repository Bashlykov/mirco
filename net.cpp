#include "net.h"

int Socket;
unsigned char connected;

int get_socket()
{
    return Socket;
}

unsigned char is_connected()
{
    return connected;
}

int createSocket()
{
	// Initialise Winsock
	WSADATA wsaData;
	if ( WSAStartup( MAKEWORD(2, 2), &wsaData ) != 0 )
	{
		printf( "WSA Initialization failed!\n" );
		WSACleanup();
		return 0;
	}

	Socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( Socket == INVALID_SOCKET )
	{
		printf( "Socket creation failed.\n" );
		WSACleanup();
		return 0;
	}

    char const on = 1;
    if ( setsockopt( Socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) == -1)
    {
        printf( "Invalid socket.\n" );
        WSACleanup();
        return 0;
    }

	unsigned long sMode = 0;
	if ( ioctlsocket(Socket, FIONBIO, &sMode) == SOCKET_ERROR )
	{
		printf( "ioctlsocket failed with error: %d\n", SOCKET_ERROR );
		WSACleanup();
		return 0;
	}

	return 1;
}


int connectServer( char * serverHost, short serverPort )
{
	// Resolve IP address for hostname
	struct hostent *host;
	if ( ( host = gethostbyname( serverHost ) ) == NULL )
	{
		printf( "Failed to resolve hostname.\n" );
		WSACleanup();
		return 0;
	}

	// Setup our socket address structure
	sockaddr_in sockAddr;
	sockAddr.sin_port = htons( serverPort );
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = *( (unsigned long*)host->h_addr );

	// Attempt to connect to server
	if ( connect(Socket, (sockaddr*)(&sockAddr), sizeof(sockAddr)) != 0 )
	{
		printf( "Failed to establish connection with server\n" );
		WSACleanup();
		return 0;
	}

	connected = 1;
	return 1;
}


void deleteSocket()
{
	shutdown(Socket, SD_SEND);	// Shutdown our socket
	closesocket(Socket);	// Close our socket entirely
	WSACleanup();	// Cleanup Winsock
	connected = 0;
}
