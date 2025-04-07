#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define WIDTH 1280
#define HEIGHT 720

int main(int argc, char **pArgv)
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
        printf("Error: %s\n", IMG_GetError());
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
    TTF_Font *pFont = TTF_OpenFont("resources/PressStart2P-Regular.ttf", 35);
    if (!pFont)
    {
        printf("Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // färger
    SDL_Color green = {0, 255, 0};
    SDL_Color red = {255, 0, 0};

    // Rendera textytor
    SDL_Surface *pStartSurf = TTF_RenderText_Solid(pFont, "Start Game", green);
    SDL_Surface *pExitSurf = TTF_RenderText_Solid(pFont, "End Game", red);
    SDL_Texture *pStartTex = SDL_CreateTextureFromSurface(pRenderer, pStartSurf);
    SDL_Texture *pExitTex = SDL_CreateTextureFromSurface(pRenderer, pExitSurf);

    SDL_Rect startRect = {120, 450, pStartSurf->w, pStartSurf->h}; // position via paint windows
    SDL_Rect exitRect = {120, 500, pExitSurf->w, pExitSurf->h};

    SDL_FreeSurface(pStartSurf);
    SDL_FreeSurface(pExitSurf);

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
        SDL_RenderCopy(pRenderer, pStartTex, NULL, &startRect);
        SDL_RenderCopy(pRenderer, pExitTex, NULL, &exitRect);
        SDL_RenderPresent(pRenderer);
    }
    // stänga ner och rensa
    SDL_DestroyTexture(pStartTex);
    SDL_DestroyTexture(pExitTex);
    SDL_DestroyTexture(pTexture);
    TTF_CloseFont(pFont);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}