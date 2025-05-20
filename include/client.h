#ifndef CLIENT_H
#define CLIENT_H

#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include <string.h>
#include "network.h"
#include "resources.h"

// Initierar klient och ansluter till server
bool initClient(const char *pIpaddress, int port);

// Skickar spelarens data till servern
bool client_sendPlayerData(PlayerData *pData);

// Tar emot data från servern
bool client_receiveServerData(PlayerData *pData);

// Stänger anslutning och frigör resurser
void closeClient(void);

#endif
