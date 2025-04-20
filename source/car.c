#include <SDL2/SDL_image.h>
#include <math.h>
#include "camera.h"
#include "car.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

bool initCar(SDL_Renderer *pRenderer, Car *pCar, const char *pImagepath, int x, int y, int w, int h)
{
    // Säkerställ att SDL_image är initierad med stöd för PNG
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        SDL_Log("Failed to initialize SDL_image: %s\n", IMG_GetError());
        return false;
    }

    SDL_Surface *pSurface = IMG_Load(pImagepath);
    if (!pSurface)
    {
        SDL_Log("IMG_Load error: %s", IMG_GetError());
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

    // Initiera bilens position och egenskaper
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
// bilen rörelse
void updateCar(Car *pCar, const Uint8 *keys)
{
    // Hur snabbt bilen accelererar när spelaren trycker på gas (UP).
    // Ett högre värde gör att bilen når maxhastighet snabbare, men kan kännas ryckig.
    const float accel = 0.2f;

    // Den högsta hastigheten bilen får uppnå i någon riktning.
    // Används som säkerhet för att bilen inte ska gå för snabbt.
    const float maxSpeed = 5.0f;

    // Hur snabbt bilen roterar (vinkeländring) när man trycker vänster eller höger.
    // Ett högre värde gör bilen mer känslig vid styrning.
    const float turnSpeed = 3.0f;

    // Friktionens styrka – hur snabbt bilen bromsas naturligt när du släpper gasen.
    // Ett högre värde gör att bilen stannar snabbare, ett lägre gör att den glider längre.
    const float friction = 0.05f;

    // Gasa
    if (keys[SDL_SCANCODE_UP])
    {
        pCar->speed += accel;
        if (pCar->speed > maxSpeed)
        {
            pCar->speed = maxSpeed;
        }
    }
    // Backa
    if (keys[SDL_SCANCODE_DOWN])
    {
        pCar->speed -= accel;
        if (pCar->speed < -maxSpeed / 2)
        {
            pCar->speed = -maxSpeed / 2;
        }
    }
    // Sväng vänster
    if (keys[SDL_SCANCODE_LEFT])
    {
        pCar->angle -= turnSpeed;
    }
    // Sväng höger
    if (keys[SDL_SCANCODE_RIGHT])
    {
        pCar->angle += turnSpeed;
    }
    // Applicera friktion
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

    // Uppdatera bilens position utifrån vinkel och hastighet
    float radians = pCar->angle * (M_PI / 180.0f);
    pCar->x += pCar->speed * cos(radians);
    pCar->y += pCar->speed * sin(radians);

    pCar->carRect.x = (int)pCar->x;
    pCar->carRect.y = (int)pCar->y;

    // Begränsa bilen inom skärmen
    int screenWidth = 1280;
    int screenHeight = 720;
    if (pCar->x < 0)
    {
        pCar->x = 0;
    }
    else if (pCar->x + pCar->carRect.w > screenWidth)
    {
        pCar->x = screenWidth - pCar->carRect.w;
    }

    if (pCar->y < 0)
    {
        pCar->y = 0;
    }
    else if (pCar->y + pCar->carRect.h > screenHeight)
    {
        pCar->y = screenHeight - pCar->carRect.h;
    }

    // Uppdatera rektangeln efter begränsning
    pCar->carRect.x = (int)pCar->x;
    pCar->carRect.y = (int)pCar->y;
}
// Rendera bilen på skärmen i förhållande till kameran
void renderCar(SDL_Renderer *pRenderer, Car *pCar, Camera *pCamera)
{
    SDL_Rect dest = {
        pCar->carRect.x - pCamera->x, // position relativ till kameran
        pCar->carRect.y - pCamera->y,
        pCar->carRect.w,
        pCar->carRect.h};

    SDL_RenderCopyEx(pRenderer, pCar->pCartexture, NULL, &dest, pCar->angle, NULL, SDL_FLIP_NONE);
}
// städa up (för minnet)
void destroyCar(Car *pCar)
{
    if (pCar->pCartexture)
    {
        SDL_DestroyTexture(pCar->pCartexture);
    }
}
