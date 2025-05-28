#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdlib.h>
#include "car.h"
#include "tilemap.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_TRAIL 30

// Struktur för en trail-markering (för driftingeffekt)
typedef struct
{
    float x, y;
    float angle;
} TrailMark;

// Struktur för bilen
struct Car
{
    SDL_Texture *pCartexture;   // Textur för rendering
    SDL_Rect carRect;           // Rektangel för position och storlek
    float x, y;                 // Position i världen
    float angle;                // Rotation i grader
    float speed;                // Nuvarande hastighet
    int width, height;          // Storlek
    bool isDrifting;            // Om bilen driftar
    TrailMark trail[MAX_TRAIL]; // Trail-markeringar
    int trailCount;             // Antal trail-markeringar
};

// Skapar och initierar en bil
Car *createCar(SDL_Renderer *pRenderer, const char *pImagepath, int x, int y, int w, int h)
{
    Car *pCar = malloc(sizeof(Car));
    if (!pCar)
        return NULL;

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        SDL_Log("Failed to initialize SDL_image: %s\n", IMG_GetError());
        free(pCar);
        return NULL;
    }

    SDL_Surface *pSurface = IMG_Load(pImagepath);
    if (!pSurface)
    {
        SDL_Log("IMG_Load error: %s", IMG_GetError());
        free(pCar);
        return NULL;
    }

    pCar->pCartexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);

    if (!pCar->pCartexture)
    {
        SDL_Log("Failed to create car texture: %s\n", SDL_GetError());
        free(pCar);
        return NULL;
    }

    pCar->carRect = (SDL_Rect){x, y, w, h};
    pCar->x = x;
    pCar->y = y;
    pCar->width = w;
    pCar->height = h;
    pCar->angle = 0.0f;
    pCar->speed = 0.0f;
    pCar->isDrifting = false;
    pCar->trailCount = 0;

    return pCar;
}

// Uppdaterar bilens rörelse och logik
void updateCar(Car *pCar, const Uint8 *pKeys, SDL_Scancode up, SDL_Scancode down, SDL_Scancode left, SDL_Scancode right, float boost)
{
    const float accel = 0.2f * boost;
    const float maxSpeed = 5.0f * boost;
    const float turnSpeed = 3.0f;
    const float friction = 0.05f;

    if (pKeys[up])
    {
        pCar->speed += accel;
        if (pCar->speed > maxSpeed)
            pCar->speed = maxSpeed;
    }
    else if (pKeys[down])
    {
        pCar->speed -= accel;
        if (pCar->speed < -maxSpeed / 2)
            pCar->speed = -maxSpeed / 2;
    }

    if (pKeys[left])
        pCar->angle -= turnSpeed;
    if (pKeys[right])
        pCar->angle += turnSpeed;

    // Drift-logik
    pCar->isDrifting = (pKeys[left] || pKeys[right]) && fabs(pCar->speed) > 1.2f;

    float radians = pCar->angle * (M_PI / 180.0f);
    float nextX = pCar->x + pCar->speed * cos(radians);
    float nextY = pCar->y + pCar->speed * sin(radians);
    float checkX = nextX + pCar->carRect.w / 2;
    float checkY = nextY + pCar->carRect.h / 2;

    // Hinderdetektering
    SDL_Rect nextRect = {
        (int)nextX + 6,
        (int)nextY + 6,
        pCar->carRect.w - 12,
        pCar->carRect.h - 12};

    bool blockedByObstacle = false;
    for (int row = 0; row < MAP_HEIGHT && !blockedByObstacle; row++)
    {
        for (int col = 0; col < MAP_WIDTH; col++)
        {
            int tileID = tilemap[row][col];
            if (tileID == 10 || tileID == 11)
            {
                SDL_Rect obstacleRect = {
                    col * TILE_SIZE + (TILE_SIZE - 8) / 4,
                    row * TILE_SIZE + (TILE_SIZE - 8) / 4,
                    8, 8};
                if (SDL_HasIntersection(&nextRect, &obstacleRect))
                {
                    blockedByObstacle = true;
                    break;
                }
            }
        }
    }
    // Om tillåten körning: uppdatera position, annars bromsa bakåt
    if (isTileAllowed(checkX, checkY) && !blockedByObstacle)
    {
        pCar->x = nextX;
        pCar->y = nextY;
    }
    else
    {
        pCar->x -= pCar->speed * cos(radians);
        pCar->y -= pCar->speed * sin(radians);
        pCar->speed *= -0.6f;
    }
    // Trail-effekt (drifting)
    if (pCar->isDrifting && fabs(pCar->speed) > 1.0f)
    {
        float trailX = pCar->x + pCar->carRect.w / 2 - cos(radians) * 20;
        float trailY = pCar->y + pCar->carRect.h / 2 - sin(radians) * 20;
        if (pCar->trailCount < MAX_TRAIL)
        {
            pCar->trail[pCar->trailCount++] = (TrailMark){trailX, trailY, pCar->angle};
        }
        else
        {
            for (int i = 1; i < MAX_TRAIL; i++)
                pCar->trail[i - 1] = pCar->trail[i];
            pCar->trail[MAX_TRAIL - 1] = (TrailMark){trailX, trailY, pCar->angle};
        }
    }
    // Friktion
    if (pCar->speed > 0)
        pCar->speed = fmax(0, pCar->speed - friction);
    else if (pCar->speed < 0)
        pCar->speed = fmin(0, pCar->speed + friction);
    // Begränsa till skärm
    pCar->x = fmax(0, fmin(pCar->x, 1366 - pCar->carRect.w));
    pCar->y = fmax(0, fmin(pCar->y, 768 - pCar->carRect.h));
    pCar->carRect.x = (int)pCar->x;
    pCar->carRect.y = (int)pCar->y;
}

