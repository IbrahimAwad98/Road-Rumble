#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

#include "game.h"
#include "car.h"
#include "camera.h"
#include "tilemap.h"
#include "client.h"
#include "server.h"
#include "network.h"

// Spelets huvudloop: hanterar input, rendering och växling mellan spellägen
void gameLoop(GameResources *pRes)
{
    // Lokala tillståndsvariabler
    int isMuted = 0;                            // Ljud av/på
    int musicVolumeLevel = 4;                   // Musikvolymnivå (0–4)
    int sfxLevel = 4;                           // Ljudeffekt-nivå (0–4)
    int musicVolumes[5] = {0, 32, 64, 96, 128}; // Volymtabell
    int sfxVolumes[5] = {0, 32, 64, 96, 128};   // SFX-volymtabell

    SDL_Event event;        // SDL-händelser
    bool isRunning = true;  // Kontroll för huvudloop
    GameMode mode = MENU;   // Startläge: meny
    int hoveredButton = -1; // Håller reda på vilket menyval som är under muspekaren

    // Initiera spelkomponenter
    pRes->camera1 = (Camera){0, 0, WIDTH, HEIGHT};
    pRes->camera2 = (Camera){0, 0, WIDTH, HEIGHT};

    if (!initCar(pRes->pRenderer, &pRes->car1, "resources/Cars/Black_viper.png", 300, 300, 128, 64) ||
        !initCar(pRes->pRenderer, &pRes->car2, "resources/Cars/Police.png", 100, 100, 128, 64))
    {
        printf("Failed to create car texture: %s\n", SDL_GetError());
        return;
    }

    pRes->car1.angle = 0.0f;
    pRes->car1.speed = 3.0f;

    // Huvudloopen körs så länge spelet är aktivt
    while (isRunning)
    {
        //  Input-hantering
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT ||
                (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
                isRunning = false;

            // Menyinteraktion (klick)
            else if (event.type == SDL_MOUSEBUTTONDOWN && mode == MENU)
            {
                int x = event.button.x, y = event.button.y;
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->startRect))
                    mode = PLAYING;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->exitRect))
                    isRunning = false;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->multiplayerRect))
                    mode = MULTIPLAYER;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->optionsRect))
                    mode = OPTIONS;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->muteRect))
                {
                    isMuted = !isMuted;
                    Mix_VolumeMusic(isMuted ? 0 : MIX_MAX_VOLUME);
                }
            }

            // Hover-effekter i meny
            if (event.type == SDL_MOUSEMOTION && mode == MENU)
            {
                int x = event.motion.x, y = event.motion.y;
                hoveredButton = -1;
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->startRect))
                    hoveredButton = 0;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->multiplayerRect))
                    hoveredButton = 1;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->optionsRect))
                    hoveredButton = 2;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->exitRect))
                    hoveredButton = 3;
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->muteRect))
                    hoveredButton = 4;
            }

            // Tangentbordsgenvägar
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_p)
                    mode = PLAYING;
                else if (event.key.keysym.sym == SDLK_m)
                    mode = MENU;
            }

            // Klick i inställningsmenyn
            if (event.type == SDL_MOUSEBUTTONDOWN && mode == OPTIONS)
            {
                int x = event.button.x, y = event.button.y;
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->musicVolumeRect))
                {
                    int seg = (x - pRes->musicVolumeRect.x) / (pRes->musicVolumeRect.w / 5);
                    musicVolumeLevel = (seg < 0) ? 0 : (seg > 4 ? 4 : seg);
                    Mix_VolumeMusic(musicVolumes[musicVolumeLevel]);
                }
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->SfxRect))
                {
                    int seg = (x - pRes->SfxRect.x) / (pRes->SfxRect.w / 5);
                    sfxLevel = (seg < 0) ? 0 : (seg > 4 ? 4 : seg);
                    Mix_VolumeMusic(sfxVolumes[sfxLevel]);
                }
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->backRect))
                {
                    mode = MENU;
                }
            }
        }

        //  Rendering beroende på spelläge
        SDL_RenderClear(pRes->pRenderer);

        //  MENY
        if (mode == MENU)
        {
            SDL_RenderCopy(pRes->pRenderer, pRes->pBackgroundTexture, NULL, NULL);

            SDL_SetTextureColorMod(pRes->pStartTexture, hoveredButton == 0 ? 200 : 255, hoveredButton == 0 ? 200 : 255, 255);
            SDL_SetTextureColorMod(pRes->pMultiplayerTexture, hoveredButton == 1 ? 200 : 255, hoveredButton == 1 ? 200 : 255, 255);
            SDL_SetTextureColorMod(pRes->pOptionsTexture, hoveredButton == 2 ? 200 : 255, hoveredButton == 2 ? 200 : 255, 255);
            SDL_SetTextureColorMod(pRes->pExitTexture, hoveredButton == 3 ? 200 : 255, hoveredButton == 3 ? 200 : 255, 255);
            SDL_SetTextureColorMod(isMuted ? pRes->pMuteTexture : pRes->pUnmuteTexture,
                                   hoveredButton == 4 ? 200 : 255, hoveredButton == 4 ? 200 : 255, 255);

            SDL_RenderCopy(pRes->pRenderer, pRes->pStartTexture, NULL, &pRes->startRect);
            SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerTexture, NULL, &pRes->multiplayerRect);
            SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsTexture, NULL, &pRes->optionsRect);
            SDL_RenderCopy(pRes->pRenderer, pRes->pExitTexture, NULL, &pRes->exitRect);
            SDL_RenderCopy(pRes->pRenderer, isMuted ? pRes->pMuteTexture : pRes->pUnmuteTexture, NULL, &pRes->muteRect);
        }

        //  SPELLÄGE: PLAYING
        else if (mode == PLAYING)
        {
            SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 255);
            SDL_RenderClear(pRes->pRenderer);

            const Uint8 *keys = SDL_GetKeyboardState(NULL);
            updateCar(&pRes->car1, keys);
            Camera *pCam = (pRes->localPlayerID == 0) ? &pRes->camera1 : &pRes->camera2;
            updateCamera(pCam, &pRes->car1.carRect);

            renderGrassBackground(pRes->pRenderer, pRes->pTiles, 93, pCam);
            renderTrackAndObjects(pRes->pRenderer, pRes->pTiles, tilemap, pCam);
            renderCar(pRes->pRenderer, &pRes->car1, pCam);
            renderCar(pRes->pRenderer, &pRes->car2, pCam);

            SDL_Rect src = getTileSrcByID(2);
            SDL_Rect dest = {400, 300, TILE_SIZE, TILE_SIZE};
            SDL_RenderCopy(pRes->pRenderer, pRes->ptilesetTexture, &src, &dest);
        }

        //  OPTIONS
        else if (mode == OPTIONS)
        {
            SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsMenuTex, NULL, NULL);
            SDL_RenderCopy(pRes->pRenderer, pRes->pBackToMenuTexture, NULL, &pRes->backRect);

            for (int i = 0; i < 5; i++)
            {
                SDL_Rect block = {
                    pRes->musicVolumeRect.x + i * (pRes->musicVolumeRect.w / 5),
                    pRes->musicVolumeRect.y,
                    (pRes->musicVolumeRect.w / 5) - 4,
                    pRes->musicVolumeRect.h};
                SDL_SetRenderDrawColor(pRes->pRenderer, (i <= musicVolumeLevel) ? 255 : 30, 128, 0, 255);
                SDL_RenderFillRect(pRes->pRenderer, &block);
            }

            for (int i = 0; i < 5; i++)
            {
                SDL_Rect block = {
                    pRes->SfxRect.x + i * (pRes->SfxRect.w / 5),
                    pRes->SfxRect.y,
                    (pRes->SfxRect.w / 5) - 4,
                    pRes->SfxRect.h};
                SDL_SetRenderDrawColor(pRes->pRenderer, (i <= sfxLevel) ? 255 : 30, 128, 0, 255);
                SDL_RenderFillRect(pRes->pRenderer, &block);
            }
        }

        // MULTIPLAYER
        else if (mode == MULTIPLAYER)
        {
            SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerMenuTex, NULL, NULL);
        }

        // === Presentera allt på skärmen ===
        SDL_RenderPresent(pRes->pRenderer);
    }
}
