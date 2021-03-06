#pragma once
#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "NetworkData.h"

class ClientGame
{
public:
	ClientGame(void);
	~ClientGame(void);

	ClientNetwork* network;
	int id;
	static unsigned int clientNumber;

    char network_data[MAX_PACKET_SIZE];

    void update();
	void receivePacket();

private:
	bool test = false;
};