// Kollision mellan två bilar – puttar isär dem om för nära
void resolveCollision(Car *pA, Car *pB)
{
    float ax = pA->x + pA->carRect.w / 2;
    float ay = pA->y + pA->carRect.h / 2;
    float bx = pB->x + pB->carRect.w / 2;
    float by = pB->y + pB->carRect.h / 2;

    float dx = ax - bx;
    float dy = ay - by;
    float distance = SDL_sqrtf(dx * dx + dy * dy);

    float minDistance = 29.0f;

    if (distance >= minDistance)
        return;

    float overlap = minDistance - distance;
    if (distance == 0.0f)
        distance = 1.0f;

    dx /= distance;
    dy /= distance;

    float push = overlap / 2.0f;

    float newAx = pA->x + dx * push;
    float newAy = pA->y + dy * push;
    float newBx = pB->x - dx * push;
    float newBy = pB->y - dy * push;

    if (isTileAllowed(newAx + pA->carRect.w / 2, newAy + pA->carRect.h / 2) &&
        isTileAllowed(newBx + pB->carRect.w / 2, newBy + pB->carRect.h / 2))
    {
        pA->x = newAx;
        pA->y = newAy;
        pB->x = newBx;
        pB->y = newBy;

        pA->carRect.x = (int)pA->x;
        pA->carRect.y = (int)pA->y;
        pB->carRect.x = (int)pB->x;
        pB->carRect.y = (int)pB->y;

        pA->speed *= 0.7f;
        pB->speed *= 0.7f;
    }
}

// Tvingar en trail-markering att läggas till (om bilen driftar)
void addTrailIfDrifting(Car *pCar)
{
    if (!pCar || !pCar->isDrifting || fabs(pCar->speed) <= 1.0f)
        return;

    float radians = pCar->angle * (M_PI / 180.0f);
    float trailX = pCar->x + pCar->carRect.w / 2 - cos(radians) * 20;
    float trailY = pCar->y + pCar->carRect.h / 2 - sin(radians) * 20;

    if (pCar->trailCount < MAX_TRAIL)
    {
        pCar->trail[pCar->trailCount++] = (TrailMark){trailX, trailY, pCar->angle};
    }
    else
    {
        for (int i = 1; i < MAX_TRAIL; i++)
            pCar->trail[i - 1] = pCar->trail[i];
        pCar->trail[MAX_TRAIL - 1] = (TrailMark){trailX, trailY, pCar->angle};
    }
}

// Renderar bilen med rotation
void renderCar(SDL_Renderer *pRenderer, Car *pCar)
{
    SDL_RenderCopyEx(pRenderer, pCar->pCartexture, NULL, &pCar->carRect, pCar->angle + 90.0f, NULL, SDL_FLIP_NONE);
}

// Frigör minnet
void destroyCar(Car *pCar)
{
    if (!pCar)
        return;
    if (pCar->pCartexture)
        SDL_DestroyTexture(pCar->pCartexture);
    free(pCar);
}

// Getters
float getCarX(const Car *p) { return p->x; }
float getCarY(const Car *p) { return p->y; }
float getCarAngle(const Car *p) { return p->angle; }
float getCarSpeed(const Car *p) { return p->speed; }
int getCarWidth(Car *p) { return p->width; }
int getCarHeight(Car *p) { return p->height; }
SDL_Rect getCarRect(const Car *p) { return p->carRect; }
bool isCarDrifting(const Car *p) { return p && p->isDrifting; }
int getTrailCount(const Car *p) { return p ? p->trailCount : 0; }
int getTrailMarkX(const Car *p, int i) { return (p && i >= 0 && i < MAX_TRAIL) ? (int)p->trail[i].x : -1; }
int getTrailMarkY(const Car *p, int i) { return (p && i >= 0 && i < MAX_TRAIL) ? (int)p->trail[i].y : -1; }
float getTrailMarkAngle(const Car *p, int i) { return (p && i >= 0 && i < MAX_TRAIL) ? p->trail[i].angle : 0.0f; }

// Setters
void setCarPosition(Car *p, float x, float y, float angle)
{
    p->x = x;
    p->y = y;
    p->angle = angle;
    p->carRect.x = (int)x;
    p->carRect.y = (int)y;
}
void setCarAngle(Car *p, float a) { if (p) p->angle = a; }
void setCarSpeed(Car *p, float s) { if (p) p->speed = s; }
void setCarDrifting(Car *p, bool d) { if (p) p->isDrifting = d; }