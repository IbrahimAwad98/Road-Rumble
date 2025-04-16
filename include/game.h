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
#define TILESET_COLUMNS 3                               // Antal kolumner i tileset-texturen
#define MAP_WIDTH 11                                    // Bredd på tilemap (antal kolumner)
#define MAP_HEIGHT 6                                    // Höjd på tilemap (antal rader)

// Struktur: GameResources
// Håller alla resurser som används under spelet
// Struktur: GameResources
// Innehåller alla resurser som används under spelets gång
typedef struct
{
    // SDL-fönster och renderare
    SDL_Window *pWindow;           // Huvudfönstret för spelet
    SDL_Renderer *pRenderer;       // Renderare för grafik

    // Menytexturer
    SDL_Texture *pBackgroundTexture;      // Bakgrundsbild för menyer
    SDL_Texture *pStartTexture;           // Textur för "Start Game"-knapp
    SDL_Texture *pMultiplayerTexture;     // Textur för "Multiplayer"-knapp
    SDL_Texture *pOptionsTexture;         // Textur för "Options"-knapp
    SDL_Texture *pExitTexture;            // Textur för "Quit Game"-knapp
    SDL_Texture *pOptionsMenuTex;         // Textur för vald inställningsmeny
    SDL_Texture *pMultiplayerMenuTex;     // Textur för vald multiplayer-meny
    SDL_Texture *pMuteTexture;            // Textur för mute-knapp
    SDL_Texture *pUnmuteTexture;          // Textur för unmute-knapp

    // Tileset och individuella tile-texturer
    SDL_Texture *pTiles[NUM_TILES];       // Array av alla enskilda tiles (90 asfalt + 14 gräs)
    SDL_Texture *ptilesetTexture;         // Hela tileset-spritesheeten

    // Typsnitt och musik
    TTF_Font *pFont;               // Typsnitt för menyer och text
    Mix_Music *pBgMusic;          // Bakgrundsmusik

    // Rektanglar för att hantera musinteraktion med menyer
    SDL_Rect startRect;           // Klickyta för "Start Game"
    SDL_Rect exitRect;            // Klickyta för "Quit Game"
    SDL_Rect multiplayerRect;     // Klickyta för "Multiplayer"
    SDL_Rect optionsRect;         // Klickyta för "Options"
    SDL_Rect muteRect;            // Klickyta för ljud av/på

    // Spelobjekt – bilar och kameror
    Car car1, car2;               // Spelarnas bilar
    Camera camera1, camera2;      // Kameror för varje spelare

    int localPlayerID;            // ID för den lokala spelaren (0 eller 1) – används vid nätverksspel

} GameResources;


// Enum: GameMode
// Anger vilket läge spelet är i just nu
typedef enum
{
    MENU,        // Huvudmenyn
    OPTIONS,     // Inställningar
    MULTIPLAYER, // Multiplayerläge
    PLAYING      // Själva spelet pågår
} GameMode;

// Funktionsdeklarationer
void gameLoop(GameResources *pRes);                                                                                             // Startar spel-loopen (huvudfunktionen som kör spelet)
void initGame(GameResources *pRes);                                                                                             // Initierar bilar, kameror m.m.
void handleGlobalKeyEvents(SDL_Event *event, GameMode *mode, bool *isRunning);                                                  // Hanterar tangenttryck som fungerar i alla lägen (t.ex. ESC, P, M)
void handleMenuEvents(SDL_Event *event, GameResources *pRes, GameMode *mode, bool *isRunning, int *hoveredButton);              // Hanterar musinmatning i menyn och ändrar spelläge vid klick
void renderMenu(GameResources *pRes, int hoveredButton);                                                                        // Renderar huvudmenyn och dess knappar
void renderPlaying(GameResources *pRes);                                                                                        // Renderar själva spelet (tiles, bilar, etc.)
void renderOptions(GameResources *pRes);                                                                                        // Renderar inställningsmenyn
void renderMultiplayer(GameResources *pRes);                                                                                    // Renderar multiplayer-menyn
SDL_Rect getTileSrcByID(int tileID);                                                                                            // Returnerar en rektangel från tileset baserat på ett tileID
void renderGrassBackground(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int grassTileID, Camera *pCamera);                    // Renderar gräs över hela spelplanen
void renderTrackAndObjects(SDL_Renderer *pRenderer, SDL_Texture **pTiles, int tilemap[MAP_HEIGHT][MAP_WIDTH], Camera *pCamera); // Renderar vägar och objekt utifrån tilemap

#endif