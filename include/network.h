#ifndef NETWORK_H
#define NETWORK_H

#include <SDL2/SDL_net.h>

#define SERVER_PORT 2000 // dynamisk port (vi måste testa)
#define MAX_CLIENTS 4

// strukten skickar data till server.
typedef struct
{
    int playerID;   // ID
    float x, y;     // position
    float angle;    // vridning
    int actionCode; // kommando ex: 0 = idle, 1 = move...
} PlayerData;

#endif
