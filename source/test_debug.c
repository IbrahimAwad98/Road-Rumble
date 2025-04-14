#include "test_debug.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

void runAllTests(GameResources *pRes, bool *pTestPassed, bool debugMode)
{
    // header för filen
    printf("===============================\n");
    printf("         STATUSRAPPORT         \n");
    printf("===============================\n");

    if (debugMode)
    {
        printf("Debug mode is active\n");
    }

    //------Testläge som gäller------
    printf("Test mode is active\n");
    printf("===============================\n");
    *pTestPassed = true;
    // visa mode på skärm
    SDL_Color white = {255, 255, 255};
    SDL_Surface *pLoadingSurf = TTF_RenderText_Solid(pRes->pFont, "Running tests...", white);
    SDL_Rect rRunning; // måste vara här för att hantera test ok texten
    if (pLoadingSurf)
    {
        SDL_Texture *pLoadingTex = SDL_CreateTextureFromSurface(pRes->pRenderer, pLoadingSurf);

        // positionera texten i mitten av fönsteret
        rRunning = (SDL_Rect){
            .w = pLoadingSurf->w,
            .h = pLoadingSurf->h,
            .x = (WIDTH - pLoadingSurf->w) / 2,
            .y = (HEIGHT - pLoadingSurf->h) / 2};

        SDL_RenderClear(pRes->pRenderer);
        SDL_RenderCopy(pRes->pRenderer, pLoadingTex, NULL, &rRunning);
        SDL_RenderPresent(pRes->pRenderer);
        SDL_Delay(1000); // visa en sekund

        // Städa efter
        SDL_FreeSurface(pLoadingSurf);
        SDL_DestroyTexture(pLoadingTex);
    }

    //------Här är tester------
    // hantera renderer och fönsteret
    if (!pRes->pRenderer || !pRes->pWindow)
    {
        printf("SDL Renderer and window is not working.\n");
        *pTestPassed = false;
    }
    else
    {
        printf("SDL is working.\n");
    }
    // hantera text
    if (!pRes->pFont)
    {
        printf("SDL font is not working.\n");
        *pTestPassed = false;
    }
    else
    {
        printf("SDL font is working.\n");
    }
    // hantera map
    if (!pRes->ptilesetTexture)
    {
        printf("SDL Tileset is not working.\n");
        *pTestPassed = false;
    }
    else
    {
        printf("Tileset is working.\n");
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

    // simulering till test failed.
    //*pTestPassed = false;

    // rita resultat (korrekt = grön & fel = röd)
    SDL_Color green = {0, 255, 0};
    SDL_Color red = {255, 0, 0};
    SDL_Color resultColor = green;
    const char *pResultText = "TEST OK";

    if (!*pTestPassed)
    {
        resultColor = red;
        pResultText = "TEST FAILED";
    }
    // med rätt färg
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
        SDL_Delay(3000); // 3 sekunder

        // städa
        SDL_FreeSurface(pResultSurf);
        SDL_DestroyTexture(pResultTex);
    }

    // testlog.txt
    printf("\nTests are done\n");
    printf("===============================\n");
}