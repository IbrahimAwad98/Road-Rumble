#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "car.h"
#include "camera.h"

// Spelkonstanter
#define WIDTH 1366                                      // Fönstrets bredd
#define HEIGHT 768                                      // Fönstrets höjd
#define AUDIO_FREQ 44100                                // Ljudfrekvens i Hz
#define AUDIO_CHANNELS 2                                // Antal ljudkanaler (stereo)
#define AUDIO_CHUNKSIZE 2048                            // Buffertstorlek för ljud
#define NUM_ASPHALT_TILES 90                            // Antal asfalt-tiles
#define NUM_GRASS_TILES 14                              // Antal gräs-tiles
#define NUM_TILES (NUM_ASPHALT_TILES + NUM_GRASS_TILES) // Totalt antal tiles
#define TILE_SIZE 128                                   // Storlek på varje tile i pixlar
#define TILESET_COLUMNS 3                               // Antal kolumner i tileset-bilden
#define MAP_WIDTH 11                                    // Bredd på tilemap (antal kolumner)
#define MAP_HEIGHT 6                                    // Höjd på tilemap (antal rader)

// Struktur: GameResources
typedef struct
{
    // SDL-fönster och renderare
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;

    // Menytexturer
    SDL_Texture *pBackgroundTexture;
    SDL_Texture *pStartTexture;
    SDL_Texture *pMultiplayerTexture;
    SDL_Texture *pOptionsTexture;
    SDL_Texture *pExitTexture;
    SDL_Texture *pOptionsMenuTex;
    SDL_Texture *pMultiplayerMenuTex;
    SDL_Texture *pMuteTexture;
    SDL_Texture *pUnmuteTexture;
    SDL_Texture *pBackToMenuTexture;

    // Tiles (alla som separata SDL_Texture)
    SDL_Texture *pTiles[NUM_TILES]; // 104 tiles totalt (90 asfalt + 14 gräs)
    SDL_Texture *ptilesetTexture;   // Komplett tileset-bild

    // Font och musik
    TTF_Font *pFont;     // Typsnitt
    Mix_Music *pBgMusic; // Bakgrundsmusik

    // Rektanglar för menyknappar och UI-element
    SDL_Rect startRect;
    SDL_Rect exitRect;
    SDL_Rect multiplayerRect;
    SDL_Rect optionsRect;
    SDL_Rect muteRect;
    SDL_Rect backRect;
    SDL_Rect musicVolumeRect;
    SDL_Rect SfxRect;

    // Spelobjekt
    Car car1, car2;          // Två bilar
    Camera camera1, camera2; // Två kameror för splitscreen eller nätverk

    // Multiplayerdata
    int localPlayerID; // ID för lokala spelaren (0 eller 1)

} GameResources;

// Enum: GameMode – olika lägen i spelet
typedef enum
{
    MENU,        // Huvudmenyn
    OPTIONS,     // Inställningsmenyn
    MULTIPLAYER, // Multiplayer-läget
    PLAYING      // Själva spelet pågår
} GameMode;

// Funktionsdeklarationer

void gameLoop(GameResources *pRes); // Startar spelets huvudloop
void initGame(GameResources *pRes); // Initierar bilar, kameror m.m.

void handleGlobalKeyEvents(SDL_Event *event, GameMode *mode, bool *isRunning);
// Hanterar tangenttryck som fungerar i alla lägen (t.ex. ESC, P, M)

void handleMenuEvents(SDL_Event *event, GameResources *pRes, GameMode *mode, bool *isRunning, int *hoveredButton);
// Hanterar menyinteraktion via mus och ändrar spelläge vid klick

void renderMenu(GameResources *pRes, int hoveredButton); // Renderar huvudmenyn
void renderOptions(GameResources *pRes);                 // Renderar inställningsmenyn
void renderMultiplayer(GameResources *pRes);             // Renderar multiplayer-menyn
void renderPlaying(GameResources *pRes);                 // Renderar spelet i PLAYING-läge

SDL_Rect getTileSrcByID(int tileID);
// Returnerar en rektangel från tileset baserat på tileID

void renderGrassBackground(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int grassTileID, Camera *pCamera);
// Renderar en bakgrund av gräs med angiven tileID

void renderTrackAndObjects(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int tilemap[MAP_HEIGHT][MAP_WIDTH], Camera *pCamera);
// Renderar vägar och objekt baserat på tilemap

#endif
