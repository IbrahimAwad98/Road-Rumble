#ifndef CLIENT_H
#define CLIENT_H

#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include <string.h>
#include "network.h"
#include "resources.h" //för SDL_NET initiering

// funktioner
bool initClient(const char *pIpaddress, int port);
bool sendPlayerData(PlayerData *pData);
bool receiveServerData(PlayerData *pData);
void closeClient(void);

#endif
