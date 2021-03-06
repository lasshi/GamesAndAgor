#pragma once
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdarg.h>


#define BUFFERSIZE 1500
#define MAX_NICK 12
#define SCREEN_X 800
#define SCREEN_Y 600
#define OBJ_SIZE 40
#define PLA_SIZE 100
#define GROWTH_FACTOR 5


/*
char buffer[1000];
memset($buffer,0,1000);

int pID = 1;
int gametime = 12;
int msgtype = 6;
int plLength = 0;
int position = 0;

*(uint16_t*)&buffer[position] = htons(pID);
position += sizeof(uint16_t);

*(uint32_t*)&buffer[position] = htonl(gametime);
position += sizeof(uint32_t);

*(uint8_t*)&buffer[position] = msgtype;
position += sizeof(uint8_t);

*(uint32_t*)&buffer[position] = htonl(plLength);
position += sizeof(uint32_t);
*/

/*
NAMING INFO:
Static objects are referred to as 'objects' and 'particle' can be either a
'player' or a 'object'
*/

// List for particles that are near
typedef struct Near {
	void *pParticle;  // typecasted to either *Player or *Object
	struct Near *pNext;
} Near;

// Struct for Players
typedef struct Player {
	uint16_t ID;
	char nick[12];
	uint32_t lastPacket; // player's game time (game time of last movement packet)
	uint32_t lastServerTime;
	uint16_t location[2];  // | X ¦ Y |
	uint16_t direction[2];  // | X ¦ Y |
	uint32_t scale;
	uint32_t points;
	int state;  // | alive | eaten | exited |
	uint32_t ping;

	// For storing players and objects nearby
	Near *nearPlayers;
	Near *nearObjects;

	// For storing player's IP address //
	struct sockaddr_storage addressStorage;
	struct sockaddr address;
	/* NOTE: this pointer must point to addressStorage. It can't be done here,
	 * it must be typecasted in a new player function:
	 * address = (struct sockaddr*)&addressStorage; */

	struct Player *pNext;
} Player;

// Struct for Objects
typedef struct Object {
	uint32_t ID;
	uint16_t location[2];
	struct Object *pNext;
} Object;

typedef struct Ack{
	uint32_t packetID;
	uint32_t gameTimeSent;
	uint16_t msgLength;
	uint16_t toPlayerID;
	char msg[BUFFERSIZE];
	struct Ack *pNext;
} Ack;

typedef struct Game {
	uint32_t gameTime;
	uint16_t nPlayers;
	uint32_t nObjects;
	Player *sPlayers;
	Object *sObjects;
	Ack *sAcks; // List of messages that haven't been acknowledged
	uint32_t packetID; // Increasing integer value to serialize ack packets
	uint32_t pingID; // Increasing integer value to serialize ping packets
} Game;

/* Message types */

enum MESSAGE_TYPE {
    GAME_MESSAGE = 0,
    ACK = 1,
    PLAYER_MOVEMENT = 2,
    PLAYER_CHAT_MESSAGE = 3,
    STATISTICS_MESSAGE = 4

};

/* Message subtypes*/
enum GAME_MESSAGE_TYPE {
    JOIN = 0,
    NICK = 1,
    EXIT = 2,
    RESTART = 3,
    GAME_END = 4,
    GAME_UPDATE = 5,
    POINTS = 6,
    PLAYER_DEAD = 7,
    PLAYER_OUT = 8,
	PING = 9
};

enum PLAYER_STATUS {
		JOINING = 0,
		ALIVE = 1,
		EATEN = 2,
		DEAD = 3,
		OUT = 4
};

/*Struct for game msg/incoming packet */
struct Packet{
  int error;
  uint8_t subType;
  uint16_t posX, posY;
  uint16_t dirX, dirY;
  uint8_t ACKTYPE;
  char nick[MAX_NICK];
  uint16_t pingID;
  uint16_t ping;

  uint32_t gameTime; /* also known as ACK_PACKET_ID xD */
	uint32_t ackID;
  uint16_t ID;
  uint8_t msgType;
	uint32_t payloadLength;
	struct sockaddr senderAddr;

};

/* GAME_UPDATE MSGS */
// enum gameUpdate { };
