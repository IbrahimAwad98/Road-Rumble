#ifndef NETWORK_H
#define NETWORK_H

#include <SDL2/SDL_net.h>

#define SERVER_PORT 3000 // dynamisk port (vi måste testa)
#define MAX_CLIENTS 4
#define MAX_SERVERS 5

// strukten skickar data till server.
typedef struct
{
    int playerID;   // ID
    float x, y;     // position
    int actionCode; // kommando ex: 0 = idle, 1 = move...
} PlayerData;

#endif
