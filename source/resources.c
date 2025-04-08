#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "resources.h"

bool loadResources(GameResources *pRes)
{
    // Ladda bakgrundsbild
    SDL_Surface *pSurface = IMG_Load("resources/Sim2.png");
    if (!pSurface)
        return false;

    // Skapa textur från ytan
    pRes->pBackgroundTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pSurface);
    SDL_FreeSurface(pSurface); // Frigör ytan efter konvertering
    if (!pRes->pBackgroundTexture)
        return false;

    // Ladda bakgrundsmusik
    pRes->pBgMusic = Mix_LoadMUS("resources/intro_Opening.mp3");
    if (pRes->pBgMusic)
        Mix_PlayMusic(pRes->pBgMusic, -1); // Loopa oändligt

    // Ladda font
    pRes->pFont = TTF_OpenFont("resources/PressStart2P-Regular.ttf", 35);
    if (!pRes->pFont)
        return false;

    // Färger
    SDL_Color green = {0, 255, 0};
    SDL_Color red = {255, 0, 0};

    // Skapa textytor
    SDL_Surface *pStartSurf = TTF_RenderText_Solid(pRes->pFont, "Start Game", green);
    SDL_Surface *pExitSurf = TTF_RenderText_Solid(pRes->pFont, "Quit Game", red);

    // Skapa texturer från ytorna
    pRes->pStartTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pStartSurf);
    pRes->pExitTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pExitSurf);

    // Sätt positioner för knapparna
    pRes->startRect = (SDL_Rect){120, 450, pStartSurf->w, pStartSurf->h};
    pRes->exitRect = (SDL_Rect){120, 500, pExitSurf->w, pExitSurf->h};

    // Rensa ytor
    SDL_FreeSurface(pStartSurf);
    SDL_FreeSurface(pExitSurf);

    return pRes->pStartTexture && pRes->pExitTexture;
}
