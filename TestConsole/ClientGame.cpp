#include "StdAfx.h"
#include "ClientGame.h"

unsigned int ClientGame::clientNumber = 1;

ClientGame::ClientGame(void)
{

    network = new ClientNetwork();
	if (network->isHost()) {
		id = HOST_ID;
		return;
	}
	else
		id = -1;

    // send init packet
    const unsigned int packet_size = sizeof(Packet);
    char packet_data[packet_size];

    Packet packet;
    packet.packet_type = INIT_CONNECTION;
	packet.id = id;

    packet.serialize(packet_data);

    int result = NetworkServices::sendMessage(network->peerSocket[HOST_ID], packet_data, packet_size);
}


ClientGame::~ClientGame(void)
{
}

void ClientGame::update()
{
	// get new clients if hosting a game
	if (network->isHost() && network->acceptNewClient(clientNumber))
	{
		printf("client %d has been connected to the host\n", clientNumber);
		clientNumber++;
	}
	if (!network->isHost()) {
		network->acceptNewClient(clientNumber);
		//printf("client %d receive connection\n", id);
	}
	receivePacket();
	if (!test && network->isHost() && clientNumber > 3) {
		test = true;
		Packet packet;
		const unsigned int packet_size = sizeof(Packet);
		char packet_data[packet_size];
		packet.packet_type = ADDRESS_LIST;
		packet.id = clientNumber;
		for (int i = 0; i < clientNumber; i++) {
			packet.peerPort[i] = network->peerPort[i];
			for (int j = 0; j < 16; j++)
				packet.peerIP[i][j] = network->peerIP[i][j];
		}
		packet.serialize(packet_data);
		for (int i = 0; i < clientNumber; i++) {
			printf("client %d has address %s and port %u\n", i, packet.peerIP[i], packet.peerPort[i]);
		}
		for (int i = 0; i < clientNumber; i++) {
			NetworkServices::sendMessage(network->peerSocket[i], packet_data, packet_size);
		}
	}
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
		if (data_length <= 0)
		{
			//no data recieved
			continue;

		}

		unsigned int j = 0;
		while (j < (unsigned int)data_length)
		{
			packet.deserialize(network_data);
			j += sizeof(Packet);
			printf("client %d receive packet type = %d\n", id, packet.packet_type);
			switch (packet.packet_type) {

			case INIT_CONNECTION:
				if (network->isHost()) {
					if (packet.id == -1) {
						Packet p;
						const unsigned int packet_size = sizeof(Packet);
						char packet_data[packet_size];
						p.packet_type = ASSIGN_ID;
						p.id = i;
						p.serialize(packet_data);
						NetworkServices::sendMessage(network->peerSocket[i], packet_data, packet_size);
					}
					else {
						network->peerPort[i] = packet.peerPort[i];
					}
				}
				else {
					//do the thing
				}
				break;

			case ASSIGN_ID:
				if (!network->isHost()) {
					id = packet.id;
					printf("This is client number %d\n", id);
					network->setListenPort(id);
					//sent listen addr and port back to the host
					Packet p;
					const unsigned int packet_size = sizeof(Packet);
					char packet_data[packet_size];
					p.packet_type = INIT_CONNECTION;
					p.id = id;
					SOCKADDR_IN sin;
					int len = sizeof(sin);
					getsockname(network->listenSocket, (sockaddr*)&sin, &len);
					p.peerPort[id] = htons(sin.sin_port);
					p.serialize(packet_data);
					NetworkServices::sendMessage(network->peerSocket[HOST_ID], packet_data, packet_size);
				}
				break;

			case ADDRESS_LIST:
				if (!network->isHost() && id != -1) {
					for (int i = 1; i < MAX_PLAYER; i++) {
						if (i == id || i == HOST_ID)
							continue;
						clientNumber = packet.id;
						network->peerPort[i] = packet.peerPort[i];
						for (int j = 0; j < 16; j++)
							network->peerIP[i][j] = packet.peerIP[i][j];
					}
					//if any socket in peer list is invalid, connect to a listen socket of that peer using peerInfo
					for (int i = 0; i < clientNumber; i++) {
						if (i == HOST_ID || i == id)
							continue;
						network->createSocket(i);
					}
				}
				break;

			case UPDATE_STATE:

				break;

			default:

				printf("error in packet types\n");

				break;
			}
		}
	}
}
