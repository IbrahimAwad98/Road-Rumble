#include "car.h"
#include <SDL2/SDL_image.h>

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
    return true;
}
void renderCar(SDL_Renderer *pRenderer, Car *pCar)
{
    SDL_RenderCopy(pRenderer, pCar->pCartexture, NULL, &pCar->carRect);
}
void destroyCar(Car *pCar)
{
    SDL_DestroyTexture(pCar->pCartexture);
}