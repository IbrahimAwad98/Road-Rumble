#include "game.h"
#include "tilemap.h"
#include <stdlib.h>

// Behövs inte då vi använder enskilda tiles för att bygga banan och inte en stor bild
SDL_Rect getTileSrcByID(int tileID)
{
    SDL_Rect src; // Skapar en rektangel som ska visa vilken del av bilden vi vill rita

    src.x = (tileID % TILESET_COLUMNS) * TILE_SIZE; // Beräknar vilken kolumn i tileset-bilden vi är på (x-position)
    src.y = (tileID / TILESET_COLUMNS) * TILE_SIZE; // Beräknar vilken rad i tileset-bilden vi är på (y-position)
    src.w = TILE_SIZE;                              // Sätter bredden på rutan till TILE_SIZE
    src.h = TILE_SIZE;                              // Sätter höjden på rutan till TILE_SIZE
    
    return src; // Returnerar rutan som visar vilken del av bilden vi vill rita
}

// Varje heltal representerar ett tileID; -1 betyder tom yta (inget ska ritas).
int tilemap[MAP_HEIGHT][MAP_WIDTH] = {
    {105, 2, 1, 6, 104, -1, 104, 5, 1, 4, 105},
    {-1, 0, 115, 0, -1, -1, -1, 0, 115, 0, -1},
    {-1, 0, -1, 0, 104, 112, 104, 0, -1, 0, -1},
    {-1, 0, 115, 23, 1, 1, 1, 24, 115, 0, -1},
    {-1, 41, 113, -1, -1, -1, -1, -1, 113, 0, -1},
    {104, 38, 1, 1, 1, 1, 1, 1, 1, 40, 104}};


// Gör hela bakgrunden till gräs
void renderGrassBackground(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int grassTileID)
{
    // Om bilden för gräset saknas, gör ingenting
    if (!pTiles[grassTileID])
    {
        return; // Avsluta funktionen
    }

    // Beräkna hur många tiles som behövs för att täcka hela skärmen i x- och y-led
    int tilesX = WIDTH / TILE_SIZE + 2; // +2 för att täcka lite extra utanför skärmen (om kameran rör sig)
    int tilesY = HEIGHT / TILE_SIZE + 2;

    // Rita ett rutnät av grästiles
    // Loopa igenom alla rader
    for (int row = 0; row < tilesY; row++)
    {
        // Loopa igenom alla kolumner
        for (int col = 0; col < tilesX; col++)
        {
            SDL_Rect dest = {
                col * TILE_SIZE, // x-position: kolumn * TILE_SIZE (t.ex. 0, 128, 256 ...) räknar ut exakt x-position i pixlar
                row * TILE_SIZE, // y-position: rad * TILE_SIZE räknar ut exakt y-position i pixlar
                TILE_SIZE, TILE_SIZE}; // Bredd och höjd på varje tile

            SDL_RenderCopy(pRenderer, pTiles[grassTileID], NULL, &dest);  // Ritar ut grästilen på rätt plats på skärmen
        }
    }
}

// Renderar banan (vägar, objekt, etc.) baserat på `tilemap`-arrayen.
// TileID -1 betyder att inget ska ritas.
void renderTrackAndObjects(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int tilemap[MAP_HEIGHT][MAP_WIDTH])
{
    // Gå igenom varje rad i tilemap
    for (int row = 0; row < MAP_HEIGHT; row++)
    {
        // Gå igenom varje kolumn i tilemap
        for (int col = 0; col < MAP_WIDTH; col++)
        {
            int tileID = tilemap[row][col]; // Hämta tileID för den här rutan

            // Hoppa över tomma tiles (-1)
            if (tileID == -1)
            {
                continue;
            }
              // Om tileID är giltigt (0 till NUM_TILES-1) och texturen finns
            if (tileID >= 0 && tileID < NUM_TILES && pTiles[tileID])
            {
                SDL_Rect dest = {
                    col * TILE_SIZE, // x-position: kolumn * TILE_SIZE
                    row * TILE_SIZE, // y-position: rad * TILE_SIZE
                    TILE_SIZE, TILE_SIZE}; // Bredd och höjd på varje tile

                SDL_RenderCopy(pRenderer, pTiles[tileID], NULL, &dest); // Ritar ut rätt tile på rätt plats
            }
        }
    }
}

bool isTileAllowed(float x, float y) // Returnerar true om en position (x, y) är på en tillåten tile (väg), annars false
{
    int col = (int)(x / TILE_SIZE); // Räkna ut vilken kolumn i tilemap vi är i (utifrån x-position)
    int row = (int)(y / TILE_SIZE); // Räkna ut vilken rad i tilemap vi är i (utifrån y-position)

    // Om positionen är utanför tilemapens gränser, returnera false
    if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH) 
        return false;

    int tileID = tilemap[row][col]; // Hämta tileID på den aktuella positionen

    // Tillåt bara specifika vägtiles
    switch (tileID)
    {
        case 0:
        case 1:
        case 2:
        case 4:
        case 5:
        case 6:
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

