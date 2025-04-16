#include "game.h"
#include "car.h"
#include "camera.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

// Returnerar korrekt rektangel från tileset baserat på tileID
SDL_Rect getTileSrcByID(int tileID)
{
    SDL_Rect src;
    src.x = (tileID % TILESET_COLUMNS) * TILE_SIZE; // kolumn i tileset
    src.y = (tileID / TILESET_COLUMNS) * TILE_SIZE; // rad i tileset
    src.w = TILE_SIZE;
    src.h = TILE_SIZE;
    return src;
}

// Tilemap (banlayout) definierad med tile-ID
int tilemap[MAP_HEIGHT][MAP_WIDTH] = {
    {-1, 2, 1, 1, 1, 1, 1, 1, 1, 4, -1},
    {-1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1},
    {-1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1},
    {-1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1},
    {-1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1},
    {-1, 38, 1, 1, 1, 1, 1, 1, 1, 40, -1}};

// Renderar ett helt bakgrundslager av gräs över hela banan
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

// Renderar själva vägen/banan och andra objekt från tilemap
void renderTrackAndObjects(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int tilemap[MAP_HEIGHT][MAP_WIDTH], Camera *pCamera)
{
    for (int row = 0; row < MAP_HEIGHT; row++)
    {
        for (int col = 0; col < MAP_WIDTH; col++)
        {
            SDL_Rect dest = {col * TILE_SIZE - pCamera->x, row * TILE_SIZE - pCamera->y, TILE_SIZE, TILE_SIZE};
            int tileID = tilemap[row][col];

            if (tileID == -1)
            {
                continue; // hoppa över täckta rutor
            }

            if (tileID >= 0 && tileID < NUM_TILES && pTiles[tileID])
            {
                SDL_RenderCopy(pRenderer, pTiles[tileID], NULL, &dest);
            }
        }
    }
}

// Spelets huvudloop – styr meny, rendering, input m.m.
void gameLoop(GameResources *pRes)
{
    int isMuted = 0; // Flagga för ljud av/på
    SDL_Event event;
    bool isRunning = true;
    GameMode mode = MENU;   // Startläge: meny
    int hoveredButton = -1; // Vilken knapp är muspekaren över?

    // Initiera två kameror (en per spelare i multiplayer t.ex.)
    pRes->camera1 = (Camera){0, 0, WIDTH, HEIGHT};
    pRes->camera2 = (Camera){0, 0, WIDTH, HEIGHT};

    // Initiera två bilar med olika positioner och texturer
    if (!initCar(pRes->pRenderer, &pRes->car1, "resources/Cars/Black_viper.png", 300, 300, 128, 64) ||
        !initCar(pRes->pRenderer, &pRes->car2, "resources/Cars/Police.png", 100, 100, 128, 64))
    {
        printf("Failed to create car texture: %s\n", SDL_GetError());
        return;
    }

    pRes->car1.angle = 0.0f;
    pRes->car1.speed = 3.0f;

    while (isRunning)
    {
        // Eventhantering
        while (SDL_PollEvent(&event))
        {
            // Avsluta spelet
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
                isRunning = false;

            // Menyinteraktion – klick på knappar
            else if (event.type == SDL_MOUSEBUTTONDOWN && mode == MENU)
            {
                int x = event.button.x, y = event.button.y;

                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->startRect))
                {
                    mode = PLAYING;
                }
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->exitRect))
                {
                    isRunning = false;
                }
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->multiplayerRect))
                {
                    mode = MULTIPLAYER;
                }
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->optionsRect))
                {
                    mode = OPTIONS;
                }
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->muteRect))
                {
                    isMuted = !isMuted;
                    Mix_VolumeMusic(isMuted ? 0 : MIX_MAX_VOLUME);
                }
            }

            // Hovereffekt för menyknappar
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

            // Tangenttryckningar för att växla spelläge manuellt
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_p)
                    mode = PLAYING;
                else if (event.key.keysym.sym == SDLK_m)
                    mode = MENU;
            }
        }

        SDL_RenderClear(pRes->pRenderer); // Rensa skärmen

        // RENDERING beroende på spelläge
        if (mode == MENU)
        {
            // Bakgrund + knappar med hovereffekter
            SDL_RenderCopy(pRes->pRenderer, pRes->pBackgroundTexture, NULL, NULL);
            // Hoverfärg justeras
            SDL_SetTextureColorMod(pRes->pStartTexture, hoveredButton == 0 ? 200 : 255, hoveredButton == 0 ? 200 : 255, 255);
            // ... upprepa för andra knappar
            // Visa knappar
            SDL_RenderCopy(pRes->pRenderer, pRes->pStartTexture, NULL, &pRes->startRect);
            SDL_RenderCopy(pRes->pRenderer, isMuted ? pRes->pMuteTexture : pRes->pUnmuteTexture, NULL, &pRes->muteRect);
        }
        else if (mode == PLAYING)
        {
            // Uppdatera bilens position, kameran och rendera hela spelvärlden
            const Uint8 *keys = SDL_GetKeyboardState(NULL);
            updateCar(&pRes->car1, keys);

            Camera *pCam = (pRes->localPlayerID == 0) ? &pRes->camera1 : &pRes->camera2;
            updateCamera(pCam, &pRes->car1.carRect);

            renderGrassBackground(pRes->pRenderer, pRes->pTiles, 93, pCam);
            renderTrackAndObjects(pRes->pRenderer, pRes->pTiles, tilemap, pCam);
            renderCar(pRes->pRenderer, &pRes->car1, pCam);
            renderCar(pRes->pRenderer, &pRes->car2, pCam);

            // Exempel på rendering av en tile
            SDL_Rect src = getTileSrcByID(2);
            SDL_Rect dest = {400, 300, TILE_SIZE, TILE_SIZE};
            SDL_RenderCopy(pRes->pRenderer, pRes->ptilesetTexture, &src, &dest);
        }
        else if (mode == OPTIONS)
        {
            SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsMenuTex, NULL, NULL);
        }
        else if (mode == MULTIPLAYER)
        {
            SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerMenuTex, NULL, NULL);
        }

        SDL_RenderPresent(pRes->pRenderer); // Uppdatera fönstret
    }
}
