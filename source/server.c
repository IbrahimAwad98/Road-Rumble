#include "server.h"
#include <string.h>
#include <stdio.h>

// Privata servervariabler
static UDPsocket serverSocket = NULL;
static UDPpacket *pPacket = NULL;

// Initierar servern och öppnar socket
bool initServer(int port)
{
    if (SDLNet_Init() < 0)
    {
        printf("SDLNet_Init error: %s\n", SDLNet_GetError());
        return false;
    }

    serverSocket = SDLNet_UDP_Open(port);
    if (!serverSocket)
    {
        printf("SDLNet_UDP_Open error: %s\n", SDLNet_GetError());
        return false;
    }

    pPacket = SDLNet_AllocPacket(sizeof(PlayerData));
    if (!pPacket)
    {
        printf("SDLNet_AllocPacket error: %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

// Tar emot spelarens data från klient
bool server_receivePlayerData(PlayerData *pData, IPaddress *pClientAddress)
{
    if (SDLNet_UDP_Recv(serverSocket, pPacket))
    {
        memcpy(pData, pPacket->data, sizeof(PlayerData));
        *pClientAddress = pPacket->address; // Spara avsändarens adress
        return true;
    }
    return false;
}

// Skickar spelarens data till en klient
bool server_sendPlayerData(PlayerData *pData, IPaddress *pClientAddress)
{
    memcpy(pPacket->data, pData, sizeof(PlayerData));
    pPacket->len = sizeof(PlayerData);
    pPacket->address = *pClientAddress;
    return SDLNet_UDP_Send(serverSocket, -1, pPacket) > 0;
}

// Stänger server och frigör resurser
void closeServer(void)
{
    if (pPacket)
    {
        SDLNet_FreePacket(pPacket);
    }
    if (serverSocket)
    {
        SDLNet_UDP_Close(serverSocket);
    }
}
