#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>
#include "sdl_init.h"

// Initierar alla SDL-komponenter och skapar fönster + renderare
bool initSDL(GameResources *pRes)
{
    // SDL (video)
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }
    printf("SDL video initialized successfully.\n");

    // SDL_image (endast PNG)
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("IMG_Init failed: %s\n", IMG_GetError());
        return false;
    }
    printf("SDL_image (PNG) initialized successfully.\n");

    // SDL_ttf (fonter)
    if (TTF_Init() == -1)
    {
        printf("TTF_Init failed: %s\n", TTF_GetError());
        return false;
    }
    printf("SDL_ttf initialized successfully.\n");

    // SDL_mixer (ljud)
    if (Mix_OpenAudio(AUDIO_FREQ, MIX_DEFAULT_FORMAT, AUDIO_CHANNELS, AUDIO_CHUNKSIZE) < 0)
    {
        printf("Mix_OpenAudio failed: %s\n", Mix_GetError());
        return false;
    }
    printf("SDL_mixer initialized successfully.\n");

    // SDL_net (nätverk)
    if (SDLNet_Init() < 0)
    {
        printf("SDLNet_Init failed: %s\n", SDLNet_GetError());
        return false;
    }
    printf("SDL_net initialized successfully.\n");

    // Skapa fönster
    pRes->pWindow = SDL_CreateWindow(
        "Road Rumble v1.0",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT,
        SDL_WINDOW_RESIZABLE);

    if (!pRes->pWindow)
    {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }
    printf("SDL window created successfully.\n");

    // Skapa renderare
    pRes->pRenderer = SDL_CreateRenderer(
        pRes->pWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!pRes->pRenderer)
    {
        printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
        return false;
    }
    printf("SDL renderer created successfully.\n");

    return true;
}
