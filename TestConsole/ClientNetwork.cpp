#include "..\testDXWin32\ClientNetwork.h"
#include "StdAfx.h"
#include "ClientNetwork.h"


ClientNetwork::ClientNetwork(void)
{
    // create WSADATA object
    WSADATA wsaData;

    // socket
	for (int i = 0; i < MAX_PLAYER; i++) {
		peerSocket[i] = INVALID_SOCKET;
	}
	listenSocket = INVALID_SOCKET;

    // holds address info for socket to connect to
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        exit(1);
    }



    // set address info
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;  //TCP connection!!!

	std::string port = std::to_string(DEFAULT_PORT);
    //resolve server address and port 
	iResult = getaddrinfo("127.0.0.1", port.c_str(), &hints, &result);

    if( iResult != 0 ) 
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        exit(1);
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
		peerSocket[HOST_ID] = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);

        if (peerSocket[HOST_ID] == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            exit(1);
        }

        // Connect to server.
        iResult = connect( peerSocket[HOST_ID], ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(peerSocket[HOST_ID]);
            peerSocket[0] = INVALID_SOCKET;
            printf ("The server is down... did not connect.\n");
		}
		else
			printf("Connected to host.\n");
    }



    // no longer need address info for server
    freeaddrinfo(result);



    // if connection failed
    if (peerSocket[HOST_ID] == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
		printf("Now hosting the game.\n");
		setListenPort(HOST_ID);
		m_isHost = true;
		return;
    }

	// Set the mode of the socket to be nonblocking
    u_long iMode = 1;

    iResult = ioctlsocket(peerSocket[HOST_ID], FIONBIO, &iMode);
    if (iResult == SOCKET_ERROR)
    {
        printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
        closesocket(peerSocket[HOST_ID]);
        WSACleanup();
        exit(1);        
    }

	//disable nagle
    char value = 1;
    setsockopt( peerSocket[HOST_ID], IPPROTO_TCP, TCP_NODELAY, &value, sizeof( value ) );
}


ClientNetwork::~ClientNetwork(void)
{
}

int ClientNetwork::receivePackets(char * recvbuf, int id) 
{
	int packetLength = 0;
	if (peerSocket[id] != INVALID_SOCKET) {
		iResult = NetworkServices::receiveMessage(peerSocket[id], recvbuf, MAX_PACKET_SIZE);
		if (iResult > 0) {
			packetLength = iResult;
		}
		else if (iResult == 0)
		{
			printf("Connection closed\n");
			closesocket(peerSocket[id]);
			WSACleanup();
			exit(1);
		}
	}
    return packetLength;
}

bool ClientNetwork::acceptNewClient(unsigned int & id)
{
	// if client waiting, accept the connection and save the socket
	peerSocket[id] = accept(listenSocket, NULL, NULL);

	if (peerSocket[id] != INVALID_SOCKET)
	{
		//disable nagle on the client's socket
		char value = 1;
		setsockopt(peerSocket[id], IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));
		SOCKADDR_IN sin;
		int len = sizeof(sin);
		getsockname(peerSocket[id], (sockaddr*)&sin, &len);
		inet_ntop(AF_INET, &sin.sin_addr, peerIP[id], 16);
		return true;
	}

	return false;
}

void ClientNetwork::createSocket(int id)
{
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	// set address info
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;  //TCP connection!!!

	std::string port = std::to_string(peerPort[id]);
									  //resolve server address and port 
	iResult = getaddrinfo(peerIP[id], port.c_str(), &hints, &result);
	printf("connecting to %s port %u on client %d\n", peerIP[id], peerPort[id], id);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		exit(1);
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		peerSocket[id] = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);

		if (peerSocket[id] == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			exit(1);
		}

		// Connect to server.
		iResult = connect(peerSocket[id], ptr->ai_addr, (int)ptr->ai_addrlen);

		if (iResult == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			closesocket(peerSocket[id]);
			peerSocket[0] = INVALID_SOCKET;
			printf("The client is down... did not connect.\n");
		}
		else
			printf("Connected to client.\n");
	}



	// no longer need address info for server
	freeaddrinfo(result);
}

void ClientNetwork::setListenPort(int offset)
{
	// address info for the server to listen to
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	// set address information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;    // TCP connection!!!
	hints.ai_flags = AI_PASSIVE;

	std::string port = std::to_string(DEFAULT_PORT + offset);
	// Resolve the server address and port
	iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		exit(1);
	}

	// Create a SOCKET for connecting to server
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		exit(1);
	}

	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;
	iResult = ioctlsocket(listenSocket, FIONBIO, &iMode);

	if (iResult == SOCKET_ERROR) {
		printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		exit(1);
	}

	// Setup the TCP listening socket
	iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		exit(1);
	}

	// no longer need address information
	freeaddrinfo(result);

	// start listening for new clients attempting to connect
	iResult = listen(listenSocket, SOMAXCONN);

	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		exit(1);
	}
	SOCKADDR_IN sin;
	int len = sizeof(sin);
	getsockname(listenSocket, (sockaddr*)&sin, &len);
	printf("listening on port %u\n", ntohs(sin.sin_port));
}
