#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

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
    IMG_Init(IMG_INIT_PNG); // bilder.
    TTF_Init();             // font
    // Skapar fönster
    SDL_Window *pWindow = SDL_CreateWindow("Road Rumble ver 0.1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!pWindow)
    {
        printf("Fönstret kunde inte skapas! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    // Skapar render
    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!pRenderer)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(pWindow);
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
    // konvertera en bild (SDL_Surface) till en textur
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);
    if (!pTexture)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }
    // Ladda font
    TTF_Font *pFont = TTF_OpenFont("resources/PressStart2P.ttf", 28);
    if (!pFont)
    {
        printf("Error: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Color white = {255, 255, 255};

    // Rendera textytor
    SDL_Surface *startSurf = TTF_RenderText_Solid(pFont, "Starta spelet", white);
    SDL_Surface *exitSurf = TTF_RenderText_Solid(pFont, "Avsluta spelet", white);
    SDL_Texture *startTex = SDL_CreateTextureFromSurface(pRenderer, startSurf);
    SDL_Texture *exitTex = SDL_CreateTextureFromSurface(pRenderer, exitSurf);

    SDL_Rect startRect = {500, 250, startSurf->w, startSurf->h};
    SDL_Rect exitRect = {500, 350, exitSurf->w, exitSurf->h};

    SDL_FreeSurface(startSurf);
    SDL_FreeSurface(exitSurf);

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
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int x = event.button.x;
                int y = event.button.y;

                if (SDL_PointInRect(&(SDL_Point){x, y}, &startRect))
                {
                    SDL_Log("Starta spelet!");
                    // Lägg till spelets logik här
                }
                if (SDL_PointInRect(&(SDL_Point){x, y}, &exitRect))
                {
                    SDL_Log("Avsluta spelet!");
                    isRunning = false;
                }
            }
        }
        // Här visas bilden
        SDL_RenderClear(pRenderer);
        SDL_RenderCopy(pRenderer, pTexture, NULL, NULL); // Rita bakgrundsbild
        SDL_RenderCopy(pRenderer, startTex, NULL, &startRect);
        SDL_RenderCopy(pRenderer, exitTex, NULL, &exitRect);
        SDL_RenderPresent(pRenderer);
    }
    // stänga ner och rensa
    SDL_DestroyTexture(startTex);
    SDL_DestroyTexture(exitTex);
    SDL_DestroyTexture(pTexture);
    TTF_CloseFont(pFont);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
