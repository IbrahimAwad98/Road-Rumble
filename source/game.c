#include "game.h"
#include <stdio.h>
#include "car.h"
#include "camera.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

// Spellägen (enum) som styr vilken "skärm" spelet befinner sig i
typedef enum
{
    MENU,
    OPTIONS,
    MULTIPLAYER,
    PLAYING,
    MUTE,
    UNMUTE
} GameMode;



// En enkel tilemap för banan
int tilemap[MAP_HEIGHT][MAP_WIDTH] = {
    {-1, 2, 1, 1, 1, 1, 1, 1, 1, 4, -1},
    {-1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1},
    {-1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1},
    {-1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1},
    {-1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1},
    {-1, 38, 1, 1, 1, 1, 1, 1, 1, 40, -1}};


// Returnerar source-rektangeln från tileset baserat på tileID
SDL_Rect getTileSrcByID(int tileID)
{
    SDL_Rect src;
    src.x = (tileID % TILESET_COLUMNS) * TILE_SIZE;
    src.y = (tileID / TILESET_COLUMNS) * TILE_SIZE;
    src.w = TILE_SIZE;
    src.h = TILE_SIZE;
    return src;
}

// Renderar gräs över hela spelplanen
void renderGrassBackground(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int grassTileID, Camera *pCamera)
{
    for (int row = 0; row < MAP_HEIGHT; row++)
    {
        for (int col = 0; col < MAP_WIDTH; col++)
        {
            SDL_Rect dest = {col * TILE_SIZE - pCamera->x, row * TILE_SIZE - pCamera->y, TILE_SIZE, TILE_SIZE};
            if (pTiles[grassTileID])
            {
                SDL_RenderCopy(pRenderer, pTiles[grassTileID], NULL, &dest);
            }
        }
    }
}

// Renderar banan och andra objekt baserat på tilemap
void renderTrackAndObjects(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int tilemap[MAP_HEIGHT][MAP_WIDTH], Camera *pCamera)
{
    for (int row = 0; row < MAP_HEIGHT; row++)
    {
        for (int col = 0; col < MAP_WIDTH; col++)
        {
            SDL_Rect dest = {col * TILE_SIZE - pCamera->x, row * TILE_SIZE - pCamera->y, TILE_SIZE, TILE_SIZE};
            int tileID = tilemap[row][col];
            if (tileID == -1)
                continue;

            if (tileID >= 0 && tileID < NUM_TILES && pTiles[tileID])
            {
                SDL_RenderCopy(pRenderer, pTiles[tileID], NULL, &dest);
            }
        }
    }
}

// Initierar kameror och bilar
void initGame(GameResources *pRes)
{

    int isMuted = 0; // 0 = ljud, 1 = ljud av
    SDL_Event event;
    bool isRunning = true;
    GameMode mode = MENU;
    int hoveredButton = -1;

    // Initiera kameror
    pRes->camera1 = (Camera){0, 0, WIDTH, HEIGHT};
    pRes->camera2 = (Camera){0, 0, WIDTH, HEIGHT};

    if (!initCar(pRes->pRenderer, &pRes->car1, "resources/Cars/Black_viper.png", 300, 300, 128, 64) ||
        !initCar(pRes->pRenderer, &pRes->car2, "resources/Cars/Police.png", 100, 100, 128, 64))
    {
        printf("Failed to create car texture: %s\n", SDL_GetError());
        exit(1);
    }

    pRes->car1.angle = 0.0f;
    pRes->car1.speed = 3.0f;
}

