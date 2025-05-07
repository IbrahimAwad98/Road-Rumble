#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "car.h"
#include "camera.h"
#include "tilemap.h"

//  Spelkonstanter
#define WIDTH 1366           // Fönstrets bredd
#define HEIGHT 768           // Fönstrets höjd
#define AUDIO_FREQ 44100     // Ljudfrekvens i Hz
#define AUDIO_CHANNELS 2     // Antal ljudkanaler (stereo)
#define AUDIO_CHUNKSIZE 2048 // Buffertstorlek för ljud

// Enum: Spellägen
typedef enum
{
    MENU,        // Huvudmenyn
    OPTIONS,     // Inställningsmenyn
    MULTIPLAYER, // Multiplayer-läget
    PLAYING      // Själva spelet pågår
} GameMode;

// Struktur: GameResources
typedef struct
{
    // SDL-fönster och renderare
    SDL_Window *pWindow;     // Huvudfönster
    SDL_Renderer *pRenderer; // Renderare för grafik

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
    SDL_Texture *pBackToMultiTexture;
    SDL_Texture *pEnterGameTexture;
    SDL_Texture *pPlayerIdTexture;

    // Tiletexturer
    SDL_Texture *pTiles[NUM_TILES]; // Enskilda tile-texturer (asfalt + gräs)
    SDL_Texture *ptilesetTexture;   // Hela tileset-bilden

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

    // Multiplayer (kommenterade delar kan användas senare)
    SDL_Rect portRect;
    SDL_Rect joinRect;
    SDL_Rect playerIdRect;
    SDL_Rect backMRect;
    SDL_Rect enterRect;

    // Spelobjekt
    Car *pCar1, *pCar2, *pCar3, *pCar4;      // två bilar
    Camera camera1, camera2; // Två kameror för splitscreen eller nätverk

    // Nätverk
    bool isHosting;
    bool isClient;

    // Multiplayerdata
    int localPlayerID; // ID för lokala spelaren (0 eller 1)

} GameResources;

// Funktionsdeklaration
void gameLoop(GameResources *pRes);

#endif // GAME_H
