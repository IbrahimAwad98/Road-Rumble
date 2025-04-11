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
    OPTIONS,
    MULTIPLAYER,
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
    int hoveredButton = -1;

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
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->multiplayerRect))
                {
                    SDL_Log("MULTIPLAYER");

                    mode = MULTIPLAYER; // Or handle however you like

                    mode = MULTIPLAYER; // gå in Multiplayer Menyn
                }
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->optionsRect))
                {
                    SDL_Log("OPTIONS clicked");

                    mode = OPTIONS; // Or handle however you like

                    mode = OPTIONS; // gå in Options Menyn
                }
            }

            if (event.type == SDL_MOUSEMOTION && mode == MENU)
            {
                int x = event.motion.x;
                int y = event.motion.y;
                hoveredButton = -1;

                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->startRect))
                    hoveredButton = 0;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->multiplayerRect))
                    hoveredButton = 1;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->optionsRect))
                    hoveredButton = 2;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->exitRect))
                    hoveredButton = 3;
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
            SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerTexture, NULL, &pRes->multiplayerRect);
            SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsTexture, NULL, &pRes->optionsRect);
            // START-Rekt effekt
            if (hoveredButton == 0)
                SDL_SetTextureColorMod(pRes->pStartTexture, 200, 200, 255); // mörkare glöd
            else
                SDL_SetTextureColorMod(pRes->pStartTexture, 255, 255, 255); // vanlig
            SDL_RenderCopy(pRes->pRenderer, pRes->pStartTexture, NULL, &pRes->startRect);

            // MULTIPLAYER-Rekt effekt
            if (hoveredButton == 1)
                SDL_SetTextureColorMod(pRes->pMultiplayerTexture, 200, 200, 255);
            else
                SDL_SetTextureColorMod(pRes->pMultiplayerTexture, 255, 255, 255);
            SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerTexture, NULL, &pRes->multiplayerRect);

            // OPTIONS-Rekt effekt
            if (hoveredButton == 2)
                SDL_SetTextureColorMod(pRes->pOptionsTexture, 200, 200, 255);
            else
                SDL_SetTextureColorMod(pRes->pOptionsTexture, 255, 255, 255);
            SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsTexture, NULL, &pRes->optionsRect);

            // EXIT-Rekt effekt
            if (hoveredButton == 3)
                SDL_SetTextureColorMod(pRes->pExitTexture, 200, 200, 255);
            else
                SDL_SetTextureColorMod(pRes->pExitTexture, 255, 255, 255);
            SDL_RenderCopy(pRes->pRenderer, pRes->pExitTexture, NULL, &pRes->exitRect);
        }
        else if (mode == PLAYING)
        {

            SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 255); // svart bakgrund
            SDL_RenderClear(pRes->pRenderer);

            // Din del: Rendera tilemap

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
            }
            // Rendera bilar
            renderCar(pRes->pRenderer, &pRes->car1);
            renderCar(pRes->pRenderer, &pRes->car2);

            // Test: rendera en tile
            SDL_Rect src = getTileSrcByID(2); // tile ID 2 från tileset
            SDL_Rect dest = {400, 300, TILE_SIZE, TILE_SIZE};
            SDL_RenderCopy(pRes->pRenderer, pRes->ptilesetTexture, &src, &dest);
        }
        else if (mode == OPTIONS)
        {
            SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsMenuTex, NULL, NULL);
        }

        // Presentera det som ritats
        SDL_RenderPresent(pRes->pRenderer);
    }
}