// Hanterar ESC, P och M
void handleGlobalKeyEvents(SDL_Event *event, GameMode *mode, bool *isRunning)
{
    if (event->type == SDL_QUIT)
        *isRunning = false;

    if (event->type == SDL_KEYDOWN)
    {
        switch (event->key.keysym.sym)
        {
        case SDLK_ESCAPE:
            *isRunning = false;
            break;
        case SDLK_p:
            *mode = PLAYING;
            SDL_Log("Change to playing-mode");
            break;
        case SDLK_m:
            *mode = MENU;
            SDL_Log("Change to menu-mode");
            break;
        }
    }
}


            else if (event.type == SDL_MOUSEBUTTONDOWN && mode == MENU)
            {
                int x = event.button.x, y = event.button.y;

                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->startRect))
                {
                    SDL_Log("Start the Game!");
                    mode = PLAYING;
                }
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->exitRect))
                {
                    SDL_Log("End the Game!");
                    isRunning = false;
                }
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->multiplayerRect))
                {
                    SDL_Log("MULTIPLAYER");
                    mode = MULTIPLAYER;
                }
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->optionsRect))
                {
                    SDL_Log("OPTIONS clicked");
                    mode = OPTIONS;
                }
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->muteRect))
                {
                    SDL_Log("Mute clicked");
                    if (isMuted)
                        {
                            Mix_VolumeMusic(MIX_MAX_VOLUME); // ljudet på
                            isMuted = 0;
                        }
                        else
                        {
                            Mix_VolumeMusic(0); // ljudet av
                            isMuted = 1;
                        }
                }
            }

            if (event.type == SDL_MOUSEMOTION && mode == MENU)
            {
                int x = event.motion.x, y = event.motion.y;
                hoveredButton = -1;

                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->startRect))
                    hoveredButton = 0;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->multiplayerRect))
                    hoveredButton = 1;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->optionsRect))
                    hoveredButton = 2;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->exitRect))
                    hoveredButton = 3;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->muteRect))
                    hoveredButton = 4;
            }

// Hanterar klick i meny-läge
void handleMenuEvents(SDL_Event *event, GameResources *pRes, GameMode *mode, bool *isRunning, int *hoveredButton)
{
    if (event->type == SDL_MOUSEBUTTONDOWN)
    {
        int x = event->button.x, y = event->button.y;
        if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->startRect))
            *mode = PLAYING;
        else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->exitRect))
            *isRunning = false;
        else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->multiplayerRect))
            *mode = MULTIPLAYER;
        else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->optionsRect))
            *mode = OPTIONS;
    }
    else if (event->type == SDL_MOUSEMOTION)
    {
        int x = event->motion.x, y = event->motion.y;
        *hoveredButton = -1;
        if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->startRect))
            *hoveredButton = 0;
        else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->multiplayerRect))
            *hoveredButton = 1;
        else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->optionsRect))
            *hoveredButton = 2;
        else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->exitRect))
            *hoveredButton = 3;
    }
}

// Renderar menyn och hovereffekter
void renderMenu(GameResources *pRes, int hoveredButton)
{
    SDL_RenderCopy(pRes->pRenderer, pRes->pBackgroundTexture, NULL, NULL);
    SDL_SetTextureColorMod(pRes->pStartTexture, hoveredButton == 0 ? 200 : 255, hoveredButton == 0 ? 200 : 255, 255);
    SDL_SetTextureColorMod(pRes->pMultiplayerTexture, hoveredButton == 1 ? 200 : 255, hoveredButton == 1 ? 200 : 255, 255);
    SDL_SetTextureColorMod(pRes->pOptionsTexture, hoveredButton == 2 ? 200 : 255, hoveredButton == 2 ? 200 : 255, 255);
    SDL_SetTextureColorMod(pRes->pExitTexture, hoveredButton == 3 ? 200 : 255, hoveredButton == 3 ? 200 : 255, 255);
    SDL_RenderCopy(pRes->pRenderer, pRes->pStartTexture, NULL, &pRes->startRect);
    SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerTexture, NULL, &pRes->multiplayerRect);
    SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsTexture, NULL, &pRes->optionsRect);
    SDL_RenderCopy(pRes->pRenderer, pRes->pExitTexture, NULL, &pRes->exitRect);
}


