#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <string.h>

// filer
#include "cleanup.h"
#include "game.h"
#include "sdl_init.h"
#include "resources.h"
#include "test_debug.h"
#include "camera.h"

int main(int argc, char **argv)
{
    // Omdirigera stdout och stderr till en loggfil (för test och felsökning)
    freopen("testlog.txt", "w", stdout); // Öppnar och skriver över fil
    freopen("testlog.txt", "a", stderr); // Append: loggar även fel

    // Struktur som håller alla spelresurser (renderer, texturer, bilar, osv.)
    GameResources res = {0};

    // Flaggor för att avgöra om spelet ska köras i test- eller debugläge
    bool testMode = false, debugMode = false;

    // Kolla igenom argumenten som skickades till programmet
    for (int i = 1; i < argc; i++)
    {
        // Tillåt både stora och små bokstäver, ex: --Test, --DEBUG
        if (strcasecmp(argv[i], "--test") == 0)
        {
            testMode = true;
        }
        if (strcasecmp(argv[i], "--debug") == 0)
        {
            debugMode = true;
        }
    }

    // Initiera SDL, fönster, ljud, nätverk osv.
    if (!initSDL(&res))
    {
        printf("Failed to initialize SDL components.\n");
        return 1; // Felkod 1 = SDL-fel
    }

    // Ladda texturer, bilder, ljud och fonter
    if (!loadResources(&res))
    {
        printf("Failed to load game resources.\n");
        cleanup(&res); // Rensa innan vi avslutar
        return 1;
    }

    // Om testläge är aktiverat, kör tester först
    if (testMode)
    {
        bool testPassed = true;

        // Kör alla testfunktioner, med eller utan debug-läge
        runAllTests(&res, &testPassed, debugMode);

        if (testPassed)
        {
            printf("TEST RESULTAT: PASSED.\n");
            printf("Everything went fine! Test completed without errors.\n");
            gameLoop(&res); // Om testerna gick bra, starta spelet
        }
        else
        {
            printf("TEST RESULTAT: FAILED.\n");
            printf("One or more tests failed. Check the log above for details.\n");
            return 1; // Avsluta om testet misslyckades
        }
    }

    // Om inte testläge, starta spelet direkt
    gameLoop(&res);

    // Rensa alla resurser och avsluta SDL
    cleanup(&res);

    return 0; // Allt gick bra
}
