#ifndef TEST_H
#define TEST_H

#include "game.h"
#include <stdbool.h>

// funktion som ska testköra om allt är korrekt
void runAllTests(GameResources *pRes, bool *pTestPassed, bool debugMode);

#endif