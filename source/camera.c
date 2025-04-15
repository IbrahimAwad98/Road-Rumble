#include "camera.h"

void updateCamera(Camera *pCamera, SDL_Rect *pTarget)
{
    // Beräkna ny X-position: centrera kameran kring målets mittpunkt horisontellt
    pCamera->x = pTarget->x + pTarget->w / 2 - pCamera->w / 2;

    // Beräkna ny Y-position: centrera kameran kring målets mittpunkt vertikalt
    pCamera->y = pTarget->y + pTarget->h / 2 - pCamera->h / 2;

    // Begränsa kameran så att den inte går utanför vänster kant
    if (pCamera->x < 0)
    {
        pCamera->x = 0;
    }
    // Begränsa kameran så att den inte går utanför övre kant
    if (pCamera->y < 0)
    {
        pCamera->y = 0;
    }
}
