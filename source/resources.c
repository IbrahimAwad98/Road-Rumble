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
    if (!pStartSurface)
    {
        printf("Failed to load start button image: %s\n", IMG_GetError());
        return false;
    }
    // omvandla textur och hantera fel...
    pRes->pStartTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pStartSurface);
    SDL_FreeSurface(pStartSurface);
    if (!pRes->pStartTexture)
    {
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
    // omvandla textur och hantera fel...
    pRes->pMultiplayerTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pMultiSurface);
    SDL_FreeSurface(pMultiSurface);
    if (!pRes->pMultiplayerTexture)
    {
        printf("Failed to create multiplayer button texture: %s\n", SDL_GetError());
        return false;
    }
    // ladda "Options" bild
    SDL_Surface *pOptionsSurface = IMG_Load("resources/images/options.png");
    ;
    if (!pOptionsSurface)
    {
        printf("Failed to load option image: %s\n", IMG_GetError());
        return false;
    }
    // omvandla textur och hantera fel...
    pRes->pOptionsTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pOptionsSurface);
    SDL_FreeSurface(pOptionsSurface);
    if (!pRes->pOptionsTexture)
    {
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
    pRes->pExitTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pExitSurface);
    SDL_FreeSurface(pExitSurface);

    if (!pRes->pExitTexture)
    {
        printf("Failed to create exit button texture: %s\n", SDL_GetError());
        return false;
    }
    
    // skapa mute knappen texure....
    SDL_Surface *pMuteSurface = IMG_Load("resources/images/mute.png");
    if (!pMuteSurface)
    {
        printf("Failed to load mute button image: %s\n", IMG_GetError());
        return false;
    }   
    // omvandla textur och hantera fel...
    pRes->pMuteTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pMuteSurface);
    SDL_FreeSurface(pMuteSurface);

    if (!pRes->pMuteTexture)
    {
        printf("Failed to create mute button texture: %s\n", SDL_GetError());
        return false;
    }

    // skapa unmute knappen texure....
    SDL_Surface *pUnmuteSurface = IMG_Load("resources/images/unmute.png");
    if (!pUnmuteSurface)
    {
        printf("Failed to load unmute button image: %s\n", IMG_GetError());
        return false;
    }
    // omvandla textur och hantera fel...
    pRes->pUnmuteTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pUnmuteSurface);
    SDL_FreeSurface(pUnmuteSurface);
    
    if (!pRes->pUnmuteTexture)
    {
        printf("Failed to create unmute button texture: %s\n", SDL_GetError());
        return false;
    }

    //Skapa "back to menu" knapp...
    SDL_Surface *pBackSurface = IMG_Load("resources/images/backtomenu.png");
    if (!pBackSurface)
    {
        printf("Failed to load mute button image: %s\n", IMG_GetError());
        return false;
    }
    // omvandla textur och fel hantering...
    pRes->pBackToMenuTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, pBackSurface);
    SDL_FreeSurface(pBackSurface);

    if (!pRes->pBackToMenuTexture)
    {
        printf("Failed to create /back to menu/ button texture: %s\n", SDL_GetError());
        return false;
    }

    
    
    // bredd x höjd "Start,Multi...."
    pRes->startRect = (SDL_Rect){830, 505, 340, 60};
    pRes->multiplayerRect = (SDL_Rect){830, 585, 340, 60};
    pRes->optionsRect = (SDL_Rect){830, 665, 180, 60};
    pRes->exitRect = (SDL_Rect){1015, 665, 160, 60};
    pRes->muteRect = (SDL_Rect){1250, 665, 60, 60};
    pRes->musicVolumeRect = (SDL_Rect){680, 240, 220, 30};
    pRes->backRect = (SDL_Rect){340, 580, 590, 90};
    pRes->SfxRect = (SDL_Rect){680, 329, 220, 30};

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

    //  Ladda in asfalt-tiles (road_asphalt01.png → road_asphalt90.png)
    char path[100];
    for (int i = 0; i < NUM_ASPHALT_TILES; i++)
    {
        snprintf(path, sizeof(path), "resources/tile/asphalt/road_asphalt%02d.png", i + 1); // laddar 01 → 90
        SDL_Surface *surface = IMG_Load(path);
        if (!surface)
        {
            printf("Failed to load tile %d: %s\n", i + 1, IMG_GetError());
            return false;
        }

        pRes->pTiles[i] = SDL_CreateTextureFromSurface(pRes->pRenderer, surface);
        SDL_FreeSurface(surface);

        if (!pRes->pTiles[i])
        {
            printf("Failed to create texture for tile %d: %s\n", i + 1, SDL_GetError());
            return false;
        }
    }

    //  Ladda in hela tileset.png till ptilesetTexture
    SDL_Surface *tilesetSurface = IMG_Load("resources/images/tileset.png");
    if (!tilesetSurface)
    {
        printf("Failed to load tileset image: %s\n", IMG_GetError());
        return false;
    }

    pRes->ptilesetTexture = SDL_CreateTextureFromSurface(pRes->pRenderer, tilesetSurface);
    SDL_FreeSurface(tilesetSurface);

    if (!pRes->ptilesetTexture)
    {
        printf("Failed to create tileset texture: %s\n", SDL_GetError());
        return false;
    }

    // Ladda grass-bilderna: land_grass01.png → land_grass14.png
    for (int i = 0; i < 14; i++)
    {
        snprintf(path, sizeof(path), "resources/tile/grass/land_grass%02d.png", i + 1); // 01 → 14
        SDL_Surface *surface = IMG_Load(path);
        if (!surface)
        {
            printf("Failed to load grass tile %d: %s\n", i + 1, IMG_GetError());
            return false;
        }

        pRes->pTiles[90 + i] = SDL_CreateTextureFromSurface(pRes->pRenderer, surface);
        SDL_FreeSurface(surface);

        if (!pRes->pTiles[90 + i])
        {
            printf("Failed to create texture for grass tile %d: %s\n", i + 1, SDL_GetError());
            return false;
        }
    }
    // ladda "Options-menu" bild...
    SDL_Surface *pOptionsMenuSur = IMG_Load("resources/images/options-menu.png");
    if (!pOptionsMenuSur)
    {
        printf("Failed to load options menu image: %s\n", IMG_GetError());
        return false;
    }
    pRes->pOptionsMenuTex = SDL_CreateTextureFromSurface(pRes->pRenderer, pOptionsMenuSur);
    SDL_FreeSurface(pOptionsMenuSur);
    if (!pRes->pOptionsMenuTex)
    {
        printf("Failed to create texture for option menu : %s\n", SDL_GetError());
        return false;
    }
    // ladda "Multiplayer-menu" bild...
    SDL_Surface *pMultiplayerMenuSur = IMG_Load("resources/images/multiplayer-menu.png");
    if (!pMultiplayerMenuSur)
    {
        printf("Failed to load multiplayer menu image: %s\n", IMG_GetError());
        return false;
    }
    pRes->pMultiplayerMenuTex = SDL_CreateTextureFromSurface(pRes->pRenderer, pMultiplayerMenuSur);
    SDL_FreeSurface(pMultiplayerMenuSur);
    if (!pRes->pMultiplayerMenuTex)
    {
        printf("Failed to create texture for multiplayer menu : %s\n", SDL_GetError());
        return false;
    }
    return true;
}
