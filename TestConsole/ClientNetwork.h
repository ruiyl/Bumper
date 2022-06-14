#pragma once
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
// Networking libraries
#include <winsock2.h>
#include <Windows.h>
#include "NetworkServices.h"
#include <ws2tcpip.h>
#include <stdio.h> 
#include "NetworkData.h"

// size of our buffer
#define DEFAULT_BUFLEN 512
// port to connect sockets through 
#define DEFAULT_PORT 8888
// number of players
#define MAX_PLAYER 8
// host id number
#define HOST_ID 0

class ClientNetwork
{

public:

    // for error checking function calls in Winsock library
    int iResult;

	SOCKET	listenSocket;
	SOCKET	peerSocket[MAX_PLAYER];
	char	peerIP[8][16];
	USHORT	peerPort[8];
    // ctor/dtor
    ClientNetwork(void);
    ~ClientNetwork(void);

	int receivePackets(char *, int id);
	bool acceptNewClient(unsigned int & id);
	void createSocket(int id);
	void setListenPort(int offset);

	bool isHost() const { return m_isHost; }

private:

	bool m_isHost = false;
};

