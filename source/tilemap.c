#include "game.h"
#include "tilemap.h"
#include <stdlib.h>

// Returnerar rätt utdragsrektangel (src rect) från tileset-bilden
SDL_Rect getTileSrcByID(int tileID)
{
    SDL_Rect src;
    src.x = (tileID % TILESET_COLUMNS) * TILE_SIZE; // Kolumn i tileset-bilden
    src.y = (tileID / TILESET_COLUMNS) * TILE_SIZE; // Rad i tileset-bilden
    src.w = TILE_SIZE;                              // Tile-bredd
    src.h = TILE_SIZE;                              // Tile-höjd
    return src;
}

// Tilemap – Definierar själva spelvärlden med hjälp av tile-ID:n.
// Varje heltal representerar ett tileID; -1 betyder tom yta (inget ska ritas).
int tilemap[MAP_HEIGHT][MAP_WIDTH] = {
    {-1, 2, 1, 6, -1, -1, -1, 5, 1, 4, -1},
    {-1, 0, -1, 0, -1, -1, -1, 0, -1, 0, -1},
    {-1, 0, -1, 0, -1, -1, -1, 0, -1, 0, -1},
    {-1, 0, -1, 23, 1, 1, 1, 24, -1, 0, -1},
    {-1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1},
    {-1, 38, 1, 1, 1, 1, 1, 1, 1, 40, -1}};

// Fyller hela bakgrunden med ett gräs-tile för att undvika svarta ytor.
// Används före den riktiga tilemapen ritas ut.
void renderGrassBackground(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int grassTileID)
{
    // Om texturen saknas, gör inget
    if (!pTiles[grassTileID])
    {
        return;
    }

    // Hur många tiles behövs för att täcka hela skärmen?
    int tilesX = WIDTH / TILE_SIZE + 2;
    int tilesY = HEIGHT / TILE_SIZE + 2;

    // Rita ett rutnät av grästiles
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
// Renderar banan (vägar, objekt, etc.) baserat på `tilemap`-arrayen.
// TileID -1 betyder att inget ska ritas.
void renderTrackAndObjects(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int tilemap[MAP_HEIGHT][MAP_WIDTH])
{
    for (int row = 0; row < MAP_HEIGHT; row++)
    {
        for (int col = 0; col < MAP_WIDTH; col++)
        {
            int tileID = tilemap[row][col];

            // Hoppa över tomma tiles (-1)
            if (tileID == -1)
            {
                continue;
            }
            // Rendera bara om tileID är giltigt och motsvarande textur finns
            if (tileID >= 0 && tileID < NUM_TILES && pTiles[tileID])
            {
                SDL_Rect dest = {
                    col * TILE_SIZE,
                    row * TILE_SIZE,
                    TILE_SIZE, TILE_SIZE};

                SDL_RenderCopy(pRenderer, pTiles[tileID], NULL, &dest);
            }
        }
    }
}

bool isTileAllowed(float x, float y)
{
    int col = (int)(x / TILE_SIZE);
    int row = (int)(y / TILE_SIZE);

    if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH)
        return false;

    return tilemap[row][col] != -1;
}
