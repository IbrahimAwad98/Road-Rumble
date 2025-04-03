#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define WIDTH 1280
#define HEIGHT 720

int main(int argc, char *pArgv[])
{
    // Initierar SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL kunde inte startas! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Skapar fönster
    SDL_Window *pWindow = SDL_CreateWindow("Road Rumble ver 0.1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!pWindow)
    {
        printf("Fönstret kunde inte skapas! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Laddar bild
    SDL_Surface *pSurface = IMG_Load("resources/Sim2.png");
    if (!pSurface)
    {
        printf("Fel vid bildladdning: %s\n", IMG_GetError());
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    // Sätt bildytan till fönstret
    SDL_Surface *windowSurface = SDL_GetWindowSurface(pWindow);
    // första null är hela bilden och andra null är vart i fönstret ska ritas.
    SDL_BlitSurface(pSurface, NULL, windowSurface, NULL);
    SDL_UpdateWindowSurface(pWindow);

    // Event loop (för att hålla fönstret)
    bool isRunning = true;
    SDL_Event event;
    while (isRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    isRunning = false;
                }
            }
        }
    }
    // stänga ner och rensa
    SDL_FreeSurface(pSurface);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();

    return 0;
}
