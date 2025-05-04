#include "car.h"
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdlib.h>

struct car {
    SDL_Texture *pCartexture;  // Textur för bilen (grafik)
    SDL_Rect carRect;          // Rektangel för var och hur bilen ritas (position & storlek)
    float x, y;                // Flyttalsposition (exakt bilposition på skärmen)
    float angle;               // Rotationsvinkel i grader
    float speed;               // Hastighet
    int screenW, screenH;      // Skärmbredd och höjd (för att begränsa bilens rörelse)
};

Car *createCar(SDL_Renderer *pRenderer, const char *pImagePath, int x, int y, int w, int h, int screenW, int screenH) {
    // Allokera minne för bilen
    Car *pCar = malloc(sizeof(Car));
    if (!pCar) return NULL;

    // Använd standardbild om ingen sökväg skickats in
    if (!pImagePath) {
        pImagePath = "resources/Cars/car.png";
    }

    // Ladda bilens bild till en SDL_Surface
    SDL_Surface *pSurface = IMG_Load(pImagePath);
    if (!pSurface) {
        SDL_Log("Failed to load image: %s\n", IMG_GetError());
        free(pCar);
        return NULL;
    }

    // Skapa textur från ytan
    pCar->pCartexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);  // Ytan behövs inte längre
    if (!pCar->pCartexture) {
        SDL_Log("Failed to create texture: %s\n", SDL_GetError());
        free(pCar);
        return NULL;
    }

    // Initiera bilens rörelsetillstånd
    pCar->x = (float)x;         // Startposition X (exakt)
    pCar->y = (float)y;         // Startposition Y (exakt)
    pCar->angle = 0.0f;         // Startvinkel
    pCar->speed = 0.0f;         // Startfart

    // Initiera rektangel för renderingen
    pCar->carRect = (SDL_Rect){x, y, w, h};  // Position och storlek

    // Spara skärmstorlek för gränskontroll
    pCar->screenW = screenW;
    pCar->screenH = screenH;

    return pCar;
}


void renderCar(SDL_Renderer *pRenderer, Car *pCar)
{
    if (!pCar || !pCar->pCartexture) return;
    SDL_RenderCopyEx(pRenderer, pCar->pCartexture, NULL, &pCar->carRect, pCar->angle, NULL, SDL_FLIP_NONE);
}
void destroyCar(Car *pCar)
{
    if (!pCar) return;
    if(pCar->pCartexture) SDL_DestroyTexture(pCar->pCartexture);
    free(pCar);
}

void setCarAngle(Car *pCar, float angle) {
    if (pCar) pCar->angle = angle;
}

void setCarSpeed(Car *pCar, float speed) {
    if (pCar) pCar->speed = speed;
}

void setCarX(Car *pCar, float x) {
    if (pCar) {
        pCar->x = x;
        pCar->carRect.x = (int)x;
    }
}

void setCarY(Car *pCar, float y) {
    if (pCar) {
        pCar->y = y;
        pCar->carRect.y = (int)y;
    }
}


float getCarAngle(Car *pCar) {
    if (pCar) return pCar->angle;
    else return 0.0f;
}

float getCarSpeed(Car *pCar){
    if (pCar) return pCar->speed;
    else return 0.0f;
}

float getCarX(Car *pCar) {
    if (pCar) return pCar->x;
    else return 0.0f;
}

float getCarY(Car *pCar) {
    if (pCar) return pCar->y;
    else return 0.0f;
}

void updateCar(Car *pCar, const Uint8 *keys,SDL_Scancode up, SDL_Scancode down, SDL_Scancode left, SDL_Scancode right)
{
    const float accel = 0.2f;       // Acceleration framåt/bakåt
    const float maxSpeed = 5.0f;    // Maxhastighet framåt
    const float turnSpeed = 3.0f;   // Hur snabbt bilen svänger
    const float friction = 0.05f;   // Friktion vid avstannande

    if (keys[up]) {
        pCar->speed += accel;
        if(pCar->speed > maxSpeed) pCar->speed = maxSpeed;
    }
    if (keys[down]) {
        pCar->speed -= accel;
        if(pCar->speed < -maxSpeed / 2) pCar->speed = -maxSpeed / 2;
    }
    if(keys[left]) {
        pCar->angle -= turnSpeed;
    }
    if(keys[right]) {
        pCar->angle += turnSpeed;
    }

    // Håll vinkeln mellan 0–360 grader

    if (pCar->angle >= 360.0f) pCar->angle -= 360.0f;
    if (pCar->angle < 0.0f) pCar->angle += 360.0f;

    //friktion
    if(pCar->speed > 0) {
        pCar->speed -= friction;
        if(pCar->speed < 0) pCar->speed = 0;
    } else if (pCar->speed < 0) {
        pCar->speed += friction;
        if(pCar->speed > 0) pCar->speed = 0;
    }

    //updatera positionen

    pCar->x += pCar->speed * cosf(pCar->angle * M_PI / 180.0f);
    pCar->y += pCar->speed * sinf(pCar->angle * M_PI / 180.0f);

    pCar->carRect.x = (int)pCar->x;
    pCar->carRect.y = (int)pCar->y;

    // Begränsa bilen inom skärmen
    
    int screenWidth = pCar->screenW;   // Bredd på spelfönstret
    int screenHeight = pCar->screenH;  // Höjd på spelfönstret

    // Begränsning X
    if (pCar->x < 0)
        pCar->x = 0;
    else if (pCar->x + pCar->carRect.w > screenWidth)
        pCar->x = screenWidth - pCar->carRect.w;

    // Begränsning Y
    if (pCar->y < 0)
        pCar->y = 0;
    else if (pCar->y + pCar->carRect.h > screenHeight)
        pCar->y = screenHeight - pCar->carRect.h;

    // Uppdatera rektangel för att rita
    pCar->carRect.x = (int)pCar->x;
    pCar->carRect.y = (int)pCar->y;

}

SDL_Rect getCarRect(Car *pCar) {
    if (pCar) return pCar->carRect;
    SDL_Rect empty = {0};
    return empty;
}

void setCarRect(Car *pCar, SDL_Rect rect) {
    if (pCar) pCar->carRect = rect;
}