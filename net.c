#include "net.h"

static unsigned char connected;

unsigned char is_connected()
{
    return connected;
}

SOCKET createSocket(char *server, char *port)
{
    // Initialise Winsock
    WSADATA wsaData;
    if ( WSAStartup( MAKEWORD(2, 2), &wsaData ) != 0 )
    {
        printf( "WSA Initialization failed!\n" );
        WSACleanup();
        return 0;
    }

    SOCKET sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( sock == INVALID_SOCKET )
    {
        printf( "Socket creation failed: %d\n", WSAGetLastError()  );
        WSACleanup();
        return 0;
    }

    char const on = 1;
    if ( setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) ) == -1)
    {
        printf( "Invalid socket: %d\n", WSAGetLastError() );
        WSACleanup();
        return 0;
    }

    unsigned long sMode = 0;
    if ( ioctlsocket(sock, (long)FIONBIO, &sMode) == SOCKET_ERROR )
    {
        printf( "ioctlsocket failed with error: %d\n", WSAGetLastError() );
        WSACleanup();
        return 0;
    }

    // Resolve IP address for hostname
    struct hostent *host;
    if ( ( host = gethostbyname( server ) ) == NULL )
    {
        printf( "Failed to resolve hostname: %d\n", WSAGetLastError() );
        WSACleanup();
        return 0;
    }

    // Setup our socket address structure
    SOCKADDR_IN sockAddr;
    sockAddr.sin_port = htons( (unsigned short)atoi(port) );
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = *( (unsigned long*)host->h_addr );

    // Attempt to connect to server
    if ( connect(sock, (SOCKADDR*)(&sockAddr), sizeof(sockAddr)) != 0 )
    {
        printf( "Failed to establish connection with server: %d\n", WSAGetLastError() );
        WSACleanup();
        return 0;
    }

    connected = 1;
    return sock;
}


void deleteSocket(SOCKET socket)
{
    shutdown(socket, SD_SEND);	// Shutdown our socket
    closesocket(socket);	// Close our socket entirely
	WSACleanup();	// Cleanup Winsock
	connected = 0;
}
