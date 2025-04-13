#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>

// Struct som håller kamerans position och storlek
typedef struct {
    int x, y;
    int w, h;
} Camera;

// Uppdatera kamerans position baserat på ett mål
void updateCamera(Camera *pCamera, SDL_Rect *pTarget);

#endif
