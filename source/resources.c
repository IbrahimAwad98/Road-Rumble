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
    // ladda "start game" bild 
    SDL_Surface *pStartSurface = IMG_Load("resources/images/start.png");
    if (!pStartSurface) {
        printf("Failed to load start button image: %s\n", IMG_GetError());
        return false;
    }
    //omvandla textur och hantera fel...
    pRes->pStartTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pStartSurface);
    SDL_FreeSurface(pStartSurface);
    if (!pRes->pStartTexture) {
        printf("Failed to create start button texture: %s\n", SDL_GetError());
        return false;
    }
    // ladda "Multiplayer" bild
    SDL_Surface *pMultiSurface = IMG_Load("resources/images/multiplayer.png");
    if (!pMultiSurface)
    {
        printf("Failed to load multiplayer image: %s\n", IMG_GetError());
        return false;
    }
    //omvandla textur och hantera fel...
    pRes->pMultiplayerTexture =  SDL_CreateTextureFromSurface(pRes->pRenderer,pMultiSurface);
    SDL_FreeSurface(pMultiSurface);
    if (!pRes->pMultiplayerTexture) {
        printf("Failed to create multiplayer button texture: %s\n", SDL_GetError());
        return false;
    }
    // ladda "Options" bild
    SDL_Surface *pOptionsSurface = IMG_Load("resources/images/options.png");;
    if (!pOptionsSurface)
    {
        printf("Failed to load option image: %s\n", IMG_GetError());
        return false;    
    }
    // omvandla textur och hantera fel...
    pRes->pOptionsTexture = SDL_CreateTextureFromSurface(pRes->pRenderer,pOptionsSurface);
    SDL_FreeSurface(pOptionsSurface);
    if (!pRes->pOptionsTexture) {
        printf("Failed to create multiplayer button texture: %s\n", SDL_GetError());
        return false;
    }
    // ladda "Exit" image...
    SDL_Surface *pExitSurface = IMG_Load("resources/images/exit.png");
    if (!pExitSurface)
    {
        printf("Failed to load exit button image: %s\n", IMG_GetError());
        return false;
    }
    // omvandla textur och hantera fel...
    pRes->pExitTexture = SDL_CreateTextureFromSurface(pRes->pRenderer,pExitSurface);
    SDL_FreeSurface(pExitSurface);

    if (!pRes->pExitTexture)
    {
        printf("Failed to create exit button texture: %s\n",SDL_GetError());
        return false;
    }
    
    //bredd x höjd "Start,Multi...."
    pRes->startRect       = (SDL_Rect){830, 505, 340, 60};
    pRes->multiplayerRect = (SDL_Rect){830, 585, 340, 60};
    pRes->optionsRect     = (SDL_Rect){830, 665, 180, 60};
    pRes->exitRect        = (SDL_Rect){1015, 665, 160, 60};
    
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
    /*SDL_Color green = {0, 255, 0};
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
    }*/

    // 💡 Laddar tileset-bild också:
    SDL_Surface *surface = IMG_Load("resources/images/tileset.png");
    // hantera fel.
    if (!surface)
    {
        printf("Failed to load tileset image: %s\n", IMG_GetError());
        return false;
    }
    pRes->ptilesetTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, surface);
    SDL_FreeSurface(surface); // glöm inte släppa ytan!

    return true;
}
