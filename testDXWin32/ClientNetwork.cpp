#include "pch.h"
#include "ClientNetwork.h"


ClientNetwork::ClientNetwork(void)
{
	// create WSADATA object
	WSADATA wsaData;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		_RPT1(0,"WSAStartup failed with error: %d\n", iResult);
		exit(1);
	}

	// socket
	for (int i = 0; i < MAX_PLAYER; i++) {
		peerSocket[i] = INVALID_SOCKET;
	}
	listenSocket = INVALID_SOCKET;
}

ClientNetwork::~ClientNetwork(void)
{
	WSACleanup();
}

bool ClientNetwork::connectToHost(const std::wstring & ip)
{
	for (int i = 0; i < 16; i++)
		peerIP[HOST_ID][i] = (char) ip.c_str()[i];
	peerPort[HOST_ID] = DEFAULT_PORT;
	try {
		if (!createSocket(HOST_ID)) {
			throw WSAGetLastError();
		}
	}
	catch (int err) {
		throw err;
	}
	return true;
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
			OutputDebugString(L"Connection closed\n");
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

		return true;
	}

	return false;
}

bool ClientNetwork::createSocket(int id)
{
	if (peerSocket[id] != INVALID_SOCKET)
		return true;
	// holds address info for socket to connect to
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

	if (iResult != 0)
	{
		_RPT1(0,"getaddrinfo failed with error: %d\n", iResult);
		throw WSAGetLastError();
		//WSACleanup();
		//exit(1);
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL;ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		peerSocket[id] = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);

		if (peerSocket[id] == INVALID_SOCKET) {
			_RPT1(0,"socket failed with error: %ld\n", WSAGetLastError());
			throw WSAGetLastError();
			//WSACleanup();
			//exit(1);
		}

		// Connect to server.
		iResult = connect(peerSocket[id], ptr->ai_addr, (int)ptr->ai_addrlen);

		if (iResult == SOCKET_ERROR)
		{
			int code = WSAGetLastError();
			closesocket(peerSocket[id]);
			peerSocket[id] = INVALID_SOCKET;
			_RPT1(0,"Client number %d is down... did not connect.\n", id);
		}
		else 
		{
			OutputDebugString(L"Connected to host.\n");

			freeaddrinfo(result);

			// Set the mode of the socket to be nonblocking
			u_long iMode = 1;

			iResult = ioctlsocket(peerSocket[id], FIONBIO, &iMode);
			if (iResult == SOCKET_ERROR)
			{
				_RPT1(0, "ioctlsocket failed with error: %d\n", WSAGetLastError());
				closesocket(peerSocket[id]);
				throw WSAGetLastError();
			}

			//disable nagle
			char value = 1;
			setsockopt(peerSocket[id], IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

			return true;
		}
	}
	// no longer need address info
	freeaddrinfo(result);
	throw WSAGetLastError();
	return false;
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
		_RPT1(0,"getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		exit(1);
	}

	// Create a SOCKET for connecting to server
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET) {
		_RPT1(0,"socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		exit(1);
	}

	// Set the mode of the socket to be nonblocking
	u_long iMode = 1;
	iResult = ioctlsocket(listenSocket, FIONBIO, &iMode);

	if (iResult == SOCKET_ERROR) {
		_RPT1(0,"ioctlsocket failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		exit(1);
	}

	// Setup the TCP listening socket
	iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		_RPT1(0,"bind failed with error: %d\n", WSAGetLastError());
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
		_RPT1(0,"listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		exit(1);
	}
}

void ClientNetwork::Reset()
{
	for (unsigned int i = 0; i < MAX_PLAYER; i++) {
		closesocket(peerSocket[i]);
		peerSocket[i] = INVALID_SOCKET;
	}
	closesocket(listenSocket);
	listenSocket = INVALID_SOCKET;
}
