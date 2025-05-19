#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdlib.h>
#include "car.h"
#include "tilemap.h"

// definiera den manuellt för windows
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define MAX_TRAIL 30

typedef struct
{
    float x, y;
    float angle;
} TrailMark;

struct Car
{
    SDL_Texture *pCartexture; // Texturen (bilden) för bilen
    SDL_Rect carRect;         // Rektangel som beskriver position och storlek för rendering
    float x, y;               // Exakt position i världen (flyttal för smidigare rörelse)
    float angle;              // Vinkel i grader (rotation)
    float speed;              // Nuvarande fart
    int width;                // bredd
    int height;               // höjd
    bool isDrifting;          // Drift effekt
    TrailMark trail[MAX_TRAIL];
    int trailCount;
};

Car *createCar(SDL_Renderer *pRenderer, const char *pImagepath, int x, int y, int w, int h)
{
    // Allokera minne för en bil
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

    // Initiera bilens rektangel för rendering
    pCar->carRect = (SDL_Rect){x, y, w, h};
    pCar->width = w;
    pCar->height = h;

    // Initiera bilens inre position och rörelse
    pCar->x = (float)x;
    pCar->y = (float)y;
    pCar->angle = 0.0f;
    pCar->speed = 0.0f;
    pCar->isDrifting = false;
    pCar->trailCount = 0;

    return pCar;
}

void updateCar(Car *pCar, const Uint8 *pKeys, SDL_Scancode up, SDL_Scancode down, SDL_Scancode left, SDL_Scancode right, float boost)
{
    const float accel = 0.2f * boost;
    const float maxSpeed = 5.0f * boost;
    const float turnSpeed = 3.0f; // Hur snabbt bilen svänger
    const float friction = 0.05f; // Hur snabbt bilen svänger

    if (pKeys[up])
    {
        pCar->speed += accel;
        if (pCar->speed > maxSpeed)
            pCar->speed = maxSpeed;
    }

    if (pKeys[down])
    {
        pCar->speed -= accel;
        if (pCar->speed < -maxSpeed / 2)
            pCar->speed = -maxSpeed / 2;
    }

    if (pKeys[left])
    {
        pCar->angle -= turnSpeed;
    }
    if (pKeys[right])
    {
        pCar->angle += turnSpeed;
    }

    if ((pKeys[left] || pKeys[right]) && fabs(pCar->speed) > 1.2f)
    {
        pCar->isDrifting = true;
    }
    else
    {
        pCar->isDrifting = false;
    }

    if (pCar->isDrifting && fabs(pCar->speed) > 1.0f)
    {
        float radians = pCar->angle * (M_PI / 180.0f);
        float trailX = pCar->x + pCar->carRect.w / 2 - cos(radians) * 20;
        float trailY = pCar->y + pCar->carRect.h / 2 - sin(radians) * 20;

        if (pCar->trailCount < MAX_TRAIL)
        {
            pCar->trail[pCar->trailCount].x = trailX;
            pCar->trail[pCar->trailCount].y = trailY;
            pCar->trail[pCar->trailCount].angle = pCar->angle;
            pCar->trailCount++;
        }
        else
        {
            for (int i = 1; i < MAX_TRAIL; i++)
            {
                pCar->trail[i - 1] = pCar->trail[i];
            }
            pCar->trail[MAX_TRAIL - 1].x = trailX;
            pCar->trail[MAX_TRAIL - 1].y = trailY;
            pCar->trail[MAX_TRAIL - 1].angle = pCar->angle;
        }
    }
    // Friktion (saktar ner bilen om man släpper knappar)
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
    // Räkna ut nästa position baserat på hastighet och vinkel
    float radians = pCar->angle * (M_PI / 180.0f);
    float nextX = pCar->x + pCar->speed * cos(radians);
    float nextY = pCar->y + pCar->speed * sin(radians);

    // Kolla om nästa position är tillåten (krockkontroll med tilemap)
    float checkX = nextX + pCar->carRect.w / 2;
    float checkY = nextY + pCar->carRect.h / 2;

    bool blockedByObstacle = false;

    int shrink = 12; // shrink width and height by 12px

    SDL_Rect nextRect = {
        (int)nextX + shrink / 2,
        (int)nextY + shrink / 2,
        pCar->carRect.w - shrink,
        pCar->carRect.h - shrink};

    // Loop over tilemap to check if car intersects with crate or barrel
    for (int row = 0; row < MAP_HEIGHT; row++)
    {
        for (int col = 0; col < MAP_WIDTH; col++)
        {
            int tileID = tilemap[row][col];

            if (tileID == 10 || tileID == 11)
            { // Barrel or Crate
                SDL_Rect obstacleRect = {
                    col * TILE_SIZE + (TILE_SIZE - 8) / 4,
                    row * TILE_SIZE + (TILE_SIZE - 8) / 4,
                    8,
                    8};

                if (SDL_HasIntersection(&nextRect, &obstacleRect))
                {
                    blockedByObstacle = true;
                    break;
                }
            }
        }
        if (blockedByObstacle)
            break;
    }

    if (isTileAllowed(checkX, checkY) && !blockedByObstacle)
    {
        pCar->x = nextX;
        pCar->y = nextY;
    }
    // Move slightly back to avoid overlapping obstacle
    else
    {
        pCar->x -= pCar->speed * cos(radians);
        pCar->y -= pCar->speed * sin(radians);

        // Apply bounce effect
        pCar->speed *= -0.6f; // adjust the value to control bounce strength
    }

    // Begränsa så att bilen inte kör utanför skärmen
    int screenWidth = 1366;
    int screenHeight = 768;

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
    pCar->carRect.x = (int)pCar->x;
    pCar->carRect.y = (int)pCar->y;
}
// Rendera
void renderCar(SDL_Renderer *pRenderer, Car *pCar)
{
    SDL_RenderCopyEx(pRenderer, pCar->pCartexture, NULL, &pCar->carRect, pCar->angle + 90.0f, NULL, SDL_FLIP_NONE);
}
// Rensa minnet
void destroyCar(Car *pCar)
{
    if (pCar)
    {
        if (pCar->pCartexture)
        {
            SDL_DestroyTexture(pCar->pCartexture);
        }
        free(pCar);
    }
}
// Getter-funktioner -> extern kod kan läsa värden men inte ändra dem direkt
float getCarX(const Car *pCar) { return pCar->x; }
float getCarY(const Car *pCar) { return pCar->y; }
float getCarAngle(const Car *pCar) { return pCar->angle; }
SDL_Rect getCarRect(const Car *pCar) { return pCar->carRect; }
int getTrailMarkX(const Car *car, int index)
{
    if (!car || index < 0 || index >= MAX_TRAIL)
        return -1;
    return (int)car->trail[index].x;
}

