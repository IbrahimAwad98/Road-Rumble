#ifndef SERVER_H
#define SERVER_H

#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include "network.h"

// funktioner
bool initServer(int port);
bool server_receivePlayerData(PlayerData *pData, IPaddress clientAddress);
bool server_sendPlayerData(PlayerData *pData, IPaddress *pClientAddress);
void closeServer(void);

#endif
