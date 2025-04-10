#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "resources.h"

bool loadResources(GameResources *pRes)
{
    SDL_Surface *pSurface = IMG_Load("resources/images/Background.png");
    if (!pSurface)
    {
        printf("Failed to load background image: %s\n", IMG_GetError());
        return false;
    }

    pRes->pBackgroundTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pSurface);
    SDL_FreeSurface(pSurface);
    if (!pRes->pBackgroundTexture)
    {
        printf("Failed to create background texture: %s\n", SDL_GetError());
        return false;
    }

    pRes->pBgMusic = Mix_LoadMUS("resources/music/intro_Opening.mp3");
    if (!pRes->pBgMusic)
    {
        printf("Failed to load background music: %s\n", Mix_GetError());
    }
    else
    {
        Mix_PlayMusic(pRes->pBgMusic, -1);
    }

    pRes->pFont = TTF_OpenFont("resources/fonts/PressStart2P-Regular.ttf", 35);
    if (!pRes->pFont)
    {
        printf("Failed to load font: %s\n", TTF_GetError());
        return false;
    }

    SDL_Color green = {0, 255, 0};
    SDL_Color red = {255, 0, 0};

    SDL_Surface *pStartSurf = TTF_RenderText_Solid(pRes->pFont, "Start Game", green);
    SDL_Surface *pExitSurf = TTF_RenderText_Solid(pRes->pFont, "Quit Game", red);

    if (!pStartSurf || !pExitSurf)
    {
        printf("Failed to render text surfaces.\n");
        return false;
    }

    pRes->pStartTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pStartSurf);
    pRes->pExitTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pExitSurf);

    // bredd x höjd
    pRes->startRect = (SDL_Rect){370, 670, pStartSurf->w, pStartSurf->h};
    pRes->exitRect = (SDL_Rect){370, 720, pExitSurf->w, pExitSurf->h};

    SDL_FreeSurface(pStartSurf);
    SDL_FreeSurface(pExitSurf);

    // 💡 Kombinerar båda delar här:
    if (!pRes->pStartTexture || !pRes->pExitTexture)
    {
        printf("Failed to create button textures: %s\n", SDL_GetError());
        return false;
    }

    // 💡 Laddar tileset-bild också:
    SDL_Surface *surface = IMG_Load("resources/images/tileset.png");
    if (!surface)
    {
        printf("Failed to load tileset image: %s\n", IMG_GetError());
        return false;
    }
    pRes->ptilesetTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, surface);
    SDL_FreeSurface(surface); // glöm inte släppa ytan!

    return true;
}
