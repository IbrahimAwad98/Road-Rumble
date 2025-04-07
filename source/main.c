#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h> // används senare för nätverksfunktioner

#define WIDTH 1280 // Bredd på fönstret i pixlar
#define HEIGHT 720 // Höjd på fönstret i pixlar

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initierar SDL för video
    {
        printf("SDL could not start! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    IMG_Init(IMG_INIT_PNG); // Initierar stöd för bilder (PNG)
    TTF_Init();             // Initierar stöd för TrueType-fonts

    if (SDLNet_Init() < 0) // Initierar SDL_net för nätverkskommunikation
    {
        printf("SDLNet_Init failed: %s\n", SDLNet_GetError());
        return 1;
    }
    // Skapar ett fönster med titel, position och storlek
    SDL_Window *pWindow = SDL_CreateWindow("Road Rumble ver 0.1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!pWindow)
    {
        printf("The window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    // Skapar en renderer för fönstret (accelererad med vsync)
    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!pRenderer)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }
    // Laddar bakgrundsbilden från fil
    SDL_Surface *pSurface = IMG_Load("resources/Sim2.png");
    if (!pSurface)
    {
        printf("Error: %s\n", IMG_GetError());
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }
    // Konverterar bilden till en textur som kan användas med renderern
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface); // Behöver inte ytan längre
    if (!pTexture)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }
    // Laddar fontfil med storlek 35
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

    // Definierar färger för texterna
    SDL_Color green = {0, 255, 0};
    SDL_Color red = {255, 0, 0};

    // Skapar ytor med texten "Start Game" och "Quit Game"
    SDL_Surface *pStartSurf = TTF_RenderText_Solid(pFont, "Start Game", green);
    SDL_Surface *pExitSurf = TTF_RenderText_Solid(pFont, "Quit Game", red);

    // Skapar texturer från ytorna
    SDL_Texture *pStartTex = SDL_CreateTextureFromSurface(pRenderer, pStartSurf);
    SDL_Texture *pExitTex = SDL_CreateTextureFromSurface(pRenderer, pExitSurf);

    // Definierar position och storlek för texterna (visas som knappar)
    SDL_Rect startRect = {120, 450, pStartSurf->w, pStartSurf->h};
    SDL_Rect exitRect = {120, 500, pExitSurf->w, pExitSurf->h};

    // Rensar ytorna efter att texturer skapats
    SDL_FreeSurface(pStartSurf);
    SDL_FreeSurface(pExitSurf);

    // Startar huvudloopen som håller fönstret öppet
    bool isRunning = true;
    SDL_Event event;
    while (isRunning)
    {
        // Hanterar alla inkommande events (t.ex. tangenttryck, mus)
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) // Om man stänger fönstret
            {
                isRunning = false;
            }
            else if (event.type == SDL_KEYDOWN) // Om man trycker på en tangent
            {
                if (event.key.keysym.sym == SDLK_ESCAPE) // ESC avslutar
                {
                    isRunning = false;
                }
            }

            // Kollar om användaren klickar med musen
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                int x = event.button.x;
                int y = event.button.y;

                // Kollar om användaren klickade på "Start Game"
                if (SDL_PointInRect(&(SDL_Point){x, y}, &startRect))
                {
                    SDL_Log("Start the game!");
                    // Här kan man lägga till logik för att starta spelet
                }

                // Kollar om användaren klickade på "Quit Game"
                if (SDL_PointInRect(&(SDL_Point){x, y}, &exitRect))
                {
                    SDL_Log("Exit the game!");
                    isRunning = false;
                }
            }
        }

        // Rensar skärmen
        SDL_RenderClear(pRenderer);

        // Ritar bakgrund och textknappar
        SDL_RenderCopy(pRenderer, pTexture, NULL, NULL);        // Bakgrund
        SDL_RenderCopy(pRenderer, pStartTex, NULL, &startRect); // Start-knapp
        SDL_RenderCopy(pRenderer, pExitTex, NULL, &exitRect);   // Quit-knapp

        // Uppdaterar fönstret visuellt
        SDL_RenderPresent(pRenderer);
    }

    // Rensar alla resurser innan avslut
    SDL_DestroyTexture(pStartTex);
    SDL_DestroyTexture(pExitTex);
    SDL_DestroyTexture(pTexture);
    TTF_CloseFont(pFont);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);

    // Avslutar alla SDL-moduler
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    SDLNet_Quit();

    return 0;
}