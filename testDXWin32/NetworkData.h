#pragma once
#include <string>

#define MAX_PACKET_SIZE 1000000

enum PacketTypes {

	INIT_CONNECTION = 0,

	READY = 1,

	ADDRESS_LIST = 2,

	ASSIGN_ID = 3,

	UPDATE_STATE = 4,

	GAME_START = 5

};

struct Packet {

    unsigned int packet_type;

	int id;

	char	peerIP[8][16];
	USHORT	peerPort[8];

	float position[3];
	float rotation[3];
	float velocity[3];
	float acceleration[3];

	bool isDestroyed;

    void serialize(char * data) {
        memcpy(data, this, sizeof(Packet));
    }

    void deserialize(char * data) {
        memcpy(this, data, sizeof(Packet));
    }
};