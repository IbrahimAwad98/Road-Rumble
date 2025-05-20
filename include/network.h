#ifndef NETWORK_H
#define NETWORK_H

#include <SDL2/SDL_net.h>

#define SERVER_PORT 55000 // Serverport för nätverk (UDP)

// Struktur som skickas mellan klient och server
typedef struct
{
    int playerID;     // Spelarens ID (1–4)
    float x, y;       // Position
    float angle;      // Vinkel (rotation)
    int actionCode;   // Kommando (t.ex. 0 = idle, 1 = move)
    int isPing;       // 1 = pingförfrågan, 0 = vanlig data
    Uint32 timestamp; // Används för ping-mätning
} PlayerData;

#endif
