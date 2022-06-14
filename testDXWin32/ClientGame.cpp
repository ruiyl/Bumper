#include "pch.h"
#include "ClientGame.h"

unsigned int ClientGame::clientNumber = 1;
unsigned int ClientGame::receivedClientCount = 1;
unsigned int ClientGame::readyCount = 1;
bool ClientGame::isPlaying = false;
bool ClientGame::isWaiting = true;

ClientGame::ClientGame(void)
{
	network = new ClientNetwork();

	for (int i = 0; i < MAX_PLAYER; i++)
	{
		StateAge[i] = 0.0f;
		isUpdated[i] = false;
	}
}


ClientGame::~ClientGame(void)
{
}

void ClientGame::HostGame()
{
	network->setListenPort(HOST_ID);

	id = HOST_ID;
}

void ClientGame::JoinGame(const std::wstring & ip)
{
	try {
		(network->connectToHost(ip));
	}
	catch (int err) {
		throw "Cannot Connected";
	}
	id = -1;
	clientNumber++;

	// send init packet
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	Packet packet;
	packet.packet_type = INIT_CONNECTION;
	packet.id = id;
	packet.serialize(packet_data);

	NetworkServices::sendMessage(network->peerSocket[HOST_ID], packet_data, packet_size);
}

void ClientGame::update(float elapsedTime)
{
	// get new clients if hosting a game
	if (!isPlaying) {
		if (isWaiting)
		{
			if (network->acceptNewClient(clientNumber)) {
				_RPT1(0, "client %d has been connected\n", clientNumber);
				clientNumber++;
				if (id == HOST_ID) {
					SOCKADDR_IN sin;
					int len = sizeof(sin);
					getsockname(network->peerSocket[clientNumber - 1], (sockaddr*)&sin, &len);
					inet_ntop(AF_INET, &sin.sin_addr, network->peerIP[clientNumber - 1], 16);
				}
				else {
					_RPT1(0, "connected peer : %d\n", clientNumber);
				}
			}
			if (!id == HOST_ID && clientNumber == receivedClientCount)
				isWaiting = false;
		}
		if (id == HOST_ID && !isWaiting && !isPlaying) {
			_RPT1(0, "ready player : %d\n", readyCount);
			if (readyCount == clientNumber) {
				const unsigned int packet_size = sizeof(Packet);
				char packet_data[packet_size];

				Packet packet;
				packet.packet_type = GAME_START;
				packet.id = id;
				packet.serialize(packet_data);

				for (unsigned int i = 1; i < clientNumber; i++)
					NetworkServices::sendMessage(network->peerSocket[i], packet_data, packet_size);

				isPlaying = true;
			}
		}
	}
	else {
		//play
		for (unsigned int i = 0; i < MAX_PLAYER; i++)
			StateAge[i] += elapsedTime;
	}
	receivePacket();
}

void ClientGame::ready()
{
	if (!id == HOST_ID) {
		Packet packet;
		const unsigned int packet_size = sizeof(Packet);
		char packet_data[packet_size];
		packet.packet_type = READY;
		packet.id = id;
		packet.serialize(packet_data);
		NetworkServices::sendMessage(network->peerSocket[HOST_ID], packet_data, packet_size);
	}
}

void ClientGame::lock()
{
	if (id == HOST_ID) {
		Packet packet;
		const unsigned int packet_size = sizeof(Packet);
		char packet_data[packet_size];
		packet.packet_type = ADDRESS_LIST;
		packet.id = clientNumber;
		for (unsigned int i = 1; i < clientNumber; i++) {
			packet.peerPort[i] = network->peerPort[i];
			for (int j = 0; j < 16; j++)
				packet.peerIP[i][j] = network->peerIP[i][j];
		}
		packet.serialize(packet_data);
		for (unsigned int i = 1; i < clientNumber; i++) {
			NetworkServices::sendMessage(network->peerSocket[i], packet_data, packet_size);
		}
		isWaiting = false;
	}
}

void ClientGame::saveState(const Utility::EntityState & state)
{
	for (int i = 0; i < 3; i++) {
		State[id].acceleration[i] = state.acceleration[i];
		State[id].velocity[i] = state.velocity[i];
		State[id].position[i] = state.position[i];
		State[id].rotation[i] = state.rotation[i];
	}
	State[id].isDestroyed = state.isDestroyed;
}

