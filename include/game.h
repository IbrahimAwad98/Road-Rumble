#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

// Konstanter för fönsterstorlek och ljud
#define WIDTH 1366
#define HEIGHT 768
#define AUDIO_FREQ 44100
#define AUDIO_CHANNELS 2
#define AUDIO_CHUNKSIZE 2048

// Struktur som innehåller alla resurser som behövs för spelet
typedef struct
{
    SDL_Window *pWindow;             // Fönster
    SDL_Renderer *pRenderer;         // Renderare
    SDL_Texture *pBackgroundTexture; // Bakgrundsbild
    SDL_Texture *pStartTexture;      // "Start Game"-textur
    SDL_Texture *pExitTexture;       // "Quit Game"-textur
    TTF_Font *pFont;                 // Typsnitt
    Mix_Music *pBgMusic;             // Bakgrundsmusik
    SDL_Rect startRect;              // Rektangel för startknappen
    SDL_Rect exitRect;               // Rektangel för avsluta-knappen
    SDL_Texture *ptilesetTexture;    // Innehåller hela Tileseten
} GameResources;

// Funktion som kör spelets huvudloop
void gameLoop(GameResources *pRes);

SDL_Rect getTileSrcByID(int tileID); // Funktion som laddar upp bilder till banan på ett enklare sätt.

#endif
