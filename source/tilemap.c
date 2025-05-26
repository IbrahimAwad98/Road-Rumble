#include "game.h"
#include "tilemap.h"
#include <stdlib.h>

// Tileset-källa (används ej i dagsläget)
SDL_Rect getTileSrcByID(int tileID)
{
    SDL_Rect src;
    src.x = (tileID % TILESET_COLUMNS) * TILE_SIZE;
    src.y = (tileID / TILESET_COLUMNS) * TILE_SIZE;
    src.w = TILE_SIZE;
    src.h = TILE_SIZE;
    return src;
}

// Returnerar rektangel för hinderobjekt (barrel/crate)
SDL_Rect getObstacleRect(int col, int row, int tileID)
{
    SDL_Rect rect = {0, 0, 0, 0};

    if (tileID == 10 || tileID == 11)
    {
        rect.x = col * TILE_SIZE + (TILE_SIZE - 48) / 6;
        rect.y = row * TILE_SIZE + (TILE_SIZE - 48) / 6;
        rect.w = 48;
        rect.h = 48;
    }

    return rect;
}

// Tilemap: -1 = tom, 0+ = tile-ID
int tilemap[MAP_HEIGHT][MAP_WIDTH] = {
    {109, 2, 1, 6, 104, -1, 104, 5, 1, 4, 105},
    {-1, 0, -1, 0, -1, -1, -1, 0, 107, 0, -1},
    {115, 0, -1, 10, 104, 112, 104, 0, -1, 0, -1},
    {115, 0, 119, 23, 1, 1, 1, 24, 114, 0, -1},
    {-1, 8, -1, -1, -1, -1, -1, -1, 106, 0, -1},
    {110, 38, 7, 1, 1, 1, 11, 1, 9, 40, 110}};

// Rendera gräsbakgrund
void renderGrassBackground(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int grassTileID)
{
    if (!pTiles[grassTileID])
        return;

    int tilesX = WIDTH / TILE_SIZE + 2;
    int tilesY = HEIGHT / TILE_SIZE + 2;

    for (int row = 0; row < tilesY; row++)
    {
        for (int col = 0; col < tilesX; col++)
        {
            SDL_Rect dest = {
                col * TILE_SIZE,
                row * TILE_SIZE,
                TILE_SIZE, TILE_SIZE};

            SDL_RenderCopy(pRenderer, pTiles[grassTileID], NULL, &dest);
        }
    }
}

// Rendera vägbana och objekt från tilemap
void renderTrackAndObjects(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int tilemap[MAP_HEIGHT][MAP_WIDTH], int currentLap, GameResources *pRes)
{
    for (int row = 0; row < MAP_HEIGHT; row++)
    {
        for (int col = 0; col < MAP_WIDTH; col++)
        {
            int tileID = tilemap[row][col];
            if (tileID == -1)
                continue;

            if (tileID >= 0 && tileID < NUM_TILES && pTiles[tileID])
            {
                SDL_Rect dest = {
                    col * TILE_SIZE,
                    row * TILE_SIZE,
                    TILE_SIZE, TILE_SIZE};

                SDL_RenderCopy(pRenderer, pTiles[tileID], NULL, &dest);

                // Specialeffekter
                if (tileID == 9 && pTiles[BOOST_FLAME_TILE_ID] && currentLap == 2)
                {
                    SDL_Rect small = {
                        dest.x + TILE_SIZE / 4,
                        dest.y + TILE_SIZE / 4,
                        TILE_SIZE / 2,
                        TILE_SIZE / 2};
                    SDL_RenderCopy(pRenderer, pTiles[BOOST_FLAME_TILE_ID], NULL, &small);
                }

                if (tileID == 7)
                {
                    SDL_Rect small = {
                        dest.x + TILE_SIZE * 2 / 3,
                        dest.y,
                        TILE_SIZE / 3,
                        TILE_SIZE};
                    SDL_RenderCopy(pRenderer, pTiles[FINISH_TILE_ID], NULL, &small);
                }

                if (tileID == 8)
                {
                    SDL_Rect small = {
                        dest.x,
                        dest.y + (TILE_SIZE - 24) / 2,
                        TILE_SIZE,
                        24};
                    SDL_RenderCopy(pRenderer, pTiles[START_TILE_ID], NULL, &small);
                }

                if (tileID == 10)
                {
                    SDL_Rect small1 = getObstacleRect(col, row, tileID);
                    SDL_RenderCopy(pRenderer, pTiles[BARELL_TILE_ID], NULL, &small1);
                }

                if (tileID == 11)
                {
                    SDL_Rect small = getObstacleRect(col, row, tileID);
                    SDL_RenderCopy(pRenderer, pTiles[CRATE_TILE_ID], NULL, &small);
                }
                if (tileID == 119 && pRes->countdownStarted)
                {
                    Uint32 elapsed = SDL_GetTicks() - pRes->countdownStartTime;
                    int frame = elapsed / 1000; // 0–3
                    if (frame >= LIGHTS_FRAME_COUNT)
                        frame = LIGHTS_FRAME_COUNT - 1;

                    // Static to keep track of last played sound
                    static int previousFrame = -1;

                    if (frame != previousFrame)
                    {
                        previousFrame = frame;

                        // Play sounds only for frames 1, 2, 3 (skip frame 0 – all lights off)
                        if (frame >= 1 && frame <= 3 && pRes->lightSounds[frame - 1])
                        {
                            Mix_PlayChannel(-1, pRes->lightSounds[frame - 1], 0);
                        }
                    }

                    // Render light sprite
                    SDL_Rect small = {
                        dest.x,
                        dest.y - (128 - TILE_SIZE) / 2,
                        128, 128};
                    SDL_RenderCopy(pRenderer, pTiles[120 + frame], NULL, &small);
                }
            }
        }
    }
}

// Kollar om en tile tillåter körning
bool isTileAllowed(float x, float y)
{
    int col = (int)(x / TILE_SIZE);
    int row = (int)(y / TILE_SIZE);

    if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH)
        return false;

    int tileID = tilemap[row][col];

    switch (tileID)
    {
    case 0:
    case 1:
    case 2:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 23:
    case 24:
    case 38:
    case 40:
    case 41:
        return true;
    default:
        return false;
    }
}
