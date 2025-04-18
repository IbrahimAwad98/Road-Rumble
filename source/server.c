#include "server.h"
#include <string.h>
#include <stdio.h>

// sammas om klient
static UDPsocket serverSocket = NULL;
static UDPpacket *pPacket = NULL;

bool initServer(int port)
{
    // hantera felet
    if (SDLNet_Init() < 0)
    {
        printf("SDLNet_Init error: %s\n", SDLNet_GetError());
        return false;
    }
    serverSocket = SDLNet_UDP_Open(port); // test 2000
    if (!serverSocket)
    {
        printf("SDLNet_UDP_Open error: %s\n", SDLNet_GetError());
        return false;
    }
    pPacket = SDLNet_AllocPacket(512);
    if (!pPacket)
    {
        printf("SDLNet_Allocpacket error: %s\n", SDLNet_GetError());
        return false;
    }
    return true;
}
bool server_receivePlayerData(PlayerData *pData, IPaddress *pClientAddress)
{
    if (SDLNet_UDP_Recv(serverSocket, pPacket))
    {
        memcpy(pData, pPacket->data, sizeof(PlayerData));
        (*pClientAddress) = pPacket->address; // spara address ip och port
        return true;
    }
    return false;
}
bool server_sendPlayerData(PlayerData *pData, IPaddress clientAddress)
{
    memcpy(pPacket->data, pData, sizeof(PlayerData));
    pPacket->len = sizeof(PlayerData);
    pPacket->address = clientAddress;
    return SDLNet_UDP_Send(serverSocket, -1, pPacket) > 0;
}
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
