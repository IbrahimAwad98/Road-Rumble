#include "client.h"
#include <string.h>

// Privata globala variabler
static UDPsocket sock = NULL;     // Klientens socket
static UDPpacket *pPacket = NULL; // Paketbuffert
static IPaddress serverAddress;   // Serverns IP och port

// Initierar klient och ansluter till server
bool initClient(const char *pIpaddress, int port)
{
    if (SDLNet_ResolveHost(&serverAddress, pIpaddress, port) < 0)
    {
        printf("Resolve error: %s\n", SDLNet_GetError());
        return false;
    }

    sock = SDLNet_UDP_Open(0); // Använd valfri lokal port
    if (!sock)
    {
        printf("UDP open error: %s\n", SDLNet_GetError());
        return false;
    }

    pPacket = SDLNet_AllocPacket(sizeof(PlayerData)); // Allokera paket (512 byte)
    if (!pPacket)
    {
        printf("SDL_AllocPacket error: %s\n", SDLNet_GetError());
        return false;
    }

    return true;
}

// Skickar spelarens data till servern
bool client_sendPlayerData(PlayerData *pData)
{
    pData->timestamp = SDL_GetTicks();                // Lägg till aktuell tid
    memcpy(pPacket->data, pData, sizeof(PlayerData)); // Kopiera in i paket
    pPacket->len = sizeof(PlayerData);                // Sätt storlek
    pPacket->address = serverAddress;                 // Destination
    return SDLNet_UDP_Send(sock, -1, pPacket) > 0;    // Skicka
}

// Tar emot data från servern
bool client_receiveServerData(PlayerData *pData)
{
    if (SDLNet_UDP_Recv(sock, pPacket))
    {
        memcpy(pData, pPacket->data, sizeof(PlayerData)); // Kopiera in oavsett data storlek
        return true;
    }
    return false;
}

// Stänger klient och frigör resurser
void closeClient()
{
    if (pPacket) SDLNet_FreePacket(pPacket);
    if (sock) SDLNet_UDP_Close(sock);
}
