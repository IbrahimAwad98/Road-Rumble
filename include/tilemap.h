#ifndef TILEMAP_H
#define TILEMAP_H

#include <SDL2/SDL.h>
#include <stdbool.h>

// Konstanter
#define TILE_SIZE 128
#define TILESET_COLUMNS 3
#define MAP_WIDTH 11
#define MAP_HEIGHT 6

#define NUM_ASPHALT_TILES 90
#define NUM_GRASS_TILES 14
#define NUM_DECOR_TILES 15

#define TILE_OFFSET_ASPHALT 0
#define TILE_OFFSET_GRASS (TILE_OFFSET_ASPHALT + NUM_ASPHALT_TILES)
#define TILE_OFFSET_DECOR (TILE_OFFSET_GRASS + NUM_GRASS_TILES)
#define NUM_TILES (NUM_ASPHALT_TILES + NUM_GRASS_TILES + NUM_DECOR_TILES)

// Specialtile-ID:n
#define BOOST_FLAME_TILE_ID (TILE_OFFSET_DECOR + 12)
#define BARELL_TILE_ID (TILE_OFFSET_DECOR + 13)
#define CRATE_TILE_ID (TILE_OFFSET_DECOR + 14)
#define START_TILE_ID 42
#define FINISH_TILE_ID 41

// Tilemap-data
extern int tilemap[MAP_HEIGHT][MAP_WIDTH];

// Funktioner
SDL_Rect getTileSrcByID(int tileID);                                                                                           // Källa i tileset för en tile
SDL_Rect getObstacleRect(int col, int row, int tileID);                                                                        // Rektangel för objekt
void renderGrassBackground(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int grassTileID);                                    // Fyller bakgrunden med gräs
void renderTrackAndObjects(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int tilemap[MAP_HEIGHT][MAP_WIDTH], int currentLap); // Renderar bana
bool isTileAllowed(float x, float y);                                                                                          // Returnerar true om bilen får köra här

#endif
