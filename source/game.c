#define TILE_SIZE 128
#define TILESET_COLUMNS 3
#define MAP_WIDTH 11
#define MAP_HEIGHT 6

#include "game.h"
#include "car.h"
#include "camera.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

void renderGrassBackground(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int grassTileID, Camera *pCamera);
void renderTrackAndObjects(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int tilemap[MAP_HEIGHT][MAP_WIDTH], Camera *pCamera);
void updateCamera(Camera *pCamera, SDL_Rect *pTarget);

// Skapa läges-typ (enum)
typedef enum
{
    MENU,
    PLAYING
} GameMode;

// Hjälpfunktion: hämtar rätt tile från tileset utifrån ID
SDL_Rect getTileSrcByID(int tileID)
{
    SDL_Rect src;
    src.x = (tileID % TILESET_COLUMNS) * TILE_SIZE;
    src.y = (tileID / TILESET_COLUMNS) * TILE_SIZE;
    src.w = TILE_SIZE;
    src.h = TILE_SIZE;
    return src;
}

int tilemap[MAP_HEIGHT][MAP_WIDTH] = {
    {-1, 2, 1, 1, 1, 1, 1, 1, 1, 4, -1},
    {-1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1},
    {-1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1},
    {-1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1},
    {-1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1},
    {-1, 38, 1, 1, 1, 1, 1, 1, 1, 40, -1}
};

void gameLoop(GameResources *pRes)
{
    SDL_Event event;
    bool isRunning = true;
    GameMode mode = MENU; // Starta i meny-läge

    pRes->camera1.x = 0;
    pRes->camera1.y = 0;
    pRes->camera1.w = WIDTH;
    pRes->camera1.h = HEIGHT;

    pRes->camera2.x = 0;
    pRes->camera2.y = 0;
    pRes->camera2.w = WIDTH;
    pRes->camera2.h = HEIGHT;

    // Initiera bilarna EN gång (bra för hantera minnet)
    // nummerna = pixlar horisontellt, vertikalt och storlek: bredd x höjd
    if (!initiCar(pRes->pRenderer, &pRes->car1, "resources/Cars/Black_viper.png", 300, 300, 128, 64) ||
        !initiCar(pRes->pRenderer, &pRes->car2, "resources/Cars/Police.png", 100, 100, 128, 64))
    {
        printf("Failed to create car texture: %s\n", SDL_GetError());
        return;
    }

    while (isRunning)
    {
        // Hantera event
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
            {
                isRunning = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN && mode == MENU)
            {
                int x = event.button.x;
                int y = event.button.y;

                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->startRect))
                {
                    SDL_Log("Start the Game!");
                    mode = PLAYING; // aktivera playingläge
                }

                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->exitRect))
                {
                    SDL_Log("End the Game!");
                    isRunning = false;
                }
            }

            // Byt mode via tangent
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_t)
                {
                    mode = PLAYING;
                    SDL_Log("Change to playing-mode");
                }
                else if (event.key.keysym.sym == SDLK_m)
                {
                    mode = MENU;
                    SDL_Log("Change to meny-mode");
                }
            }
        }

        // Rensa skärmen
        SDL_RenderClear(pRes->pRenderer);

        // Rendera beroende på vilket läge vi är i
        if (mode == MENU)
        {
            SDL_RenderCopy(pRes->pRenderer, pRes->pBackgroundTexture, NULL, NULL);
            SDL_RenderCopy(pRes->pRenderer, pRes->pStartTexture, NULL, &pRes->startRect);
            SDL_RenderCopy(pRes->pRenderer, pRes->pExitTexture, NULL, &pRes->exitRect);
        } 
        else if (mode == PLAYING)
        {
            SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 255); // svart bakgrund
            SDL_RenderClear(pRes->pRenderer);
            Camera *pActiveCamera = (pRes->localPlayerID == 0) ? &pRes->camera1 : &pRes->camera2;

            updateCamera(pActiveCamera, &pRes->car1.carRect);
            
            renderGrassBackground(pRes->pRenderer, pRes->pTiles, 93, pActiveCamera);
            renderTrackAndObjects(pRes->pRenderer, pRes->pTiles, tilemap, pActiveCamera);
            
            renderCar(pRes->pRenderer, &pRes->car1, pActiveCamera);
            renderCar(pRes->pRenderer, &pRes->car2, pActiveCamera);

            // 🧪 Test: rendera en test-tile (valfritt behålla)
            SDL_Rect src = getTileSrcByID(2); // tile ID 2 från tileset
            SDL_Rect dest = {400, 300, TILE_SIZE, TILE_SIZE};
            SDL_RenderCopy(pRes->pRenderer, pRes->ptilesetTexture, &src, &dest);
        }

        // Flytta upp SDL_RenderPresent hit så att det körs varje loop-iteration
        SDL_RenderPresent(pRes->pRenderer);
    } // end while(isRunning)
} // end gameLoop

void renderGrassBackground(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int grassTileID, Camera *pCamera){
    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
            SDL_Rect dest = { col * TILE_SIZE - pCamera->x, row * TILE_SIZE - pCamera->y, TILE_SIZE, TILE_SIZE };

            if (pTiles[grassTileID]) {
                SDL_RenderCopy(pRenderer, pTiles[grassTileID], NULL, &dest);
            }
        }
    }
}

void renderTrackAndObjects(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int tilemap[MAP_HEIGHT][MAP_WIDTH], Camera *pCamera){
    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int col = 0; col < MAP_WIDTH; col++) {
            SDL_Rect dest = { col * TILE_SIZE - pCamera->x, row * TILE_SIZE - pCamera->y, TILE_SIZE, TILE_SIZE };
            int tileID = tilemap[row][col];

            if (tileID == -1) {
                continue; // ❌ hoppa över rutor som är täckta
            }

            // ✅ RITA om: Endast om giltigt ID och texturen finns
            if (tileID >= 0 && tileID < NUM_TILES && pTiles[tileID]) {
                SDL_RenderCopy(pRenderer, pTiles[tileID], NULL, &dest);
            }
        }
    }
}

