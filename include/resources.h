#ifndef RESOURCES_H
#define RESOURCES_H

#include "game.h"
#include <stdbool.h>

// Laddar in alla resurser som bakgrund, text, ljud, fonter
// Returnerar true om allt gick bra
bool loadResources(GameResources *pRes);

#endif
