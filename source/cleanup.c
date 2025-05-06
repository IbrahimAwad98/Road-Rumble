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
    // rensa bilar
    destroyCar(pRes->pCar1);
    destroyCar(pRes->pCar2);

    // Frigör alla resurser i omvänd ordning
    SDL_DestroyTexture(pRes->pStartTexture);
    SDL_DestroyTexture(pRes->pExitTexture);
    SDL_DestroyTexture(pRes->pBackgroundTexture);
    TTF_CloseFont(pRes->pFont);
    SDL_DestroyRenderer(pRes->pRenderer);
    SDL_DestroyWindow(pRes->pWindow);
    Mix_FreeMusic(pRes->pBgMusic);

    // frigöra nätverk
    closeClient();
    closeServer();

    // Avsluta alla SDL-subsystem
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    SDLNet_Quit();
    Mix_CloseAudio();
}
