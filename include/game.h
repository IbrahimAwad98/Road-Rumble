#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "tilemap.h"
#include "car.h"

// Konstanter
#define WIDTH 1366
#define HEIGHT 768
#define AUDIO_FREQ 44100
#define AUDIO_CHANNELS 2
#define AUDIO_CHUNKSIZE 2048
#define BOOST_FRAME_COUNT 6
#define LIGHTS_FRAME_COUNT 4
#define LIGHTS_SOUND_COUNT 3
// Konstanter
#define TILE_SIZE 128
#define TILESET_COLUMNS 3

#define NUM_ASPHALT_TILES 90
#define NUM_GRASS_TILES 14
#define NUM_DECOR_TILES 15

#define TILE_OFFSET_ASPHALT 0
#define TILE_OFFSET_GRASS (TILE_OFFSET_ASPHALT + NUM_ASPHALT_TILES)
#define TILE_OFFSET_DECOR (TILE_OFFSET_GRASS + NUM_GRASS_TILES)
#define NUM_TILES 126

// Specialtile-ID:n
#define BOOST_FLAME_TILE_ID (TILE_OFFSET_DECOR + 12)
#define BARELL_TILE_ID (TILE_OFFSET_DECOR + 13)
#define CRATE_TILE_ID (TILE_OFFSET_DECOR + 14)
#define START_TILE_ID 42
#define FINISH_TILE_ID 41

// Spellägen
typedef enum
{
    MENU,
    OPTIONS,
    MULTIPLAYER,
    PLAYING
} GameMode;
typedef enum
{
    CLASSIC,
    DARK
} MenuMode;
typedef enum
{
    WASD,
    ARROWS
} ControllerMode;

// Spelresurser
typedef struct GameResources
{
    // Fönster och renderare
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;

    // Menytexturer (Classic)
    SDL_Texture *pBackgroundTexture, *pStartTexture, *pMultiplayerTexture, *pOptionsTexture, *pExitTexture;
    SDL_Texture *pOptionsMenuTex, *pMultiplayerMenuTex;
    SDL_Texture *pMuteTexture, *pUnmuteTexture;
    SDL_Texture *pBackToMenuTexture, *pBackToMultiTexture, *pEnterGameTexture;
    SDL_Texture *pPlayerIdTexture, *pClassicTexture, *pWASDTexture, *pArrowTexture;

    // Menytexturer (Dark)
    SDL_Texture *pBackgroundDarkTexture, *pStartDarkTexture, *pMultiplayerDarkTexture, *pOptionsDarkTexture, *pExitDarkTexture;
    SDL_Texture *pOptionsMenuDarkTex, *pMultiplayerMenuDarkTex;
    SDL_Texture *pMuteDarkTexture, *pUnmuteDarkTexture;
    SDL_Texture *pBackToMenuDarkTexture, *pBackToMultiDarkTexture, *pEnterGameDarkTexture;
    SDL_Texture *pPlayerIdDarkTexture, *pDarkTexture, *pWASDDarkTexture, *pArrowDarkTexture;

    // Spelgrafik
    SDL_Texture *pTireTrailTexture;
    SDL_Texture *pBoostFlameFrames[BOOST_FRAME_COUNT];
    SDL_Texture *pTiles[NUM_TILES];
    SDL_Texture *ptilesetTexture;
    SDL_Texture *pLightsTextureFrames[LIGHTS_FRAME_COUNT];

    // Ljud och text
    TTF_Font *pFont;
    Mix_Music *pBgMusic;
    Mix_Chunk *lightSounds[LIGHTS_SOUND_COUNT];

    // UI-element (Menyknappar)
    SDL_Rect startRect, exitRect, multiplayerRect, optionsRect, muteRect;

    bool raceStarted;
    bool startCountdown;
    bool countdownStarted;
    Uint32 countdownStartTime;
    Uint32 COUNTDOWN_TOTAL_DURATION;

    // UI-element (Options)
    SDL_Rect backRect, backDarkRect;
    SDL_Rect musicVolumeRect, musicVolumeDarkRect;
    SDL_Rect SfxRect, SfxDarkRect;
    SDL_Rect classicRect, darkRect;
    SDL_Rect WASDRect, arrowRect;
    SDL_Rect WASDDarkRect, arrowDarRect;

    // UI-element (Multiplayer)
    SDL_Rect portRect, portDarkRect;
    SDL_Rect joinRect, joinDarkRect;
    SDL_Rect playerIdRect, playerIdDarkRect;
    SDL_Rect backMRect, backMDarkRect;
    SDL_Rect enterRect, enterDarkRect;

    // Spelobjekt
    Car *pCar1, *pCar2, *pCar3, *pCar4;

    // Nätverksstatus
    bool isHosting;
    bool isClient;
    int localPlayerID;

} GameResources;

// Startar spelets huvudloop
void gameLoop(GameResources *pRes);

#endif