void ClientGame::sendState()
{
	Utility::EntityState state = State[id];
	Packet packet;
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];
	packet.packet_type = UPDATE_STATE;
	packet.id = id;
	for (int i = 0; i < 3; i++) {
		packet.acceleration[i] = state.acceleration[i];
		packet.velocity[i] = state.velocity[i];
		packet.position[i] = state.position[i];
		packet.rotation[i] = state.rotation[i];
	}
	packet.isDestroyed = state.isDestroyed;
	packet.serialize(packet_data);
	for (unsigned int i = 0; i < clientNumber; i++) {
		if (i == id)
			continue;
		NetworkServices::sendMessage(network->peerSocket[i], packet_data, packet_size);
	}
}

void ClientGame::Reset()
{
	for (int i = 0; i < MAX_PLAYER; i++)
	{
		StateAge[i] = 0.0f;
		isUpdated[i] = false;
	}
	isPlaying = false;
	isWaiting = true;
	readyCount = 1;
	receivedClientCount = 1;
	clientNumber = 1;
	id = -1;
	network->Reset();
}

void ClientGame::receivePacket()
{
	Packet packet;
	const unsigned int packet_size = sizeof(Packet);
	char packet_data[packet_size];

	for (unsigned int i = 0; i < clientNumber; i++) {
		if (i == id)
			continue;
		int data_length = network->receivePackets(network_data, i);
		if (data_length <= 0) {
			//no data recieved
			continue;

		}

		unsigned int j = 0;
		while (j < (unsigned int)data_length)
		{
			packet.deserialize(network_data);
			j += sizeof(Packet);
			//_RPT1(0,"client %d receive packet type = %d\n", id, packet.packet_type);
			switch (packet.packet_type) {

			case INIT_CONNECTION:
				if (id == HOST_ID) {
					if (packet.id == -1) {
						packet.packet_type = ASSIGN_ID;
						packet.id = i;
						packet.serialize(packet_data);
						NetworkServices::sendMessage(network->peerSocket[i], packet_data, packet_size);
					}
					else {
						network->peerPort[packet.id] = packet.peerPort[packet.id];
					}
				}
				else {
					if (i != packet.id) {
						SOCKET temp = network->peerSocket[i];
						network->peerSocket[i] = network->peerSocket[packet.id];
						network->peerSocket[packet.id] = temp;
					}
				}
				break;

			case ASSIGN_ID:
				if (!id == HOST_ID) {
					id = packet.id;
					network->setListenPort(id);
					//_RPT1(0,"This is client number %d\n", id);

					SOCKADDR_IN sin;
					int len = sizeof(sin);
					getsockname(network->listenSocket, (sockaddr*)&sin, &len);
					network->peerPort[id] = ntohs(sin.sin_port);

					packet.packet_type = INIT_CONNECTION;
					packet.id = id;
					packet.peerPort[id] = network->peerPort[id];
					packet.serialize(packet_data);

					NetworkServices::sendMessage(network->peerSocket[HOST_ID], packet_data, packet_size);
				}
				break;

			case ADDRESS_LIST:
				if (!id == HOST_ID) {
					receivedClientCount = packet.id;
					for (unsigned int k = 1; k < receivedClientCount; k++) {
						if (k == id || k == HOST_ID)
							continue;
						network->peerPort[k] = packet.peerPort[k];
						for (int l = 0; l < 16; l++)
							network->peerIP[k][l] = packet.peerIP[k][l];
						
						if (network->createSocket(k)) {

							packet.packet_type = INIT_CONNECTION;
							packet.id = id;
							packet.serialize(packet_data);

							NetworkServices::sendMessage(network->peerSocket[k], packet_data, packet_size);
						}
					}
				}
				break;

			case UPDATE_STATE:
				isUpdated[packet.id] = true;
				for (int k = 0; k < 3; k++) {
					State[packet.id].acceleration[k] = packet.acceleration[k];
					State[packet.id].velocity[k] = packet.velocity[k];
					State[packet.id].position[k] = packet.position[k];
					State[packet.id].rotation[k] = packet.rotation[k];
				}
				StateAge[packet.id] = 0.0f;
				State[packet.id].isDestroyed = packet.isDestroyed;
				break;

			case GAME_START:
				isPlaying = true;
				OutputDebugString(L"start game\n");
				break;

			case READY:
				if (id == HOST_ID)
					readyCount++;
				OutputDebugString(L"ready receive\n");
				break;

			default:

				OutputDebugString(L"error in packet types\n");

				break;
			}
		}
	}
}
