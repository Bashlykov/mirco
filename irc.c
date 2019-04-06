#include "irc.h"

static volatile boolean running;

void connectToServer(char* host, char* port)
{
    SOCKET socket;
    HANDLE hThread;
    char nickname[STR_LEN];

    socket = createSocket(host, port);

    if (socket)
    {
        printf("Connected to %s!\n", host);

        if (sendUserNick(socket, nickname))
        {
            running = TRUE;
            hThread = (HANDLE)_beginthread(runThread, 0, (void *)socket);
            WaitForSingleObject(hThread, 0);

            while (is_connected() && running)
            {
                if(handlerBuffeFrom(socket, nickname) == 0)
                    return;                                           
            }

            if (is_connected())
                deleteSocket(socket);

            printf( "Disconnected!\n" );
        }
    }
}

void runThread(void *socket)
{
    char message_to[MSG_LEN]; // msg to server

    while(TRUE)
	{
        fgets(message_to, sizeof(message_to), stdin);

        if(!strcmp(message_to, "\n"))
			continue;

        if(strstr(message_to, "quit"))
		{
            sendMessage((SOCKET)socket, message_to);
            running = FALSE;
			break;
		}

        sendMessage((SOCKET)socket, message_to);
        memset(message_to, 0, sizeof(message_to));
	}

	_endthread();
}


int sendMessage(SOCKET socket, char *buffer_to)
{
    if (strlen(buffer_to) == 0)
		return 0;

    strcat(buffer_to, "\n");

    if (send(socket, buffer_to, (int)strlen(buffer_to), 0) == SOCKET_ERROR)
    {
        printf("Error send data: %d\n", WSAGetLastError());
        return 0;
    }

	return 1;
}

int handlerBuffeFrom(SOCKET socket, char *nickname)
{
	int len; // length msg from server
    char buffer_from[BUFF_SIZE]; // msg from server
    memset(buffer_from, 0, BUFF_SIZE);

    if(!is_connected())
		return 0;

    if((len = recv(socket, buffer_from, BUFF_SIZE, 0)) == SOCKET_ERROR)
    {
        printf("Error recv data: %d\n", WSAGetLastError());
        return 0;
    }

    if (strstr(buffer_from, ":Nickname is already in use"))
    {
        setNickName(socket, buffer_from, nickname);
        return 1;
    }

    //for reconnect
    if (strstr(buffer_from, "ERROR :Closing Link"))
    {
        printf("%s", buffer_from);
        return 0;
    }

//TODO change NICK

    if (strstr(buffer_from, "PRIVMSG"))
    {
        privateMsg(buffer_from, nickname);
        return 1;
    }

    if (strstr(buffer_from, "PING"))
    {
        pingPong(socket, buffer_from);
        return 1;
    }

    printf("%s", buffer_from);
	return 1;
}

void setNickName(SOCKET socket, char *buffer_from, char *nickname)
{
    char cmd_nick[MSG_LEN];

    printf("%s", buffer_from);
    printf("Input your nickname: ");

    scanf("%s", nickname);
    sprintf(cmd_nick, "%s %s", NICK, nickname);
    sendMessage(socket, cmd_nick);
    memset(buffer_from, 0, strlen(buffer_from));
}

int sendUserNick( SOCKET socket, char *nickname )
{
    char user[STR_LEN];
    char *msgNickName;

    printf( "Input User Name: "	);
    scanf("%s", user);
	printf( "Input Nick Name: "	);
    scanf("%s", nickname);

	// send user and nick to server
    msgNickName = (char *)malloc(strlen(user) + strlen(nickname) + strlen(NAME_APP) + 24);
    sprintf(msgNickName, "USER %s 8 * :%s\nNICK %s\n", user, NAME_APP, nickname);
    printf("%s", msgNickName);

    if(!sendMessage(socket, msgNickName))
    {
        return 0;
    }

    free (msgNickName);

	return 1;
}

// for stay on a server we play the game ping pong with server
void pingPong(SOCKET socket, char *serverBuffer)
{
    char *pong; // send ping to server
    char *subGetBuf;

    printf("%s", serverBuffer);
    pong = (char *)malloc(strlen(serverBuffer));
    subGetBuf = strchr(serverBuffer, ' ');

    if(subGetBuf != NULL)
    {
        if(strstr(subGetBuf, ":"))
        {
            subGetBuf = strstr(subGetBuf, ":");
            ++subGetBuf;
        }
        sprintf(pong, "%s %s", PONG, subGetBuf);
        printf("%s", pong);
        sendMessage(socket, pong);
    }

    memset(serverBuffer, 0, strlen(serverBuffer));
}

// private message to me
void privateMsg(char *serverBuffer, char *nickname)
{
    char nick_user_host[MSG_LEN];
    char irc_cmd[MSG_LEN];
    char user_to[MSG_LEN];
    char *user_from;
    char *message_from;

    printf("%s", serverBuffer);
    // parse for ':nickname!user@host irc_cmd user_to' without ' :wrod1 word2'
    sscanf(serverBuffer, "%s%s%s", nick_user_host, irc_cmd, user_to);

    if (!strcmp(user_to, nickname))
	{
        //:nickname!username@server -> :nickname
        user_from = strtok(nick_user_host, "!");
        ++user_from;

        //':nickname!user@host irc_cmd user_to :wrod1 word2 ...' -> 'wrod1 word2 ...'
        message_from = strtok(serverBuffer, ":");
        if(message_from != NULL)
            message_from = strtok(NULL, ":");

        printf("[private message]<%s>: %s\n", user_from, message_from);
        memset(serverBuffer, 0, strlen(serverBuffer));
	}   
}
