#ifndef TEST_H
#define TEST_H

#include <stdbool.h>
#include "game.h"

// funktion som ska testköra om allt är korrekt
void runAllTests(GameResources *pRes, bool *pTestPassed, bool debugMode);

#endif