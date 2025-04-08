#include "game.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

void gameLoop(GameResources *pRes)
{
    SDL_Event event;
    bool isRunning = true;

    while (isRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
            {
                isRunning = false;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int x = event.button.x;
                int y = event.button.y;

                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->startRect))
                {
                    SDL_Log("Starta spelet!");
                }

                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->exitRect))
                {
                    SDL_Log("Avslutar spelet!");
                    isRunning = false;
                }
            }
        }

        // Rendera
        SDL_RenderClear(pRes->pRenderer);
        SDL_RenderCopy(pRes->pRenderer, pRes->pBackgroundTexture, NULL, NULL);
        SDL_RenderCopy(pRes->pRenderer, pRes->pStartTexture, NULL, &pRes->startRect);
        SDL_RenderCopy(pRes->pRenderer, pRes->pExitTexture, NULL, &pRes->exitRect);
        SDL_RenderPresent(pRes->pRenderer);
    }
}
