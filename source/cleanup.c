#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>
#include "cleanup.h"
#include "car.h"
#include "client.h"
#include "server.h"

void cleanup(GameResources *pRes)
{
    destroyCar(pRes->pCar1); // rensa bilar
    destroyCar(pRes->pCar2);
    destroyCar(pRes->pCar3);
    destroyCar(pRes->pCar4);
    SDL_DestroyTexture(pRes->pStartTexture); // Frigör alla resurser i omvänd ordning
    SDL_DestroyTexture(pRes->pExitTexture);
    SDL_DestroyTexture(pRes->pBackgroundTexture);
    TTF_CloseFont(pRes->pFont);
    SDL_DestroyRenderer(pRes->pRenderer);
    SDL_DestroyWindow(pRes->pWindow);
    Mix_FreeMusic(pRes->pBgMusic);
    closeClient(); // frigöra nätverk
    closeServer();
    TTF_Quit(); // Avsluta alla SDL-subsystem
    IMG_Quit();
    SDL_Quit();
    SDLNet_Quit();
    Mix_CloseAudio();
}
