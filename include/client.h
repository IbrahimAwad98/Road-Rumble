#ifndef CLIENT_H
#define CLIENT_H

#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include <string.h>
#include "network.h"
#include "resources.h"

bool initClient(const char *pIpaddress, int port);
bool client_sendPlayerData(PlayerData *pData);
bool client_receiveServerData(PlayerData *pData);
void closeClient(void);

#endif
