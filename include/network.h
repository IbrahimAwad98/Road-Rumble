#ifndef NETWORK_H
#define NETWORK_H

#include <SDL2/SDL_net.h>

#define SERVER_PORT 55000 // Serverport för nätverk (UDP)

typedef struct
{
    int playerID;      // ID för spelaren (0–3)
    float x, y;        // Spelarens position i världen
    float angle;       // Spelarens rotation (i grader)
    float speed;       // Spelarens hastighet
    int actionCode;    // Kod för åtgärder (t.ex. 0 = idle, 1 = rörelse)
    int isPing;        // 1 om detta är en pingförfrågan, annars 0
    Uint32 timestamp;  // Tidsstämpel för att mäta ping
    int isBoosting;    // 1 om spelaren använder boost, annars 0
    int isDrifting;    // 1 om spelaren driftar, annars 0
    int isStartSignal; // 1 om servern signalerar spelstart
} PlayerData;

#endif
