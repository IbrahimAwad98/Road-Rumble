#include "test_debug.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

// Kör alla tester för spelets resurser och SDL-komponenter
void runAllTests(GameResources *pRes, bool *pTestPassed, bool debugMode)
{
    // Skriver ut en header för testrapporten
    printf("===============================\n");
    printf("         STATUSRAPPORT         \n");
    printf("===============================\n");

    // Visar om debug-läge är aktiverat
    if (debugMode)
    {
        printf("Debug mode is active\n");
    }

    // Aktiverar testläge
    printf("Test mode is active\n");
    printf("===============================\n");

    // Sätter teststatus som godkänd tills något visar motsatsen
    *pTestPassed = true;

    // Skapa en vit textfärg för visning
    SDL_Color white = {255, 255, 255};

    // Rendera texten "Running tests..." till en SDL_Surface
    SDL_Surface *pLoadingSurf = TTF_RenderText_Solid(pRes->pFont, "Running tests...", white);
    SDL_Rect rRunning; // Rektangel som definierar var texten visas

    if (pLoadingSurf)
    {
        // Skapa textur från ytan och centrera den i fönstret
        SDL_Texture *pLoadingTex = SDL_CreateTextureFromSurface(pRes->pRenderer, pLoadingSurf);
        rRunning = (SDL_Rect){
            .w = pLoadingSurf->w,
            .h = pLoadingSurf->h,
            .x = (WIDTH - pLoadingSurf->w) / 2,
            .y = (HEIGHT - pLoadingSurf->h) / 2};

        SDL_RenderClear(pRes->pRenderer);
        SDL_RenderCopy(pRes->pRenderer, pLoadingTex, NULL, &rRunning);
        SDL_RenderPresent(pRes->pRenderer);
        SDL_Delay(1000); // Visa i 1 sekund

        // Frigör resurser
        SDL_FreeSurface(pLoadingSurf);
        SDL_DestroyTexture(pLoadingTex);
    }

    // === Början på faktiska tester ===

    // Testar att renderer och fönster finns
    if (!pRes->pRenderer || !pRes->pWindow)
    {
        printf("SDL Renderer and window is not working.\n");
        *pTestPassed = false;
    }
    else
    {
        printf("SDL is working.\n");
    }

    // Testar att fonten laddats korrekt
    if (!pRes->pFont)
    {
        printf("SDL font is not working.\n");
        *pTestPassed = false;
    }
    else
    {
        printf("SDL font is working.\n");
    }

    // Testar att tileset-texturen laddats
    if (!pRes->ptilesetTexture)
    {
        printf("SDL Tileset is not working.\n");
        *pTestPassed = false;
    }
    else
    {
        printf("Tileset is working.\n");
    }

    // Testar om SDL_net är initierad korrekt
    if (SDLNet_Init() != 0)
    {
        printf("SDL_net is not working: %s\n", SDLNet_GetError());
        *pTestPassed = false;
    }
    else
    {
        printf("SDL_net is working.\n");
    }

    // === Resultatvisning ===

    SDL_Color green = {0, 255, 0};
    SDL_Color red = {255, 0, 0};
    SDL_Color resultColor = green;
    const char *pResultText = "TEST OK";

    // Om något test fallerade, visa röd text
    if (!*pTestPassed)
    {
        resultColor = red;
        pResultText = "TEST FAILED";
    }

    // Rendera resultattexten
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
        SDL_Delay(3000); // Visa resultat i 3 sekunder

        // Rensa resurser
        SDL_FreeSurface(pResultSurf);
        SDL_DestroyTexture(pResultTex);
    }

    // Avsluta rapport
    printf("\nTests are done\n");
    printf("===============================\n");
}
