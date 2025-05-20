#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "car.h"
#include "camera.h"
#include "tilemap.h"

// Konstanter
#define WIDTH 1366
#define HEIGHT 768
#define AUDIO_FREQ 44100
#define AUDIO_CHANNELS 2
#define AUDIO_CHUNKSIZE 2048
#define BOOST_FRAME_COUNT 6

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
typedef struct
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

    // Ljud och text
    TTF_Font *pFont;
    Mix_Music *pBgMusic;

    // UI-element (Menyknappar)
    SDL_Rect startRect, exitRect, multiplayerRect, optionsRect, muteRect;

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
    Camera camera1, camera2;

    // Nätverksstatus
    bool isHosting;
    bool isClient;
    int localPlayerID;

} GameResources;

// Startar spelets huvudloop
void gameLoop(GameResources *pRes);

#endif