int getTrailMarkY(const Car *car, int index)
{
    if (!car || index < 0 || index >= MAX_TRAIL)
        return -1;
    return (int)car->trail[index].y;
}

int getTrailCount(const Car *car)
{
    if (!car)
        return 0;
    return car->trailCount;
}
float getTrailMarkAngle(const Car *car, int index)
{
    if (!car || index < 0 || index >= MAX_TRAIL)
        return 0.0f;
    return car->trail[index].angle;
}
int getCarWidth(Car *car)
{
    return car->carRect.w;
}

int getCarHeight(Car *car)
{
    return car->carRect.h;
}

// bilpostioner
void setCarPosition(Car *car, float x, float y, float angle)
{
    car->x = x;
    car->y = y;
    car->angle = angle;
    car->carRect.x = (int)x;
    car->carRect.y = (int)y;
}
void setCarAngle(Car *pCar, float angle)
{
    if (pCar)
        pCar->angle = angle;
}
void setCarSpeed(Car *pCar, float speed)
{
    if (pCar)
        pCar->speed = speed;
}
void resolveCollision(Car *pA, Car *pB)
{
    float ax = pA->x + pA->carRect.w / 2;
    float ay = pA->y + pA->carRect.h / 2;
    float bx = pB->x + pB->carRect.w / 2;
    float by = pB->y + pB->carRect.h / 2;

    float dx = ax - bx;
    float dy = ay - by;
    float distance = SDL_sqrtf(dx * dx + dy * dy);

    float minDistance = 29.0f; // Justera detta: bilarnas visuella närhet

    if (distance >= minDistance)
        return; // För långt ifrån, ingen krock

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

    // Kolla att båda bilarna får stå kvar på banan
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
