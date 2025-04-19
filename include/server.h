#ifndef SERVER_H
#define SERVER_H

#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include "network.h"

// Båda använder pekare till IPaddress (för att modifiera och återanvända)
bool server_receivePlayerData(PlayerData *pData, IPaddress *pClientAddress);
bool server_sendPlayerData(PlayerData *pData, IPaddress *pClientAddress);

// Init och stäng
bool initServer(int port);
void closeServer(void);

#endif
