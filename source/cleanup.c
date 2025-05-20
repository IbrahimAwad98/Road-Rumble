#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>
#include "cleanup.h"
#include "car.h"
#include "client.h"
#include "server.h"

void cleanup(GameResources *pRes)
{
    // Rensa bilar
    destroyCar(pRes->pCar1);
    destroyCar(pRes->pCar2);
    destroyCar(pRes->pCar3);
    destroyCar(pRes->pCar4);

    //  Rensa texturer (huvudmeny)
    if (pRes->pStartTexture)
    {
        SDL_DestroyTexture(pRes->pStartTexture);
    }
    if (pRes->pExitTexture)
    {
        SDL_DestroyTexture(pRes->pExitTexture);
    }
    if (pRes->pBackgroundTexture)
    {
        SDL_DestroyTexture(pRes->pBackgroundTexture);
    }
    // Rensa tile-texturer
    for (int i = 0; i < NUM_TILES; i++)
    {
        if (pRes->pTiles[i])
        {
            SDL_DestroyTexture(pRes->pTiles[i]);
            pRes->pTiles[i] = NULL;
        }
    }
    // Rensa font
    if (pRes->pFont)
        TTF_CloseFont(pRes->pFont);

    // Rensa renderer och fönster
    if (pRes->pRenderer)
    {
        SDL_DestroyRenderer(pRes->pRenderer);
    }
    if (pRes->pWindow)
    {
        SDL_DestroyWindow(pRes->pWindow);
    }
    // Rensa musik
    if (pRes->pBgMusic)
    {
        Mix_FreeMusic(pRes->pBgMusic);
    }
    // Stäng nätverk
    closeClient();
    closeServer();

    // Stäng SDL-subsystem
    Mix_CloseAudio();
    SDLNet_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    printf("Cleanup completed!\n");
}
