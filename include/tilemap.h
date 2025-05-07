#ifndef TILEMAP_H
#define TILEMAP_H

#include <SDL2/SDL.h>

// Konstanter
#define TILE_SIZE 128        // Storlek på varje tile i pixlar
#define TILESET_COLUMNS 3    // Antal kolumner i tileset-bilden
#define MAP_WIDTH 11         // Bredd på tilemap (antal kolumner)
#define MAP_HEIGHT 6         // Höjd på tilemap (antal rader)
#define NUM_ASPHALT_TILES 90 // Antal asfalt-tiles
#define NUM_GRASS_TILES 14   // Antal gräs-tiles
#define NUM_DECOR_TILES 13   // Antal dekorationer

#define TILE_OFFSET_ASPHALT 0
#define TILE_OFFSET_GRASS (TILE_OFFSET_ASPHALT + NUM_ASPHALT_TILES)
#define TILE_OFFSET_DECOR (TILE_OFFSET_GRASS + NUM_GRASS_TILES)

#define NUM_TILES (NUM_ASPHALT_TILES + NUM_GRASS_TILES + NUM_DECOR_TILES) // Totalt antal tiles

// Global tilemap
extern int tilemap[MAP_HEIGHT][MAP_WIDTH];

// Funktioner
SDL_Rect getTileSrcByID(int tileID);
void renderGrassBackground(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int grassTileID);
void renderTrackAndObjects(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int tilemap[MAP_HEIGHT][MAP_WIDTH]);
bool isTileAllowed(float x, float y);

#endif
