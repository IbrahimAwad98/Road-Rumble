#include <SDL2/SDL_net.h>
#include <stdbool.h>
#include <stdio.h>

// Egna headers
#include "server.h"
#include "network.h"

#define NROFPLAYERS 2   // Max två spelare för denna version
#define TIMEOUT_MS 5000 // 5 sekunder
// Struktur för att hålla varje ansluten spelares data
typedef struct
{
    IPaddress address;     // Klientens IP och port
    PlayerData data;       // Senaste mottagna PlayerData
    bool active;           // Om spelaren är ansluten
    Uint32 lastActiveTime; // senaste data togs emot
} Player;

int main(int argc, char **argv)
{
    // Initiera SDL_net
    if (SDLNet_Init() < 0)
    {
        printf("SDLNet_Init error: %s\n", SDLNet_GetError());
        return true;
    }

    // Öppna en UDP-socket på fördefinierad port
    if (!initServer(SERVER_PORT))
    {
        printf("Failed to start server.\n");
        SDLNet_Quit();
        return true;
    }

    printf("Server is running on port: %d.\n", SERVER_PORT);

    // Initiera spelarlista (max 2 spelare)
    Player players[NROFPLAYERS] = {0};
    PlayerData playerData;
    IPaddress clientAddress;

    // Huvudloop: tar emot data och skickar svar
    while (true)
    {

        // Vänta på data från en klient
        if (server_receivePlayerData(&playerData, &clientAddress))
        {
            int clientIndex = -1;

            // Kolla om klienten redan finns i listan
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

            // Om ny klient – registrera den
            if (clientIndex == -1)
            {
                for (int i = 0; i < NROFPLAYERS; i++)
                {
                    if (!players[i].active)
                    {
                        players[i].address = clientAddress;
                        players[i].active = true;
                        clientIndex = i;
                        printf("New player connected: slot %d\n", i);
                        break;
                    }
                }
                // kontollera om server är upptagen
                if (clientIndex == -1)
                {
                    printf("Server full. Rejecting client %u:%u\n", clientAddress.host, clientAddress.port);

                    PlayerData rejectData = {0};
                    rejectData.playerID = -1;
                    server_sendPlayerData(&rejectData, &clientAddress);
                    continue; // hoppa resten
                }
            }

            // Spara mottagen data och bestäm ID
            if (clientIndex != -1)
            {
                players[clientIndex].data = playerData;
                players[clientIndex].lastActiveTime = SDL_GetTicks();
                players[clientIndex].data.playerID = clientIndex; // servern bestämmer ID
                players[clientIndex].address = clientAddress;

                // Hitta motståndaren (andra spelaren)
                int opponentIndex = (clientIndex == 0) ? 1 : 0;

                if (players[opponentIndex].active)
                {

                    // Skicka motståndarens data till klienten
                    server_sendPlayerData(&players[opponentIndex].data, &clientAddress);
                }
                else
                {
                    // Motståndare inte ansluten – skicka tom data
                    PlayerData empty = {0};
                    server_sendPlayerData(&empty, &clientAddress);
                }
            }
        }
        // kontorllera timeouts
        Uint32 now = SDL_GetTicks();
        for (int i = 0; i < NROFPLAYERS; i++)
        {
            if (players[i].active && (now - players[i].lastActiveTime > TIMEOUT_MS))
            {
                printf("Timeout: Player %d disconnected.\n", i);
                players[i].active = false;
            }
        }

        // Vänta lite för att spara CPU
        SDL_Delay(1);
    }

    // om while loopen bryts då detta gäller.
    closeServer();
    SDLNet_Quit();
    return false;
}
