#ifndef TEST_H
#define TEST_H

#include <stdbool.h>
#include "game.h"

// Kör tester för grafik, ljud, nätverk och resurser
void runAllTests(GameResources *pRes, bool *pTestPassed, bool debugMode);

#endif
