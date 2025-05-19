    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_mixer.h>
    #include "resources.h"
    #include "tilemap.h"

    bool createSurfaceAndTexture(SDL_Texture **pTexture, SDL_Renderer *pRenderer  ,const char *pics){
        SDL_Surface *pSurface = IMG_Load(pics);
        if (!pSurface)
        {
            printf("Failed to load background image: %s\n", IMG_GetError());
            return false;
        }
        // omvandla till texture och hantera fel.
        *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
        SDL_FreeSurface(pSurface);
        if (!*pTexture)
        {
            printf("Failed to create background texture: %s\n", SDL_GetError());
            return false;
        }
        return true;

    };

    bool loadResources(GameResources *pRes)
    {

        //--------------------------------------------------------------
        // Resources for Classic mode
        //--------------------------------------------------------------


        //ladda bakgrund bilden
        if (!createSurfaceAndTexture(&pRes->pBackgroundTexture, pRes->pRenderer, "resources/images/Background.png"))
        {

            return false;
        }

        //ladda "start game" knapp bild
        if (!createSurfaceAndTexture(&pRes->pStartTexture, pRes->pRenderer, "resources/images/start.png"))
        {
            return false;
        }

        // ladda "Multiplayer" knapp bild
        if (!createSurfaceAndTexture(&pRes->pMultiplayerTexture, pRes->pRenderer, "resources/images/multiplayer.png"))
        {
            return false;
        }

        
        // ladda "Options" knapp bild
        if (!createSurfaceAndTexture(&pRes->pOptionsTexture, pRes->pRenderer, "resources/images/options.png"))
        {
            return false;
        }
        
        
        // ladda "Exit" knapp bild...
        if (!createSurfaceAndTexture(&pRes->pExitTexture, pRes->pRenderer, "resources/images/exit.png"))
        {
            return false;
        }

        // skapa "mute" knappen texure...
        if (!createSurfaceAndTexture(&pRes->pMuteTexture, pRes->pRenderer, "resources/images/mute.png"))
        {
            return false;
        }

        // skapa "unmute" knappen bild
        if (!createSurfaceAndTexture(&pRes->pUnmuteTexture, pRes->pRenderer, "resources/images/unmute.png"))
        {
            return false;
        }
        
        // Skapa "back to menu" knapp...
        if (!createSurfaceAndTexture(&pRes->pBackToMenuTexture, pRes->pRenderer, "resources/images/backtomenu.png"))
        {
            return false;
        }
        
        // Skapa "back to menu" knapp...
        if (!createSurfaceAndTexture(&pRes->pBackToMultiTexture, pRes->pRenderer, "resources/images/backtomulti.png"))
        {
            return false;
        }
        
        // skapa enter textur
        if (!createSurfaceAndTexture(&pRes->pEnterGameTexture, pRes->pRenderer, "resources/images/entertomulti.png"))
        {
            return false;
        }
        
        // skapa classic knappen texure...
        if (!createSurfaceAndTexture(&pRes->pClassicTexture, pRes->pRenderer, "resources/images/modes.png"))
        {
            return false;
        }
        // ladda "Options-menu" bild...

        if (!createSurfaceAndTexture(&pRes->pOptionsMenuTex, pRes->pRenderer, "resources/images/options-menu.png"))
        {
            return false;
        }
        
        // ladda "Multiplayer-menu" bild...
        if (!createSurfaceAndTexture(&pRes->pMultiplayerMenuTex, pRes->pRenderer, "resources/images/multiplayer-menu.png"))
        {
            return false;
        }
        
        if (!createSurfaceAndTexture(&pRes->pWASDTexture, pRes->pRenderer, "resources/images/wasd.png"))
        {

            return false;
        }

        if (!createSurfaceAndTexture(&pRes->pArrowTexture, pRes->pRenderer, "resources/images/arrows.png"))
        {

            return false;
        }
        
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


        


        //----------------------------------------------------------
        // Resources för dark mod
        //----------------------------------------------------------


        //ladda bakgrund "Dark" bild
        if (!createSurfaceAndTexture(&pRes->pBackgroundDarkTexture, pRes->pRenderer, "resources/images/BackgroundDark.png"))
        {
            return false;
        }
 
        //skapa dark knappen texure....
        if (!createSurfaceAndTexture(&pRes->pDarkTexture, pRes->pRenderer, "resources/images/dark.png"))
        {
            return false;
        }
        // ladda "Options-menu" bild...

        if (!createSurfaceAndTexture(&pRes->pOptionsMenuDarkTex, pRes->pRenderer, "resources/images/option-menu-dark.png"))
        {
            return false;
        }
        
        // ladda "Multiplayer-menu" bild...
        if (!createSurfaceAndTexture(&pRes->pMultiplayerMenuDarkTex, pRes->pRenderer, "resources/images/multiplayer-menu-dark.png"))
        {
            return false;
        }

        if (!createSurfaceAndTexture(&pRes->pWASDDarkTexture, pRes->pRenderer, "resources/images/wasd-dark.png"))
        {

            return false;
        }
        if (!createSurfaceAndTexture(&pRes->pArrowDarkTexture, pRes->pRenderer, "resources/images/arrows-dark.png"))
        {

            return false;
        }

        // bredd x höjd "Start,Multi...."
        
        pRes->darkRect = (SDL_Rect){670, 398, 250, 70};
        pRes->SfxDarkRect = (SDL_Rect){650, 310, 220, 30};
        pRes->backDarkRect = (SDL_Rect){340, 550, 570, 80};
        pRes->musicVolumeDarkRect = (SDL_Rect){650, 235, 220, 30};
        pRes->portDarkRect = (SDL_Rect){470, 240, 646, 78};
        pRes->joinDarkRect = (SDL_Rect){472, 328, 646, 79};
        pRes->playerIdDarkRect = (SDL_Rect){470, 420, 646, 72};
        pRes->backMDarkRect= (SDL_Rect){240, 500, 370, 75};
        pRes->enterDarkRect = (SDL_Rect){720, 500, 370, 75};
        pRes->WASDDarkRect = (SDL_Rect){745, 460, 145, 60};
        pRes->arrowDarRect = (SDL_Rect){745, 460, 145, 60};
        

        //------------------------------------------------
        //Spell Effekter
        //------------------------------------------------

        if (!createSurfaceAndTexture(&pRes->pTireTrailTexture, pRes->pRenderer, "resources/tile/effects/tire.png"))
        {
            return false;
        }
        SDL_SetTextureBlendMode(pRes->pTireTrailTexture, SDL_BLENDMODE_BLEND);

     char path1[128];
    for (int i = 0; i < BOOST_FRAME_COUNT; ++i) {
        snprintf(path1, sizeof(path1), "resources/tile/effects/flame_0%d.png", i + 1);
        if (!createSurfaceAndTexture(&pRes->pBoostFlameFrames[i], pRes->pRenderer, path1)) {
            return false;
        }
        SDL_SetTextureBlendMode(pRes->pBoostFlameFrames[i], SDL_BLENDMODE_BLEND);
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

    //  Ladda in asfalt-tiles (road_asphalt01.png → road_asphalt90.png) BANAN
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

    //  Ladda in hela start till ptilesetTexture (måste justeras)
    /*SDL_Surface *tilesetSurface = IMG_Load("resources/tile/decor/Start.png");
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
    }*/

    // Ladda grass-bilderna: land_grass01.png → land_grass14.png BANAN
    for (int i = 0; i < 14; i++)
    {
        // Skapar en filväg som t.ex. "resources/tile/grass/land_grass01.png", "land_grass02.png" osv
        snprintf(path, sizeof(path), "resources/tile/grass/land_grass%02d.png", i + 1);
        SDL_Surface *surface = IMG_Load(path);
        if (!surface)
        {
            printf("Failed to load grass tile %d: %s\n", i + 1, IMG_GetError());
            return false; // Om bilden inte kunde laddas – avbryt funktionen
        }

        // Gör om bilden till en SDL_Texture som kan ritas ut, och spara i plats 90+i i pTiles[]
        // Ex: land_grass01.png sparas i pTiles[90], land_grass02.png i pTiles[91] osv
        pRes->pTiles[90 + i] = SDL_CreateTextureFromSurface(pRes->pRenderer, surface);

        SDL_FreeSurface(surface); // Bilden i RAM behövs inte längre – radera den

        if (!pRes->pTiles[90 + i]) // Om texturen inte kunde skapas, visa felmeddelande och avbryt
        {
            printf("Failed to create texture for grass tile %d: %s\n", i + 1, SDL_GetError());
            return false;
        }
    }
        
// Ladda decor-tiles: index 104–118 Bana
    const char *decorFilenames[NUM_DECOR_TILES] = {
        "Bush_01.png", "Race_Flag.png", "Decor_Building_01.png", "Decor_Building_02.png",
        "Finish.png", "Pavilion_01.png", "Pavilion_02.png", "Racing_Lights.png",
        "Water_Tile.png", "Tree_01.png", "Tree_02.png", "Tribune_full.png", "Boost.png","barrel.png","Crate.png"};

    for (int i = 0; i < NUM_DECOR_TILES; i++)
    {
        snprintf(path, sizeof(path), "resources/tile/decor/%s", decorFilenames[i]);
        SDL_Surface *surface = IMG_Load(path);
        if (!surface)
        {
            printf("Failed to load decor tile %s: %s\n", decorFilenames[i], IMG_GetError());
            return false;
        }

        pRes->pTiles[TILE_OFFSET_DECOR + i] = SDL_CreateTextureFromSurface(pRes->pRenderer, surface);
        SDL_FreeSurface(surface);

        if (!pRes->pTiles[TILE_OFFSET_DECOR + i])
        {
            printf("Failed to create texture for decor tile %s: %s\n", decorFilenames[i], SDL_GetError());
            return false;
        }
    }
    return true;
}
