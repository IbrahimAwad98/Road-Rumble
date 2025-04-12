#include "car.h"
#include <SDL2/SDL_image.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14
#endif

bool initiCar(SDL_Renderer *pRenderer, Car *pCar, const char *pImagepath, int x, int y, int w, int h)
{
    SDL_Surface *pSurface = IMG_Load(pImagepath);
    if (!pSurface)
    {
        return false;
    }
    pCar->pCartexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);
    // hantera felet.
    if (!pCar->pCartexture)
    {
        SDL_Log("Failed to create car texture: %s\n", SDL_GetError());
        return false;
    }

    pCar->carRect.x = x; // Var bilen ritas horisontellt (vänster/höger)
    pCar->carRect.y = y; // Var bilen ritas vertikalt (upp/ner)
    pCar->carRect.w = w; // bredd
    pCar->carRect.h = h; // höjd
    pCar->x = (float)x;
    pCar->y = (float)y;
    pCar->angle = 0.0f;
    pCar->speed = 0.0f;

    return true;
}
// rendera bilen
void renderCar(SDL_Renderer *pRenderer, Car *pCar)
{
    SDL_RenderCopyEx(pRenderer, pCar->pCartexture, NULL, &pCar->carRect, pCar->angle, NULL, SDL_FLIP_NONE);
}
// städa up (för minnet)
void destroyCar(Car *pCar)
{
    SDL_DestroyTexture(pCar->pCartexture);
}
// bilen rörelse
void updateCar(Car *pCar, const Uint8 *keys)
{
    const float accel = 0.2f;
    const float maxSpeed = 5.0f;
    const float turnSpeed = 3.0f;
    const float friction = 0.05f;

    if (keys[SDL_SCANCODE_UP])
    {
        pCar->speed += accel;
        if (pCar->speed > maxSpeed)
        {
            pCar->speed = maxSpeed;
        }
    }
    if (keys[SDL_SCANCODE_DOWN])
    {
        pCar->speed -= accel;
        if (pCar->speed < -maxSpeed / 2)
        {
            pCar->speed = -maxSpeed / 2;
        }
    }
    if (keys[SDL_SCANCODE_LEFT])
    {
        pCar->angle -= turnSpeed;
    }
    if (keys[SDL_SCANCODE_RIGHT])
    {
        pCar->angle += turnSpeed;
    }
    // friktion
    if (pCar->speed > 0)
    {
        pCar->speed -= friction;
        if (pCar->speed < 0)
            pCar->speed = 0;
    }
    else if (pCar->speed < 0)
    {
        pCar->speed += friction;
        if (pCar->speed > 0)
            pCar->speed = 0;
    }

    // updatera positionen
    pCar->x += pCar->speed * cos(pCar->angle * M_PI / 180.0f);
    pCar->y += pCar->speed * sin(pCar->angle * M_PI / 180.0f);

    pCar->carRect.x = (int)pCar->x;
    pCar->carRect.y = (int)pCar->y;

    // Begränsa bilen inom skärmen
    int screenWidth = 1280; // Byt till din fönsterbredd
    int screenHeight = 720; // Byt till din fönsterhöjd

    // Begränsning X
    if (pCar->x < 0)
    {
        pCar->x = 0;
    }
    else if (pCar->x + pCar->carRect.w > screenWidth)
    {
        pCar->x = screenWidth - pCar->carRect.w;
    }

    // Begränsning Y
    if (pCar->y < 0)
    {
        pCar->y = 0;
    }
    else if (pCar->y + pCar->carRect.h > screenHeight)
    {
        pCar->y = screenHeight - pCar->carRect.h;
    }

    // Uppdatera rektangel för att rita
    pCar->carRect.x = (int)pCar->x;
    pCar->carRect.y = (int)pCar->y;
}