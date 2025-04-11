#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "game.h"
#include "sdl_init.h"
#include "resources.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include "cleanup.h"

int main(int argc, char **argv)
{
    // Strukturen som håller alla resurser
    GameResources res = {0};

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

    printf("Resources loaded successfully!\n");


    gameLoop(&res); // Starta huvudloopen för spelet
    cleanup(&res);  // Rensa och avsluta
    return 0;
}