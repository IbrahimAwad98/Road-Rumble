#ifndef SERVER_H
#define SERVER_H

#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include "network.h"

bool server_receivePlayerData(PlayerData *pData, IPaddress *pClientAddress);
bool server_sendPlayerData(PlayerData *pData, IPaddress *pClientAddress);
bool initServer(int port);
void closeServer(void);

#endif
