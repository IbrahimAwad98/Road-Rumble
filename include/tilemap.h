#ifndef TILEMAP_H
#define TILEMAP_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "game.h"

typedef struct GameResources GameResources;

#define MAP_WIDTH 11
#define MAP_HEIGHT 6

// Tilemap-data
extern int tilemap[MAP_HEIGHT][MAP_WIDTH];

// Funktioner
SDL_Rect getTileSrcByID(int tileID);                                                                                                                // Källa i tileset för en tile
SDL_Rect getObstacleRect(int col, int row, int tileID);                                                                                             // Rektangel för objekt
void renderGrassBackground(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int grassTileID);                                                         // Fyller bakgrunden med gräs
void renderTrackAndObjects(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int tilemap[MAP_HEIGHT][MAP_WIDTH], int currentLap, GameResources *pRes); // Renderar bana
bool isTileAllowed(float x, float y);                                                                                                               // Returnerar true om bilen får köra här

#endif
