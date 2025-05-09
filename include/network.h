#ifndef NETWORK_H
#define NETWORK_H

#include <SDL2/SDL_net.h>

#define SERVER_PORT 55000 // dynamisk, privat och ephemeral (2000 lokalt)

// strukten skickar data till server.
typedef struct
{

    int playerID;    // ID
    float x, y;      // position
    float angle;     // vridning
    int actionCode;  // kommando ex: 0 = idle, 1 = move...

    int playerID;     // ID
    float x, y;       // position
    float angle;      // vridning
    int actionCode;   // kommando ex: 0 = idle, 1 = move...
    int isPing;       // 0 vanlig data 1 pingförfrågan
    Uint32 timestamp; // för ping-mätning
} PlayerData;

#endif
