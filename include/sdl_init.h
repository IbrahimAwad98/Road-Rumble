#ifndef SDL_INIT_H
#define SDL_INIT_H

#include "game.h"
#include <stdbool.h>

// Initierar SDL, fönster, ljud, nätverk m.m.
// Returnerar true om allt gick bra
bool initSDL(GameResources *pRes);

#endif
