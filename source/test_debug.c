#include "test_debug.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

void runAllTests(GameResources *pRes, bool *pTestPassed, bool debugMode)
{
    if (debugMode)
    {
        printf("------Debug mode is active------\n");
    }

    printf("------Test mode is active------\n");
    // hantera renderer och fönsteret
    if (!pRes->pRenderer || !pRes->pWindow)
    {
        printf("SDL Renderer and window is not working.\n");
        pTestPassed = false;
    }
    else
    {
        printf("SDL is working.\n");
    }
    // hantera text
    if (!pRes->pFont)
    {
        printf("SDL font is not working.\n");
        pTestPassed = false;
    }
    else
    {
        printf("SDL font is working.\n");
    }
    // rita resultat
    SDL_Color green = {0, 255, 0};
    SDL_Surface *pSurf = TTF_RenderText_Solid(pRes->pFont, "TEST OK", green);
    if (!pSurf)
    {
        printf("Text-rendering misslyckades.\n");
        *pTestPassed = false;
    }
    else
    {
        SDL_Texture *pTex = SDL_CreateTextureFromSurface(pRes->pRenderer, pSurf);
        SDL_Rect r = {500, 300, pSurf->w, pSurf->h};
        SDL_RenderCopy(pRes->pRenderer, pTex, NULL, &r);
        SDL_RenderPresent(pRes->pRenderer);
        SDL_Delay(3000); // 3 sekunder
        SDL_FreeSurface(pSurf);
        SDL_DestroyTexture(pTex);
        printf("Text-rendering OK.\n");
    }
    // hantera map
    if (!pRes->ptilesetTexture)
    {
        printf("SDL Tileset is not working.\n");
        *pTestPassed = false;
    }
    else
    {
        printf("Tileset  is working.\n");
    }
    // hantera nätverk
    if (SDLNet_Init() != 0)
    {
        printf("SDL_net is not working: %s\n", SDLNet_GetError());
        *pTestPassed = false;
    }
    else
    {
        printf("SDL_net is nworking.\n");
    }

    printf("\n------Tests are done------\n");
}