#ifndef CAR_H
#define CAR_H

#include <SDL2/SDL.h>
#include <stdbool.h>

// Ta bort detaljerna — gör den abstrakt
typedef struct Car Car;

// Funktionen skapar och returnerar en ny bil
Car *createCar(SDL_Renderer *pRenderer, const char *pImagepath, int x, int y, int w, int h);

// Uppdaterar bilens rörelse
void updateCar(Car *pCar, const Uint8 *pKeys, SDL_Scancode up, SDL_Scancode down, SDL_Scancode left, SDL_Scancode right);

// Renderar bilen
void renderCar(SDL_Renderer *pRenderer, Car *pCar);

// Förstör bilen
void destroyCar(Car *pCar);

// Getter-funktioner
float getCarX(const Car *pCar);
float getCarY(const Car *pCar);
float getCarAngle(const Car *pCar);
SDL_Rect getCarRect(const Car *pCar);

// fixa postioner
void setCarPosition(Car *car, float x, float y, float angle);
void setCarAngle(Car *pCar, float angle);
void setCarSpeed(Car *pCar, float speed);

#endif
