#include "game.h"
#include "tilemap.h"

// Returnerar rätt utdrag (src rect) från tileset för angiven tileID
// Används när man ska rendera en specifik tile från tilesetbilden
SDL_Rect getTileSrcByID(int tileID)
{
    SDL_Rect src;
    src.x = (tileID % TILESET_COLUMNS) * TILE_SIZE; // kolumn i tileset
    src.y = (tileID / TILESET_COLUMNS) * TILE_SIZE; // rad i tileset
    src.w = TILE_SIZE;
    src.h = TILE_SIZE;
    return src;
}

// Själva tilemappen – 2D-array där varje värde representerar en tileID
// -1 = tom (ingen tile)
// Definierar spelvärldens tilemap med ID:n, -1 = tom eller täckt
int tilemap[MAP_HEIGHT][MAP_WIDTH] = {
    {-1, 2, 1, 6, -1, -1, -1, 5, 1, 4, -1},
    {-1, 0, -1, 0, -1, -1, -1, 0, -1, 0, -1},
    {-1, 0, -1, 0, -1, -1, -1, 0, -1, 0, -1},
    {-1, 0, -1, 23, 1, 1, 1, 24, -1, 0, -1},
    {-1, 0, -1, -1, -1, -1, -1, -1, -1, 0, -1},
    {-1, 38, 1, 1, 1, 1, 1, 1, 1, 40, -1}};

// Renderar hela bakgrunden med ett gräs-tile
void renderGrassBackground(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int grassTileID, Camera *pCamera)
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
                col * TILE_SIZE - (pCamera->x % TILE_SIZE),
                row * TILE_SIZE - (pCamera->y % TILE_SIZE),
                TILE_SIZE, TILE_SIZE};

            SDL_RenderCopy(pRenderer, pTiles[grassTileID], NULL, &dest);
        }
    }
}

// Renderar bana + objekt enligt tilemap, hoppar över tomma tiles
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
                continue; // skippar "tomma" rutor
            }

            // Renderar tile om ID är giltigt och textur finns
            if (tileID >= 0 && tileID < NUM_TILES && pTiles[tileID])
            {
                SDL_RenderCopy(pRenderer, pTiles[tileID], NULL, &dest);
            }
        }
    }
}