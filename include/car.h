#ifndef CAR_H
#define CAR_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct Car Car;

Car *createCar(SDL_Renderer *pRenderer, const char *pImagepath, int x, int y, int w, int h);
void updateCar(Car *pCar, const Uint8 *pKeys, SDL_Scancode up, SDL_Scancode down, SDL_Scancode left, SDL_Scancode right, float boost);
void renderCar(SDL_Renderer *pRenderer, Car *pCar);
void destroyCar(Car *pCar);
float getCarX(const Car *pCar);
float getCarY(const Car *pCar);
float getCarAngle(const Car *pCar);
SDL_Rect getCarRect(const Car *pCar);
int getTrailMarkX(const Car *car, int index);
int getTrailMarkY(const Car *car, int index);
int getTrailCount(const Car *car);
float getTrailMarkAngle(const Car *car, int index);
void setCarPosition(Car *car, float x, float y, float angle);
void setCarAngle(Car *pCar, float angle);
void setCarSpeed(Car *pCar, float speed);
void resolveCollision(Car *pA, Car *pB);

#endif
