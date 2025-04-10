#ifndef CAR_H
#define CAR_H
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

// definiera bilen
typedef struct
{
    SDL_Texture *pCartexture;
    SDL_Rect carRect;
    float angle;
    float speed;
} Car;

// tre funktioner för att laddar bilen och städa efter
bool initiCar(SDL_Renderer *pRenderer, Car *pCar, const char *pImagepath, int x, int y, int w, int h);
void renderCar(SDL_Renderer *pRenderer, Car *pCar);
void destroyCar(Car *pCar);

#endif