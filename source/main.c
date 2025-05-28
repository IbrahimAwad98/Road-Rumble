#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include "cleanup.h"
#include "game.h"
#include "sdl_init.h"
#include "resources.h"
#include "test_debug.h"
#include "server.h"
#include "network.h"
#include "client.h"
#include "globals.h"

// Globala nätverksvariabler
char serverIP[IPLENGTH] = "127.0.0.1"; // Standard: localhost
int PlayerID = 0;

int main(int argc, char **argv)
{
    // Logga stdout och stderr till fil
    freopen("testlog.txt", "w", stdout);
    freopen("testlog.txt", "a", stderr);

    GameResources res = {0}; // Allokerar allt spelinnehåll

    bool testMode = false, debugMode = false;

    // Tolka kommandoradsargument
    for (int i = 1; i < argc; i++)
    {
        if (strcasecmp(argv[i], "--test") == 0)
            testMode = true;
        else if (strcasecmp(argv[i], "--debug") == 0)
            debugMode = true;
        else if (strcasecmp(argv[i], "--ip") == 0 && i + 1 < argc)
        {
            strncpy(serverIP, argv[++i], sizeof(serverIP) - 1);
            serverIP[sizeof(serverIP) - 1] = '\0';
        }
        else if (strcasecmp(argv[i], "--id") == 0 && i + 1 < argc)
        {
            PlayerID = atoi(argv[++i]);
            if (PlayerID < 1 || PlayerID > 4)
            {
                printf("Wrong ID. Use --id 1 to 4.\n");
                return true;
            }
            PlayerID--; // Justera till 1-4
        }
    }

    // Initiera SDL och nätverk
    if (!initSDL(&res))
    {
        printf("Failed to initialize SDL components.\n");
        return true;
    }
    if (!initClient(serverIP, SERVER_PORT))
    {
        printf("Failed to connect to server at: %s.\n", serverIP);
        return true;
    }

    // Ladda resurser
    if (!loadResources(&res))
    {
        printf("Failed to load game resources.\n");
        cleanup(&res);
        return true;
    }

    // Om testläge är aktivt, kör tester först
    if (testMode)
    {
        bool testPassed = true;
        runAllTests(&res, &testPassed, debugMode);

        if (testPassed)
        {
            printf("TEST RESULTAT: PASSED.\n");
            gameLoop(&res); // Starta spelet
        }
        else
        {
            printf("TEST RESULTAT: FAILED.\n");
            return true;
        }
    }
    else
    {
        gameLoop(&res); // Starta spelet direkt
    }

    // Rensa resurser innan avslut
    cleanup(&res);
    return false;
}
