#pragma once

#define UPDATE_INTERVAL 0.1f

#include <winsock2.h>
#include <Windows.h>
#include "ClientNetwork.h"
#include "NetworkData.h"
#include "Utility.h"

class ClientGame
{
public:
	ClientGame(void);
	~ClientGame(void);

	ClientNetwork* network;
	int id;

	static unsigned int clientNumber;
	static unsigned int receivedClientCount;
	static unsigned int	readyCount;
	static bool			isPlaying;
	static bool			isWaiting;

    char					network_data[MAX_PACKET_SIZE];

	Utility::EntityState	State[MAX_PLAYER];
	float					StateAge[MAX_PLAYER];
	bool					isUpdated[MAX_PLAYER];

	void HostGame();
	void JoinGame(const std::wstring& ip);
    void update(float elapsedTime);
	void ready();
	void lock();

	void saveState(const Utility::EntityState& state);
	void sendState();
	
	void Reset();

private:
	void receivePacket();
};

