#ifndef CAR_H
#define CAR_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include "camera.h"

// Struktur som definierar en bil
typedef struct
{
    SDL_Texture *pCartexture; // Bilens textur (bild)
    SDL_Rect carRect;         // Rektangel för renderingsposition/storlek
    float x, y;               // Bilens exakta position i världen (float för mjuk rörelse)
    float angle;              // Bilens riktning i grader
    float speed;              // Nuvarande hastighet
} Car;

bool initCar(SDL_Renderer *pRenderer, Car *pCar, const char *pImagepath, int x, int y, int w, int h); // Initierar bilen (laddar bild, ställer in position & storlek)
void renderCar(SDL_Renderer *pRenderer, Car *pCar, Camera *pCamera);                                  // Renderar bilen till skärmen, med hänsyn till kamerans position
void destroyCar(Car *pCar);                                                                           // Frigör minne och textur som bilen använder
void updateCar(Car *pCar, const Uint8 *keys);                                                         // Uppdaterar bilens position och rörelse baserat på tangentinmatning

#endif
