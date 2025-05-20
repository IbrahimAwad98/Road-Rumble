#ifndef SERVER_H
#define SERVER_H

#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include "network.h"

// Initierar servern på angiven port
bool initServer(int port);

// Tar emot data från en klient
bool server_receivePlayerData(PlayerData *pData, IPaddress *pClientAddress);

// Skickar data till en klient
bool server_sendPlayerData(PlayerData *pData, IPaddress *pClientAddress);

// Stänger server och frigör resurser
void closeServer(void);

#endif
