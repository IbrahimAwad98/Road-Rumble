#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>
#include "sdl_init.h"

bool initSDL(GameResources *pRes)
{
    // Initiera SDL-video
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return false;
    }

    // Initiera bildhantering
    if (!IMG_Init(IMG_INIT_PNG))
    {
        return false;
    }
    // Initiera text
    if (TTF_Init() == -1)
    {
        return false;
    }

    // Initiera ljud
    if (Mix_OpenAudio(AUDIO_FREQ, MIX_DEFAULT_FORMAT, AUDIO_CHANNELS, AUDIO_CHUNKSIZE) < 0)
    {
        return false;
    }

    // Initiera nätverk
    if (SDLNet_Init() < 0)
    {
        return false;
    }

    // Skapa fönster
    pRes->pWindow = SDL_CreateWindow("Road Rumble v1.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!pRes->pWindow)
    {
        return false;
    }

    // Skapa renderer
    pRes->pRenderer = SDL_CreateRenderer(pRes->pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    return pRes->pRenderer != NULL;
}
