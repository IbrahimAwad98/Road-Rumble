#include "camera.h"

void updateCamera(Camera *pCamera, SDL_Rect *pTarget)
{
    pCamera->x = pTarget->x + pTarget->w / 2 - pCamera->w / 2;
    pCamera->y = pTarget->y + pTarget->h / 2 - pCamera->h / 2;

    if (pCamera->x < 0)
        pCamera->x = 0;
    if (pCamera->y < 0)
        pCamera->y = 0;
}
