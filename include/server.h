#ifndef SERVER_H
#define SERVER_H

#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include "network.h"

// funktioner
bool initServer(int port);
bool receiveServerData(PlayerData *pData, IPaddress clientAddress);
bool sendPlayerData(PlayerData *pData, IPaddress *pClientAddress);
void closetServer(void);

#endif