// Renderar själva spelet (bilar, kamera, tiles)
void renderPlaying(GameResources *pRes)
{
    SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(pRes->pRenderer);

    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    updateCar(&pRes->car1, keys);


        if (mode == MENU)
        {
            SDL_RenderCopy(pRes->pRenderer, pRes->pBackgroundTexture, NULL, NULL);

            // Menyknappar + hovereffekter
            SDL_SetTextureColorMod(pRes->pStartTexture, hoveredButton == 0 ? 200 : 255, hoveredButton == 0 ? 200 : 255, 255);
            SDL_SetTextureColorMod(pRes->pMultiplayerTexture, hoveredButton == 1 ? 200 : 255, hoveredButton == 1 ? 200 : 255, 255);
            SDL_SetTextureColorMod(pRes->pOptionsTexture, hoveredButton == 2 ? 200 : 255, hoveredButton == 2 ? 200 : 255, 255);
            SDL_SetTextureColorMod(pRes->pExitTexture, hoveredButton == 3 ? 200 : 255, hoveredButton == 3 ? 200 : 255, 255);
            if (isMuted == 1)
            {
                SDL_SetTextureColorMod(pRes->pMuteTexture, hoveredButton == 4 ? 200 : 255, hoveredButton == 4 ? 200 : 255, 255);
            }
            else
            {
                SDL_SetTextureColorMod(pRes->pUnmuteTexture, hoveredButton == 4 ? 200 : 255, hoveredButton == 4 ? 200 : 255, 255);
            }
            
            


            SDL_RenderCopy(pRes->pRenderer, pRes->pStartTexture, NULL, &pRes->startRect);
            SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerTexture, NULL, &pRes->multiplayerRect);
            SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsTexture, NULL, &pRes->optionsRect);
            SDL_RenderCopy(pRes->pRenderer, pRes->pExitTexture, NULL, &pRes->exitRect);
            
            if (isMuted == 0)
            {
                SDL_RenderCopy(pRes->pRenderer, pRes->pUnmuteTexture, NULL, &pRes->muteRect);
            }else
            {
                SDL_RenderCopy(pRes->pRenderer, pRes->pMuteTexture, NULL, &pRes->muteRect);
                
            }
        }
        else if (mode == PLAYING)
        {
            SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 255);
            SDL_RenderClear(pRes->pRenderer);

    Camera *pCam = (pRes->localPlayerID == 0) ? &pRes->camera1 : &pRes->camera2;
    updateCamera(pCam, &pRes->car1.carRect);


    renderGrassBackground(pRes->pRenderer, pRes->pTiles, 93, pCam);
    renderTrackAndObjects(pRes->pRenderer, pRes->pTiles, tilemap, pCam);
    renderCar(pRes->pRenderer, &pRes->car1, pCam);
    renderCar(pRes->pRenderer, &pRes->car2, pCam);

    SDL_Rect src = getTileSrcByID(2);
    SDL_Rect dest = {400, 300, TILE_SIZE, TILE_SIZE};
    SDL_RenderCopy(pRes->pRenderer, pRes->ptilesetTexture, &src, &dest);
}

// Renderar options-menyn
void renderOptions(GameResources *pRes)
{
    SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsMenuTex, NULL, NULL);
}

// Renderar multiplayer-menyn
void renderMultiplayer(GameResources *pRes)
{
    SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerMenuTex, NULL, NULL);
}

// Huvudspel-loopen
void gameLoop(GameResources *pRes)
{
    SDL_Event event;
    bool isRunning = true;
    GameMode mode = MENU;
    int hoveredButton = -1;

    initGame(pRes);

    while (isRunning)
    {
        while (SDL_PollEvent(&event))
        {
            handleGlobalKeyEvents(&event, &mode, &isRunning);
            if (mode == MENU)
                handleMenuEvents(&event, pRes, &mode, &isRunning, &hoveredButton);
        }

        SDL_RenderClear(pRes->pRenderer);

        switch (mode)
        {
        case MENU:
            renderMenu(pRes, hoveredButton);
            break;
        case PLAYING:
            renderPlaying(pRes);
            break;
        case OPTIONS:
            renderOptions(pRes);
            break;
        case MULTIPLAYER:
            renderMultiplayer(pRes);
            break;
        }

        SDL_RenderPresent(pRes->pRenderer);
    }
}
