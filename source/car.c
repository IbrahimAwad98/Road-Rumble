#include <SDL2/SDL_image.h>
#include <math.h>
#include "car.h"
#include "tilemap.h"

// Om konstanten M_PI inte redan är definierad, definiera den manuellt
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Funktion: Laddar in bilens textur, initierar bilens position och egenskaper
bool initCar(SDL_Renderer *pRenderer, Car *pCar, const char *pImagepath, int x, int y, int w, int h)
{
    // Initiera SDL_image med PNG-stöd om det inte redan är gjort
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        SDL_Log("Failed to initialize SDL_image: %s\n", IMG_GetError());
        return false;
    }

    // Ladda in bilden från angiven filväg
    SDL_Surface *pSurface = IMG_Load(pImagepath);
    if (!pSurface)
    {
        SDL_Log("IMG_Load error: %s", IMG_GetError());
        return false;
    }

    // Skapa textur från den inlästa bilden
    pCar->pCartexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface); // Frigör ytan (ej längre behövs)

    if (!pCar->pCartexture)
    {
        SDL_Log("Failed to create car texture: %s\n", SDL_GetError());
        return false;
    }

    // Initiera bilens position och storlek
    pCar->carRect.x = x;
    pCar->carRect.y = y;
    pCar->carRect.w = w;
    pCar->carRect.h = h;

    // Initiera flyttalskoordinater och rörelseegenskaper
    pCar->x = (float)x;
    pCar->y = (float)y;
    pCar->angle = 0.0f;
    pCar->speed = 0.0f;

    return true;
}

// Funktion:  Uppdaterar bilens rörelse, vinkel och position beroende på input
void updateCar(Car *pCar, const Uint8 *pKeys, SDL_Scancode up, SDL_Scancode down, SDL_Scancode left, SDL_Scancode right)
{
    // Konstanter för rörelse
    const float accel = 0.2f;     // Acceleration framåt/bakåt
    const float maxSpeed = 5.0f;  // Maxhastighet framåt
    const float turnSpeed = 3.0f; // Hur snabbt bilen svänger
    const float friction = 0.05f; // Friktion vid avstannande

    // Gasa (framåt)
    if (pKeys[up])
    {
        pCar->speed += accel;
        if (pCar->speed > maxSpeed)
        {
            pCar->speed = maxSpeed;
        }
    }

    // Backa
    if (pKeys[down])
    {
        pCar->speed -= accel;
        if (pCar->speed < -maxSpeed / 2)
        {
            pCar->speed = -maxSpeed / 2;
        }
    }

    // Sväng vänster
    if (pKeys[left])
    {
        pCar->angle -= turnSpeed;
    }

    // Sväng höger
    if (pKeys[right])
    {
        pCar->angle += turnSpeed;
    }

    // Applicera friktion så bilen stannar gradvis
    if (pCar->speed > 0)
    {
        pCar->speed -= friction;
        if (pCar->speed < 0)
        {
            pCar->speed = 0;
        }
    }
    else if (pCar->speed < 0)
    {
        pCar->speed += friction;
        if (pCar->speed > 0)
        {
            pCar->speed = 0;
        }
    }

    // Uppdatera bilens position baserat på riktning (vinkel) och fart
    float radians = pCar->angle * (M_PI / 180.0f); // Omvandla till radianer
    float nextX = pCar->x + pCar->speed * cos(radians);
    float nextY = pCar->y + pCar->speed * sin(radians);

    float checkX = nextX + pCar->carRect.w / 2;
    float checkY = nextY + pCar->carRect.h / 2;

    if (isTileAllowed(checkX, checkY)) {
        pCar->x = nextX;
        pCar->y = nextY;
    } else {
        pCar->speed = 0;
    }

    // Uppdatera renderingsrektangeln
    pCar->carRect.x = (int)pCar->x;
    pCar->carRect.y = (int)pCar->y;

    // Håll bilen inom skärmens gränser
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

    // Justera rektangeln efter ev. begränsning
    pCar->carRect.x = (int)pCar->x;
    pCar->carRect.y = (int)pCar->y;
}

// Funktion: renderCar Renderar bilen på skärmen med rotation
void renderCar(SDL_Renderer *pRenderer, Car *pCar)
{
    SDL_RenderCopyEx(
        pRenderer,
        pCar->pCartexture,
        NULL,
        &pCar->carRect,
        pCar->angle + 90.0f,
        NULL,
        SDL_FLIP_NONE);
}

// Funktion: destroyCar från minnet
void destroyCar(Car *pCar)
{
    if (pCar->pCartexture)
    {
        SDL_DestroyTexture(pCar->pCartexture);
    }
}
