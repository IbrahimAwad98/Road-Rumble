#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include "test_debug.h"

// Kör tester för spelets resurser och SDL-komponenter
void runAllTests(GameResources *pRes, bool *pTestPassed, bool debugMode)
{
    // Statusrapport-header
    printf("===============================\n");
    printf("         STATUSRAPPORT         \n");
    printf("===============================\n");

    if (debugMode)
        printf("Debug mode is active\n");

    printf("Test mode is active\n");
    printf("===============================\n");

    *pTestPassed = true;

    //  Visa "Running tests..." på skärmen
    SDL_Color white = {255, 255, 255};
    SDL_Surface *pLoadingSurf = TTF_RenderText_Solid(pRes->pFont, "Running tests...", white);
    SDL_Rect rRunning;

    if (pLoadingSurf)
    {
        SDL_Texture *pLoadingTex = SDL_CreateTextureFromSurface(pRes->pRenderer, pLoadingSurf);
        rRunning = (SDL_Rect){
            .w = pLoadingSurf->w,
            .h = pLoadingSurf->h,
            .x = (WIDTH - pLoadingSurf->w) / 2,
            .y = (HEIGHT - pLoadingSurf->h) / 2};

        SDL_RenderClear(pRes->pRenderer);
        SDL_RenderCopy(pRes->pRenderer, pLoadingTex, NULL, &rRunning);
        SDL_RenderPresent(pRes->pRenderer);
        SDL_Delay(1000);

        SDL_FreeSurface(pLoadingSurf);
        SDL_DestroyTexture(pLoadingTex);
    }

    // Utför tester
    if (!pRes->pRenderer || !pRes->pWindow)
    {
        printf("SDL Renderer and window is not working.\n");
        *pTestPassed = false;
    }
    else
    {
        printf("SDL is working.\n");
    }

    if (!pRes->pFont)
    {
        printf("SDL font is not working.\n");
        *pTestPassed = false;
    }
    else
    {
        printf("SDL font is working.\n");
    }
    if (SDLNet_Init() != 0)
    {
        printf("SDL_net is not working: %s\n", SDLNet_GetError());
        *pTestPassed = false;
    }
    else
    {
        printf("SDL_net is working.\n");
    }

    // Visa testresultat
    SDL_Color green = {0, 255, 0};
    SDL_Color red = {255, 0, 0};
    SDL_Color resultColor = *pTestPassed ? green : red;
    const char *pResultText = *pTestPassed ? "TEST OK" : "TEST FAILED";

    SDL_Surface *pResultSurf = TTF_RenderText_Solid(pRes->pFont, pResultText, resultColor);
    if (pResultSurf)
    {
        SDL_Texture *pResultTex = SDL_CreateTextureFromSurface(pRes->pRenderer, pResultSurf);
        SDL_Rect rTest = {
            .w = pResultSurf->w,
            .h = pResultSurf->h,
            .x = (WIDTH - pResultSurf->w) / 2,
            .y = rRunning.y + rRunning.h + 20};

        SDL_RenderCopy(pRes->pRenderer, pResultTex, NULL, &rTest);
        SDL_RenderPresent(pRes->pRenderer);
        SDL_Delay(3000);

        SDL_FreeSurface(pResultSurf);
        SDL_DestroyTexture(pResultTex);
    }

    // Avslutning
    printf("\nTests are done\n");
    printf("===============================\n");
}
