#define TILE_SIZE 128
#define TILESET_COLUMNS 3
#define MAP_WIDTH 20
#define MAP_HEIGHT 10

#include "game.h"
#include "car.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

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
    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
    {100, 2, 1, 0, 0, 0, 0, 0, 0, 100},
    {100, 0, 0, 0, 0, 0, 0, 0, 0, 100},
    {100, 0, 0, 0, 0, 0, 0, 0, 0, 100},
    {100, 0, 0, 0, 0, 0, 0, 0, 0, 100},
    {100, 0, 0, 0, 0, 0, 0, 0, 0, 100},
    {100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
};

void gameLoop(GameResources *pRes)
{
    SDL_Event event;
    bool isRunning = true;
    GameMode mode = MENU; // Starta i meny-läge

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
<<<<<<< HEAD
        }
        else if (mode == PLAYING)
        {
            SDL_SetRenderDrawColor(pRes->pRenderer, 255, 255, 255, 255);
            SDL_RenderClear(pRes->pRenderer);

            for (int row = 0; row < MAP_HEIGHT; row++)
            {
                for (int col = 0; col < MAP_WIDTH; col++)
                {
                    int tileID = tilemap[row][col];
                    SDL_Rect dest = {col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE};

                    if (tileID >= 0 && tileID < NUM_TILES && pRes->pTiles[tileID])
                    {
                        SDL_RenderCopy(pRes->pRenderer, pRes->pTiles[tileID], NULL, &dest);
                    }
                }
                == == == =
            }
            else if (mode == PLAYING)
            {
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 255); // svart
                SDL_RenderClear(pRes->pRenderer);

                renderCar(pRes->pRenderer, &pRes->car1);
                renderCar(pRes->pRenderer, &pRes->car2);

                // Test: rendera en tile
                SDL_Rect src = getTileSrcByID(2); // tile ID 2 från tileset
                SDL_Rect dest = {400, 300, TILE_SIZE, TILE_SIZE};
                SDL_RenderCopy(pRes->pRenderer, pRes->ptilesetTexture, &src, &dest);
>>>>>>> c3d084a1f269ec16b472ec2223d47f56dc563b57
            }
        }

        SDL_RenderPresent(pRes->pRenderer);
    }
}
