#ifndef CAR_H
#define CAR_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct Car Car;

// Konstruktion / Destruktion
Car *createCar(SDL_Renderer *pRenderer, const char *pImagepath, int x, int y, int w, int h); // Skapar en bil
void destroyCar(Car *pCar);                                                                  // Frigör minne

// Uppdatering / Rendering
void updateCar(Car *pCar, const Uint8 *pKeys, SDL_Scancode up, SDL_Scancode down, SDL_Scancode left, SDL_Scancode right, float boost); // Uppdaterar input och rörelse
void renderCar(SDL_Renderer *pRenderer, Car *pCar);                                                                                    // Renderar bilen

// Getters
float getCarX(const Car *pCar);       // Hämtar X-position
float getCarY(const Car *pCar);       // Hämtar Y-position
float getCarAngle(const Car *pCar);   // Hämtar rotation
int getCarWidth(Car *car);            // Hämtar bredd
int getCarHeight(Car *car);           // Hämtar höjd
SDL_Rect getCarRect(const Car *pCar); // Hämtar renderingsrektangel

int getTrailMarkX(const Car *car, int index);       // Hämtar trail X-koordinat
int getTrailMarkY(const Car *car, int index);       // Hämtar trail Y-koordinat
float getTrailMarkAngle(const Car *car, int index); // Hämtar trail vinkel
float getCarSpeed(const Car *pCar);
int getTrailCount(const Car *car); // Antal trail-märken
bool isCarDrifting(const Car *pCar);

// Setters
void setCarPosition(Car *car, float x, float y, float angle); // Sätter position och vinkel
void setCarAngle(Car *pCar, float angle);                     // Sätter enbart vinkel
void setCarSpeed(Car *pCar, float speed);                     // Sätter fart
void setCarDrifting(Car *pCar, bool drifting);
// Kollision
void resolveCollision(Car *pA, Car *pB); // Löser krock mellan två bilar
void addTrailIfDrifting(Car *pCar);      // Lägger till en trail-markering om bilen driftar

#endif
