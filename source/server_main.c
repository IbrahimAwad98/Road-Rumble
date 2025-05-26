#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "server.h"
#include "network.h"

#define NROFPLAYERS 4   // Max antal spelare
#define TIMEOUT_MS 5000 // Timeout efter 5 sekunder

// Spelarstruktur
typedef struct
{
    IPaddress address;
    PlayerData data;
    bool active;
    Uint32 lastActiveTime;
} Player;

int main(int argc, char **argv)
{
    // Initiera nätverksbibliotek
    if (SDLNet_Init() < 0)
    {
        printf("SDLNet_Init error: %s\n", SDLNet_GetError());
        return 1;
    }

    // Starta server
    if (!initServer(SERVER_PORT))
    {
        printf("Failed to start server.\n");
        SDLNet_Quit();
        return 1;
    }

    printf("Server is running on port: %d.\n", SERVER_PORT);

    Player players[NROFPLAYERS] = {0};
    PlayerData playerData;
    IPaddress clientAddress;

    // Serverloop
    while (true)
    {
        // Mottagning av data från klient
        if (server_receivePlayerData(&playerData, &clientAddress))
        {
            // Hantera ping-paket
            if (playerData.isPing == 1)
            {
                server_sendPlayerData(&playerData, &clientAddress);
                continue;
            }

            int clientIndex = -1;

            // Leta efter befintlig klient
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

            // Ny klient -> tilldela plats
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

                // Om full server -> avvisa
                if (clientIndex == -1)
                {
                    printf("Server full. Rejecting client %u:%u\n", clientAddress.host, clientAddress.port);
                    PlayerData rejectData = {0};
                    rejectData.playerID = -1;
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

                // Skicka till alla andra spelare
                for (int i = 0; i < NROFPLAYERS; i++)
                {
                    if (i != clientIndex && players[i].active)
                    {
                        server_sendPlayerData(&players[clientIndex].data, &players[i].address);
                    }
                }
            }
        }

        // Timeout-hantering
        Uint32 now = SDL_GetTicks();
        for (int i = 0; i < NROFPLAYERS; i++)
        {
            if (players[i].active && (now - players[i].lastActiveTime > TIMEOUT_MS))
            {
                printf("Timeout: Player %d disconnected.\n", players[i].data.playerID + 1);
                players[i].active = false;
            }
        }
        bool allConnected = true;
        for (int i = 0; i < NROFPLAYERS; i++)
        {
            if (!players[i].active)
            {
                allConnected = false;
                break;
            }
        }

        static bool startSignalSent = false;
        if (allConnected && !startSignalSent)
        {
            PlayerData startSignal = {0};
            startSignal.isStartSignal = 1;
            for (int i = 0; i < NROFPLAYERS; i++)
            {
                server_sendPlayerData(&startSignal, &players[i].address);
            }
            startSignalSent = true;
            printf(" All players connected — Start signal sent.\n");
        }

        SDL_Delay(1); // Minska CPU-belastning
    }

    // Rensa resurser om loopen bryts
    closeServer();
    SDLNet_Quit();
    return 0;
}
