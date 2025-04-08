#include <stdio.h>
#include "game.h"
#include "sdl_init.h"
#include "resources.h"
#include "cleanup.h"

int main(int argc, char **argv)
{
    // Strukturen som håller alla resurser
    GameResources res = {0};

    // Initiera SDL, fönster, ljud, nätverk
    if (!initSDL(&res))
    {
        printf("Kunde inte initiera SDL-komponenter.\n");
        return 1;
    }

    // Ladda bilder, texturer, musik, fonter
    if (!loadResources(&res))
    {
        printf("Kunde inte ladda spelets resurser.\n");
        cleanup(&res);
        return 1;
    }

    // Starta huvudloopen för spelet
    gameLoop(&res);

    // Rensa och avsluta
    cleanup(&res);
    return 0;
}
