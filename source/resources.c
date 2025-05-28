#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "resources.h"
#include "tilemap.h"

// Hjälpfunktion för att ladda bild till texture
bool createSurfaceAndTexture(SDL_Texture **pTexture, SDL_Renderer *pRenderer, const char *pics)
{
    SDL_Surface *pSurface = IMG_Load(pics);
    if (!pSurface)
    {
        printf("Failed to load image: %s\n", IMG_GetError());
        return false;
    }

    *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);

    if (!*pTexture)
    {
        printf("Failed to create texture: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

// Laddar alla resurser för spelet
bool loadResources(GameResources *pRes)
{
    // Classic mode UI
    if (!createSurfaceAndTexture(&pRes->pBackgroundTexture, pRes->pRenderer, "resources/images/Background.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pStartTexture, pRes->pRenderer, "resources/images/start.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pMultiplayerTexture, pRes->pRenderer, "resources/images/multiplayer.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pOptionsTexture, pRes->pRenderer, "resources/images/options.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pExitTexture, pRes->pRenderer, "resources/images/exit.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pMuteTexture, pRes->pRenderer, "resources/images/mute.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pUnmuteTexture, pRes->pRenderer, "resources/images/unmute.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pBackToMenuTexture, pRes->pRenderer, "resources/images/backtomenu.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pBackToMultiTexture, pRes->pRenderer, "resources/images/backtomulti.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pEnterGameTexture, pRes->pRenderer, "resources/images/entertomulti.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pClassicTexture, pRes->pRenderer, "resources/images/modes.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pOptionsMenuTex, pRes->pRenderer, "resources/images/options-menu.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pMultiplayerMenuTex, pRes->pRenderer, "resources/images/multiplayer-menu.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pWASDTexture, pRes->pRenderer, "resources/images/wasd.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pArrowTexture, pRes->pRenderer, "resources/images/arrows.png"))
        return false;

    // Classic UI rektanglar
    pRes->startRect = (SDL_Rect){830, 505, 340, 60};
    pRes->multiplayerRect = (SDL_Rect){830, 585, 340, 60};
    pRes->optionsRect = (SDL_Rect){830, 665, 180, 60};
    pRes->exitRect = (SDL_Rect){1015, 665, 160, 60};
    pRes->muteRect = (SDL_Rect){1250, 665, 60, 60};
    pRes->musicVolumeRect = (SDL_Rect){680, 240, 220, 30};
    pRes->backRect = (SDL_Rect){340, 580, 590, 90};
    pRes->SfxRect = (SDL_Rect){680, 329, 220, 30};
    pRes->portRect = (SDL_Rect){465, 230, 646, 85};
    pRes->joinRect = (SDL_Rect){465, 325, 646, 89};
    pRes->playerIdRect = (SDL_Rect){465, 430, 646, 85};
    pRes->backMRect = (SDL_Rect){240, 540, 370, 75};
    pRes->enterRect = (SDL_Rect){720, 540, 370, 75};
    pRes->classicRect = (SDL_Rect){670, 398, 250, 70};
    pRes->WASDRect = (SDL_Rect){750, 490, 170, 60};
    pRes->arrowRect = (SDL_Rect){750, 490, 170, 60};

    // Dark mode UI
    if (!createSurfaceAndTexture(&pRes->pBackgroundDarkTexture, pRes->pRenderer, "resources/images/BackgroundDark.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pDarkTexture, pRes->pRenderer, "resources/images/dark.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pOptionsMenuDarkTex, pRes->pRenderer, "resources/images/option-menu-dark.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pMultiplayerMenuDarkTex, pRes->pRenderer, "resources/images/multiplayer-menu-dark.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pWASDDarkTexture, pRes->pRenderer, "resources/images/wasd-dark.png"))
        return false;
    if (!createSurfaceAndTexture(&pRes->pArrowDarkTexture, pRes->pRenderer, "resources/images/arrows-dark.png"))
        return false;

    // Dark mode UI rektanglar
    pRes->darkRect = (SDL_Rect){670, 398, 250, 70};
    pRes->SfxDarkRect = (SDL_Rect){650, 310, 220, 30};
    pRes->backDarkRect = (SDL_Rect){340, 550, 570, 80};
    pRes->musicVolumeDarkRect = (SDL_Rect){650, 235, 220, 30};
    pRes->portDarkRect = (SDL_Rect){470, 240, 646, 78};
    pRes->joinDarkRect = (SDL_Rect){472, 328, 646, 79};
    pRes->playerIdDarkRect = (SDL_Rect){470, 420, 646, 72};
    pRes->backMDarkRect = (SDL_Rect){240, 500, 370, 75};
    pRes->enterDarkRect = (SDL_Rect){720, 500, 370, 75};
    pRes->WASDDarkRect = (SDL_Rect){745, 460, 145, 60};
    pRes->arrowDarRect = (SDL_Rect){745, 460, 145, 60};

    // Effekter
    if (!createSurfaceAndTexture(&pRes->pTireTrailTexture, pRes->pRenderer, "resources/tile/effects/tire.png"))
        return false;
    SDL_SetTextureBlendMode(pRes->pTireTrailTexture, SDL_BLENDMODE_BLEND);

    char path1[128];
    for (int i = 0; i < BOOST_FRAME_COUNT; ++i)
    {
        snprintf(path1, sizeof(path1), "resources/tile/effects/flame_0%d.png", i + 1);
        if (!createSurfaceAndTexture(&pRes->pBoostFlameFrames[i], pRes->pRenderer, path1))
            return false;
        SDL_SetTextureBlendMode(pRes->pBoostFlameFrames[i], SDL_BLENDMODE_BLEND);
    }

    char path2[128];
    for (int i = 0; i < LIGHTS_FRAME_COUNT; ++i)
    {
        snprintf(path2, sizeof(path2), "resources/tile/effects/lights_0%d.png", i + 1);
        if (!createSurfaceAndTexture(&pRes->pTiles[120 + i], pRes->pRenderer, path2))
            return false;
    }

    char soundPath[128];
    for (int i = 0; i < LIGHTS_SOUND_COUNT; ++i)
    {
        snprintf(soundPath, sizeof(soundPath), "resources/Music/race_light_part_%d.wav", i + 1);
        pRes->lightSounds[i] = Mix_LoadWAV(soundPath);
        if (!pRes->lightSounds[i])
        {
            printf("Failed to load light sound %d: %s\n", i + 1, Mix_GetError());
            return false;
        }
    }

    SDL_Surface *grassSurface = IMG_Load("resources/tile/grass/land_grass15.png");
    if (!grassSurface)
    {
        printf("Failed to load lights tile: %s\n", IMG_GetError());
        return false;
    }

    pRes->pTiles[119] = SDL_CreateTextureFromSurface(pRes->pRenderer, grassSurface);
    SDL_FreeSurface(grassSurface);
    if (!pRes->pTiles[119])
    {
        printf("Failed to create texture for lights tile: %s\n", SDL_GetError());
        return false;
    }
    // Musik
    pRes->pBgMusic = Mix_LoadMUS("resources/music/intro_Opening.mp3");
    if (!pRes->pBgMusic)
        printf("Failed to load background music: %s\n", Mix_GetError());
    else
        Mix_PlayMusic(pRes->pBgMusic, -1);

    // Font
    pRes->pFont = TTF_OpenFont("resources/fonts/PressStart2P-Regular.ttf", 35);
    if (!pRes->pFont)
    {
        printf("Failed to load font: %s\n", TTF_GetError());
        return false;
    }

    // Asfalt-tiles
    char path[100];
    for (int i = 0; i < NUM_ASPHALT_TILES; i++)
    {
        snprintf(path, sizeof(path), "resources/tile/asphalt/road_asphalt%02d.png", i + 1);
        if (!createSurfaceAndTexture(&pRes->pTiles[i], pRes->pRenderer, path))
        {
            printf("Failed to load asphalt tile %d\n", i + 1);
            return false;
        }
    }

    // Grass-tiles
    for (int i = 0; i < 14; i++)
    {
        snprintf(path, sizeof(path), "resources/tile/grass/land_grass%02d.png", i + 1);
        if (!createSurfaceAndTexture(&pRes->pTiles[90 + i], pRes->pRenderer, path))
        {
            printf("Failed to load grass tile %d\n", i + 1);
            return false;
        }
    }

    // Dekorationer
    const char *decorFilenames[NUM_DECOR_TILES] = {
        "Bush_01.png", "Race_Flag.png", "Decor_Building_01.png", "Decor_Building_02.png",
        "Finish.png", "Pavilion_01.png", "Pavilion_02.png", "Racing_Lights.png",
        "Water_Tile.png", "Tree_01.png", "Tree_02.png", "Tribune_full.png",
        "Boost.png", "barrel.png", "Crate.png"};

    char path3[100];
    for (int i = 0; i < NUM_DECOR_TILES; i++)
    {
        snprintf(path3, sizeof(path3), "resources/tile/decor/%s", decorFilenames[i]);
        if (!createSurfaceAndTexture(&pRes->pTiles[TILE_OFFSET_DECOR + i], pRes->pRenderer, path3))
        {
            printf("Failed to load decor tile %s\n", decorFilenames[i]);
            return false;
        }
    }

    return true;
}