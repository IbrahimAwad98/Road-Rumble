#ifndef CAR_H
#define CAR_H
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

// definiera bilen
typedef struct car Car;

Car *createCar(SDL_Renderer *pRenderer, const char *pImagePath, int x, int y, int w, int h, int screenW, int screenH);
void updateCar(Car *pCar, const Uint8 *keys, SDL_Scancode up, SDL_Scancode down, SDL_Scancode left, SDL_Scancode right);
void renderCar(SDL_Renderer *pRenderer, Car *pCar);
void destroyCar(Car *pCar);

void setCarAngle(Car *pCar, float angle);
void setCarSpeed(Car *pCar, float speed);
void setCarX(Car *pCar, float x);
void setCarY(Car *pCar, float y);
float getCarAngle(Car *pCar);
float getCarSpeed(Car *pCar);
float getCarX(Car *pCar);
float getCarY(Car *pCar);
SDL_Rect getCarRect(Car *pCar);
void setCarRect(Car *pCar, SDL_Rect rect);



#endif