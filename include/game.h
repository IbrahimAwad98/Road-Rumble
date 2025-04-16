#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "car.h"
#include "camera.h" // Include the header file defining Camera

// Konstanter för fönsterstorlek och ljud
#define WIDTH 1366
#define HEIGHT 768
#define AUDIO_FREQ 44100
#define AUDIO_CHANNELS 2
#define AUDIO_CHUNKSIZE 2048
#define NUM_ASPHALT_TILES 90
#define NUM_GRASS_TILES 14
#define NUM_TILES (NUM_ASPHALT_TILES + NUM_GRASS_TILES)

#define TILE_SIZE 128
#define TILESET_COLUMNS 3
#define MAP_WIDTH 11
#define MAP_HEIGHT 6

#include "camera.h"
#include "car.h"

// Struktur som innehåller alla resurser som behövs för spelet
typedef struct
{
    SDL_Window *pWindow;              // Fönster
    SDL_Renderer *pRenderer;          // Renderare
    SDL_Texture *pBackgroundTexture;  // Bakgrundsbild
    SDL_Texture *pStartTexture;       // "Start Game"-textur
    SDL_Texture *pMultiplayerTexture; // "Multiplayer"-textur
    SDL_Texture *pOptionsTexture;     // "Options"-textur
    SDL_Texture *pExitTexture;        // "Quit Game"-textur
    SDL_Texture *pOptionsMenuTex;     // "Options selected"-textur
    SDL_Texture *pMultiplayerMenuTex; // "Multiplayer selected"- textur
    SDL_Texture *pMuteTexture;        // "Mute" textur
    SDL_Texture *pUnmuteTexture;      // "Unmute" textur
    SDL_Texture *pTiles[NUM_TILES];   // Alla separata bilder (90 st)
    SDL_Texture *ptilesetTexture;     // Innehåller hela Tileseten

    TTF_Font *pFont;          // Typsnitt
    Mix_Music *pBgMusic;      // Bakgrundsmusik
    SDL_Rect startRect;       // Rektangel för startknappen
    SDL_Rect exitRect;        // Rektangel för avsluta-knappen
    SDL_Rect multiplayerRect; // Rektangel för multispelare-knappen
    SDL_Rect optionsRect;     // Rektangel för inställning-knappen
    SDL_Rect muteRect;        // Rektangel för mute-knappen

    Car car1, car2;                   // initiera bilar.
    Camera camera1;                   // Kamera för spelare 1
    Camera camera2;                   // Kamera för spelare 2
    int localPlayerID;                // 0 eller 1 – sätts i framtiden via nätverk
    

} GameResources;

// Funktion som kör spelets huvudloop
void gameLoop(GameResources *pRes);

// Funktioner för rendering och kamera
void renderGrassBackground(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int grassTileID, Camera *pCamera);
void renderTrackAndObjects(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int tilemap[MAP_HEIGHT][MAP_WIDTH], Camera *pCamera);
void updateCamera(Camera *pCamera, SDL_Rect *pTarget);

// Funktion som laddar upp bilder till banan på ett enklare sätt.
SDL_Rect getTileSrcByID(int tileID);

#endif
