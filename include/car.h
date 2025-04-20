#ifndef CAR_H
#define CAR_H

#include <SDL2/SDL.h>
#include <stdbool.h>

// Struktur för bil
typedef struct
{
    SDL_Texture *pCartexture; // Pekare till bilens textur
    SDL_Rect carRect;         // Rektangel för position och storlek
    float x, y;               // Flyttal för exakt position
    float angle;              // Vinkel
    float speed;              // Hastighet
} Car;

// Initierar bil
bool initCar(SDL_Renderer *pRenderer, Car *pCar, const char *pImagepath, int x, int y, int w, int h);

// Uppdaterar bilens rörelse
void updateCar(Car *pCar, const Uint8 *keys);

// Renderar bil (utan kamera)
void renderCar(SDL_Renderer *pRenderer, Car *pCar, void *unused);

// Frigör bilens resurser
void destroyCar(Car *pCar);

#endif
