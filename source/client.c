#include "client.h"
#include <string.h>

// socket och paket ska vara privata globala (lättare anrop)
static UDPsocket sock = NULL;     // för att skicka data
static UDPpacket *pPacket = NULL; // paketet med data
static IPaddress serverAddress;   // Ip address och port

bool initClient(const char *pIpaddress, int port)
{
    // hantera error
    if (SDLNet_ResolveHost(&serverAddress, pIpaddress, port) < 0)
    {
        printf("Resolve error: %s\n", SDLNet_GetError());
        return false;
    }
    sock = SDLNet_UDP_Open(0); // valfri port
    if (!sock)
    {
        printf("UDP open error: %s\n", SDLNet_GetError());
        return false;
    }
    pPacket = SDLNet_AllocPacket(512); // storlek 512 bytes
    if (!pPacket)
    {
        printf("SDL_AllocPacket error: %s\n", SDLNet_GetError());
        return false;
    }
    // allt gick bra
    return true;
}
// sätter paketes destinationsaddress;
bool client_sendPlayerData(PlayerData *pData)
{
    memcpy(pPacket->data, pData, sizeof(PlayerData)); // kopierar strukten till pekare
    pPacket->len = sizeof(PlayerData);                // antal bytes
    pPacket->address = serverAddress;                 // destination
    return SDLNet_UDP_Send(sock, -1, pPacket) > 0;    // via UDP
}
bool client_receiveServerData(PlayerData *pData)
{
    if (SDLNet_UDP_Recv(sock, pPacket))
    {
        memcpy(pData, pPacket->data, sizeof(PlayerData)); // kopiera data
        return true;
    }
    return false; // om det inte gick
}
void closeClient()
{
    // stänger och rensar
    SDLNet_FreePacket(pPacket);
    SDLNet_UDP_Close(sock);
}