#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>

// filer
#include "cleanup.h"
#include "game.h"
#include "sdl_init.h"
#include "resources.h"
#include "test_debug.h"

int main(int argc, char **argv)
{

    // skriv ut test och debugg utdata för en fil
    freopen("testlog.txt", "w", stdout);
    freopen("testlog.txt", "a", stderr);

    // Strukturen som håller alla resurser
    GameResources res = {0};
    // två variabel för test-debug hantering
    bool testMode = false, debugMode = false;

    // flaggor
    for (int i = 1; i < argc; i++)
    {
        // hantera båda stor och små bokstav
        if (strcasecmp(argv[i], "--test") == 0)
        {
            testMode = true;
        }
        if (strcasecmp(argv[i], "--debug") == 0)
        {
            debugMode = true;
        }
    }

    // Initiera SDL, fönster, ljud, nätverk
    if (!initSDL(&res))
    {
        printf("Failed to initialize SDL components.\n");
        return 1;
    }

    // Ladda bilder, texturer, musik, fonter
    if (!loadResources(&res))
    {
        printf("Failed to load game resources.\n");
        cleanup(&res);
        return 1;
    }

    if (testMode)
    {
        bool testPassed = true;
        runAllTests(&res, &testPassed, debugMode);
        if (testPassed)
        {
            printf("TEST RESULTAT: PASSED.\n");
            printf("Everything went fine! Test completed without errors.\n");
            return 0;
        }
        else
        {
            printf("TEST RESULTAT: FAILED.\n");
            printf("One or more tests failed. Check the log above for details.\n");
            return 1;
        }
    }

    gameLoop(&res); // Starta huvudloopen för spelet
    cleanup(&res);  // Rensa och avsluta
    return 0;
}