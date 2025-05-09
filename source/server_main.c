#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
// filer
#include "server.h"
#include "network.h"

#define NROFPLAYERS 4   // Max 4 spelare
#define TIMEOUT_MS 5000 // Timeout efter 5 sekunder

// Struktur för varje ansluten spelare
typedef struct
{
    IPaddress address;     // Spelarens IP och port
    PlayerData data;       // Senast mottagna PlayerData
    bool active;           // Är spelaren aktiv/ansluten?
    Uint32 lastActiveTime; // När senast tog emot data
} Player;

int main(int argc, char **argv)
{
    // Initiera SDL_net
    if (SDLNet_Init() < 0)
    {
        printf("SDLNet_Init error: %s\n", SDLNet_GetError());
        return 1;
    }

    // Öppna en UDP-socket
    if (!initServer(SERVER_PORT))
    {
        printf("Failed to start server.\n");
        SDLNet_Quit();
        return 1;
    }

    printf("Server is running on port: %d.\n", SERVER_PORT);

    Player players[NROFPLAYERS] = {0}; // Nollställ alla spelare

    PlayerData playerData;
    IPaddress clientAddress;

    // Huvudloop
    while (true)
    {
        // Vänta på data från klient
        if (server_receivePlayerData(&playerData, &clientAddress))
        {
            if (playerData.isPing == 1)
            {
                server_sendPlayerData(&playerData, &clientAddress);
                continue;
            }

            int clientIndex = -1;

            // Finn om klienten redan finns
            for (int i = 0; i < NROFPLAYERS; i++)
            {
                if (players[i].active &&
                    players[i].address.host == clientAddress.host &&
                    players[i].address.port == clientAddress.port)
                {
                    clientIndex = i;
                    break;
                }
            }

            // Om ny klient, registrera
            if (clientIndex == -1)
            {
                for (int i = 0; i < NROFPLAYERS; i++)
                {
                    if (!players[i].active)
                    {
                        players[i].address = clientAddress;
                        players[i].active = true;
                        clientIndex = i;
                        printf("New player connected: slot %d\n", playerData.playerID + 1);
                        break;
                    }
                }

                // Server full
                if (clientIndex == -1)
                {
                    printf("Server full. Rejecting client %u:%u\n", clientAddress.host, clientAddress.port);
                    PlayerData rejectData = {0};
                    rejectData.playerID = -1; // signalerar avvisad
                    server_sendPlayerData(&rejectData, &clientAddress);
                    continue;
                }
            }

            // Uppdatera spelarens data
            if (clientIndex != -1)
            {
                players[clientIndex].data = playerData;
                players[clientIndex].lastActiveTime = SDL_GetTicks();
                players[clientIndex].address = clientAddress;

                // Skicka den NYA datan till alla andra spelare
                for (int i = 0; i < NROFPLAYERS; i++)
                {
                    if (i != clientIndex && players[i].active)
                    {
                        server_sendPlayerData(&players[clientIndex].data, &players[i].address);
                    }
                }
            }
        }
        // Kolla timeouts
        Uint32 now = SDL_GetTicks();
        for (int i = 0; i < NROFPLAYERS; i++)
        {
            if (players[i].active && (now - players[i].lastActiveTime > TIMEOUT_MS))
            {
                printf("Timeout: Player %d disconnected.\n", players[i].data.playerID + 1);
                players[i].active = false;
            }
        }

        SDL_Delay(1); // Spara CPU
    }

    // Om loop bryts
    closeServer();
    SDLNet_Quit();
    return 0;
}