#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "server.h"
#include "network.h"

#define NROFPLAYERS 4   // Max antal spelare
#define TIMEOUT_MS 5000 // Timeout efter 5 sekunder inaktivitet

// Struktur som representerar en spelare
typedef struct
{
    IPaddress address;     // IP och port
    PlayerData data;       // Senaste mottagna spelardata
    bool active;           // Om spelaren är aktiv
    Uint32 lastActiveTime; // Tidpunkt för senaste aktivitet
} Player;

// Jämför två IPaddress-strukturer
bool isSameAddress(IPaddress a, IPaddress b)
{
    return a.host == b.host && a.port == b.port;
}

int main(int argc, char **argv)
{
    // Initiera SDL_net
    if (SDLNet_Init() < 0)
    {
        printf("SDLNet_Init error: %s\n", SDLNet_GetError());
        return 1;
    }

    // Initiera servern
    if (!initServer(SERVER_PORT))
    {
        printf("Failed to start server.\n");
        SDLNet_Quit();
        return 1;
    }

    printf("Server is running on port: %d.\n", SERVER_PORT);

    Player players[NROFPLAYERS] = {0}; // Lista med alla spelare
    PlayerData playerData;             // Temporär buffert för mottagna data
    IPaddress clientAddress;           // Avsändarens adress

    // Oändlig server-loop
    while (true)
    {
        // Vänta på data från klient
        if (server_receivePlayerData(&playerData, &clientAddress))
        {
            // Om det är en pingförfrågan, svara direkt och hoppa vidare
            if (playerData.isPing == 1)
            {
                server_sendPlayerData(&playerData, &clientAddress);
                continue;
            }

            int clientIndex = -1; // Index för spelaren

            // Kolla om klienten redan är registrerad
            for (int i = 0; i < NROFPLAYERS; i++)
            {
                if (players[i].active && isSameAddress(players[i].address, clientAddress))
                {
                    clientIndex = i;
                    break;
                }
            }

            // Om ny spelare, tilldela första lediga plats
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

                // Om servern är full, skicka avvisning
                if (clientIndex == -1)
                {
                    printf("Server full. Rejecting client %u:%u\n", clientAddress.host, clientAddress.port);
                    PlayerData rejectData = {0};
                    rejectData.playerID = -1;
                    server_sendPlayerData(&rejectData, &clientAddress);
                    continue;
                }
            }

            // Uppdatera spelarens senaste data
            if (clientIndex != -1)
            {
                players[clientIndex].data = playerData;
                players[clientIndex].lastActiveTime = SDL_GetTicks();
                players[clientIndex].address = clientAddress;

                // Skicka datan till övriga spelare
                for (int i = 0; i < NROFPLAYERS; i++)
                {
                    if (i != clientIndex && players[i].active)
                    {
                        server_sendPlayerData(&players[clientIndex].data, &players[i].address);
                    }
                }
            }
        }

        // Kontrollera timeouts för alla spelare
        Uint32 now = SDL_GetTicks();
        for (int i = 0; i < NROFPLAYERS; i++)
        {
            if (players[i].active && (now - players[i].lastActiveTime > TIMEOUT_MS))
            {
                printf("Timeout: Player %d disconnected.\n", players[i].data.playerID + 1);
                players[i].active = false;
            }
        }

        // Kolla om alla spelare är anslutna
        bool allConnected = true;
        for (int i = 0; i < NROFPLAYERS; i++)
        {
            if (!players[i].active)
            {
                allConnected = false;
                break;
            }
        }

        // Skicka startsignal när alla är anslutna
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
            printf("All players connected, Start signal sent.\n");
        }

        SDL_Delay(1); // Undvik 100% CPU-belastning
    }

    // Om programmet når hit: stäng server och SDL_net
    closeServer();
    SDLNet_Quit();
    return 0;
}
