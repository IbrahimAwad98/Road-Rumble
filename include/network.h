#ifndef NETWORK_H
#define NETWORK_H

#include <SDL2/SDL_net.h>

#define SERVER_PORT 55000 // Serverport för nätverk (UDP)

typedef struct
{
    int playerID;
    float x, y;
    float angle;
    float speed;
    int actionCode;
    int isPing;
    Uint32 timestamp;
    int isBoosting;
    int isDrifting;
    int isStartSignal;
} PlayerData;

#endif
