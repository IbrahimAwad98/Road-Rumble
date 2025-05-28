#ifndef TILEMAP_H
#define TILEMAP_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "game.h"

typedef struct GameResources GameResources;

// Storlek på tilemapen (i tiles)
#define MAP_WIDTH 11
#define MAP_HEIGHT 6

// Global tilemap: varje ruta innehåller ett tileID (-1 = tom)
extern int tilemap[MAP_HEIGHT][MAP_WIDTH];

SDL_Rect getTileSrcByID(int tileID);                                                                                                                // Källa i tileset för en tile
SDL_Rect getObstacleRect(int col, int row, int tileID);                                                                                             // Rektangel för objekt
void renderGrassBackground(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int grassTileID);                                                         // Fyller bakgrunden med gräs
void renderTrackAndObjects(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int tilemap[MAP_HEIGHT][MAP_WIDTH], int currentLap, GameResources *pRes); // Renderar bana
bool isTileAllowed(float x, float y);                                                                                                               // Returnerar true om bilen får köra här

#endif
