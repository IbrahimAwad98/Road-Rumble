#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "resources.h"

bool loadResources(GameResources *pRes)
{
    // ladda bakgrund.
    SDL_Surface *pSurface = IMG_Load("resources/images/Background.png");
    if (!pSurface)
    {
        printf("Failed to load background image: %s\n", IMG_GetError());
        return false;
    }
    // omvandla till texture och hantera fel.
    pRes->pBackgroundTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pSurface);
    SDL_FreeSurface(pSurface);
    if (!pRes->pBackgroundTexture)
    {
        printf("Failed to create background texture: %s\n", SDL_GetError());
        return false;
    }

    // ladda musiken och hantera fel.
    pRes->pBgMusic = Mix_LoadMUS("resources/music/intro_Opening.mp3");
    if (!pRes->pBgMusic)
    {
        printf("Failed to load background music: %s\n", Mix_GetError());
    }
    else
    {
        Mix_PlayMusic(pRes->pBgMusic, -1);
    }

    // ladda upp texten t.ex. meny och hantera fel.
    pRes->pFont = TTF_OpenFont("resources/fonts/PressStart2P-Regular.ttf", 35);
    if (!pRes->pFont)
    {
        printf("Failed to load font: %s\n", TTF_GetError());
        return false;
    }

    // desgina texten (färgen)
    SDL_Color green = {0, 255, 0};
    SDL_Color red = {255, 0, 0};
    SDL_Surface *pStartSurf = TTF_RenderText_Solid(pRes->pFont, "Start Game", green);
    SDL_Surface *pExitSurf = TTF_RenderText_Solid(pRes->pFont, "Quit Game", red);

    // hantera fel
    if (!pStartSurf || !pExitSurf)
    {
        printf("Failed to render text surfaces: %s\n", SDL_GetError());
        return false;
    }

    // omvandla dem till texture
    pRes->pStartTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pStartSurf);
    pRes->pExitTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pExitSurf);

    // bredd x höjd
    pRes->startRect = (SDL_Rect){370, 670, pStartSurf->w, pStartSurf->h};
    pRes->exitRect = (SDL_Rect){370, 720, pExitSurf->w, pExitSurf->h};

    // bygga dem
    SDL_FreeSurface(pStartSurf);
    SDL_FreeSurface(pExitSurf);

    // 💡 Kombinerar båda delar här:
    if (!pRes->pStartTexture || !pRes->pExitTexture)
    {
        printf("Failed to create button textures: %s\n", SDL_GetError());
        return false;
    }

    // 🔁 Ladda in asfalt-tiles (road_asphalt01.png → road_asphalt90.png)
    char path[100];
    for (int i = 0; i < NUM_ASPHALT_TILES; i++) {
        snprintf(path, sizeof(path), "resources/bana/asfalt/road_asphalt%02d.png", i + 1);  // laddar 01 → 90
        SDL_Surface *surface = IMG_Load(path);
        if (!surface) {
            printf("Failed to load tile %d: %s\n", i + 1, IMG_GetError());
            return false;
        }

        pRes->pTiles[i] = SDL_CreateTextureFromSurface(pRes->pRenderer, surface);
        SDL_FreeSurface(surface);

        if (!pRes->pTiles[i]) {
            printf("Failed to create texture for tile %d: %s\n", i + 1, SDL_GetError());
            return false;
        }
    }

    // 🧩 Ladda in hela tileset.png till ptilesetTexture
    SDL_Surface *tilesetSurface = IMG_Load("resources/images/tileset.png");
    if (!tilesetSurface)
    {
        printf("Failed to load tileset image: %s\n", IMG_GetError());
        return false;
    }

    pRes->ptilesetTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, tilesetSurface);
    SDL_FreeSurface(tilesetSurface);

    if (!pRes->ptilesetTexture) {
        printf("Failed to create tileset texture: %s\n", SDL_GetError());
        return false;
    }

    // Ladda grass-bilderna: land_grass01.png → land_grass14.png
    for (int i = 0; i < 14; i++) {
        snprintf(path, sizeof(path), "resources/bana/grass/land_grass%02d.png", i + 1);  // 01 → 14
        SDL_Surface *surface = IMG_Load(path);
        if (!surface) {
            printf("Failed to load grass tile %d: %s\n", i + 1, IMG_GetError());
            return false;
        }

        pRes->pTiles[90 + i] = SDL_CreateTextureFromSurface(pRes->pRenderer, surface);
        SDL_FreeSurface(surface);

        if (!pRes->pTiles[90 + i]) {
            printf("Failed to create texture for grass tile %d: %s\n", i + 1, SDL_GetError());
            return false;
        }
    }
    return true;
}
