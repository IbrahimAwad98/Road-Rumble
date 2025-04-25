#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

// filer
#include "game.h"
#include "car.h"
#include "tilemap.h"
#include "client.h"
#include "server.h"
#include "network.h"

// Spelets huvudloop: hanterar input, rendering och växling mellan spellägen
void gameLoop(GameResources *pRes)
{
    // Ljudinställningar och tillstånd
    int isMuted = 0;                            // Flagga för ljud av/på
    int musicVolumeLevel = 4;                   // Musikvolym (0–4)
    int sfxLevel = 4;                           // Ljudeffektsvolym (0–4)
    int musicVolumes[5] = {0, 32, 64, 96, 128}; // Steg för musikvolym
    int sfxVolumes[5] = {0, 32, 64, 96, 128};   // Steg för ljudeffekter
    char joinIpText[32] = "127.0.0.1";          // Joina IP
    int selectedField = -1;                     // host=0, join=1

    SDL_Event event;
    bool isRunning = true;          // Om spelet ska fortsätta köras
    bool isFullscreen = true;       // flagga
    bool escWasPressedOnce = false; // flagga
    GameMode mode = MENU;           // Startläge: huvudmeny
    int hoveredButton = -1;         // Vilken menyknapp som musen är över

    // justerar automatisk
    SDL_RenderSetLogicalSize(pRes->pRenderer, WIDTH, HEIGHT);

    // Initiera bilar
    if (!initCar(pRes->pRenderer, &pRes->car1, "resources/Cars/Black_viper.png", 300, 300, 128, 64) ||
        !initCar(pRes->pRenderer, &pRes->car2, "resources/Cars/Police.png", 100, 100, 128, 64))
    {
        printf("Failed to create car texture: %s\n", SDL_GetError());
        return;
    }

    // Startvärden för bil 1
    pRes->car1.angle = 0.0f;
    pRes->car1.speed = 3.0f;
    // Startvärden för bil 1
    pRes->car2.angle = 0.0f;
    pRes->car2.speed = 3.0f;

    // Huvudloop
    while (isRunning)
    {
        //  Händelsehantering
        while (SDL_PollEvent(&event))
        {
            // Avsluta spel
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
            {
                if (!escWasPressedOnce)
                {
                    // Första trycket – växla till windowed
                    isFullscreen = false;
                    SDL_SetWindowFullscreen(pRes->pWindow, 0);                                            // Avsluta fullscreen
                    SDL_SetWindowSize(pRes->pWindow, WIDTH, HEIGHT);                                      // Återställ storlek
                    SDL_SetWindowPosition(pRes->pWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED); // Centrera
                    SDL_ShowWindow(pRes->pWindow);                                                        // Tvinga visning
                    SDL_Delay(100);                                                                       // Vänta kort för att undvika buggar

                    escWasPressedOnce = true;
                }
                else
                {
                    // Andra trycket – avsluta spelet
                    isRunning = false;
                }
            }
            // Menyinteraktion med musen
            else if (event.type == SDL_MOUSEBUTTONDOWN && mode == MENU)
            {
                int x = event.button.x, y = event.button.y;
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->startRect))
                {
                    mode = PLAYING;
                }
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->exitRect))
                {
                    isRunning = false;
                }
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->multiplayerRect))
                {
                    mode = MULTIPLAYER;
                }
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->optionsRect))
                {
                    mode = OPTIONS;
                }
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->muteRect))
                {
                    isMuted = !isMuted;
                    Mix_VolumeMusic(isMuted ? 0 : MIX_MAX_VOLUME);
                }
            }

            // Visuell feedback för menyknappar
            if (event.type == SDL_MOUSEMOTION && mode == MENU)
            {
                int x = event.motion.x, y = event.motion.y;
                hoveredButton = -1;
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->startRect))
                {
                    hoveredButton = 0;
                }
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->multiplayerRect))
                {
                    hoveredButton = 1;
                }

                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->optionsRect))
                {
                    hoveredButton = 2;
                }
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->exitRect))
                {
                    hoveredButton = 3;
                }
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->muteRect))
                {
                    hoveredButton = 4;
                }
            }

            // Snabbtangent-funktioner
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_p)
                {
                    mode = PLAYING;
                }
                else if (event.key.keysym.sym == SDLK_m)
                {
                    mode = MENU;
                }
            }
            //  Klick i inställningsmenyn
            if (event.type == SDL_MOUSEBUTTONDOWN && mode == OPTIONS)
            {
                int x = event.button.x, y = event.button.y;
                // Musikvolym
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->musicVolumeRect))
                {
                    int seg = (x - pRes->musicVolumeRect.x) / (pRes->musicVolumeRect.w / 5);
                    musicVolumeLevel = (seg < 0) ? 0 : (seg > 4 ? 4 : seg);
                    Mix_VolumeMusic(musicVolumes[musicVolumeLevel]);
                }
                // Ljudeffektvolym
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->SfxRect))
                {
                    int seg = (x - pRes->SfxRect.x) / (pRes->SfxRect.w / 5);
                    sfxLevel = (seg < 0) ? 0 : (seg > 4 ? 4 : seg);
                    Mix_VolumeMusic(sfxVolumes[sfxLevel]);
                }
                // Tillbaka till meny
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->backRect))
                {
                    mode = MENU;
                }
            }

            if (event.type == SDL_TEXTINPUT && mode == MULTIPLAYER && selectedField == 1)
            {
                if (strlen(joinIpText) < sizeof(joinIpText) - 1)
                {
                    strcat(joinIpText, event.text.text);
                }
            }

            if (event.type == SDL_KEYDOWN && mode == MULTIPLAYER && selectedField == 1)
            {
                if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(joinIpText) > 0)
                {
                    joinIpText[strlen(joinIpText) - 1] = '\0';
                }
            }

            //  Klick i multiplayermenyn
            if (event.type == SDL_MOUSEBUTTONDOWN && mode == MULTIPLAYER)
            {
                int x = event.button.x;
                int y = event.button.y;
                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->backMRect))
                {
                    mode = MENU;
                }
            }
        }

        // Rendering
        SDL_RenderClear(pRes->pRenderer);

        // Huvudmenyn
        if (mode == MENU)
        {
            // Bakgrund
            SDL_RenderCopy(pRes->pRenderer, pRes->pBackgroundTexture, NULL, NULL);

            // Färgtoning för hovereffekt
            SDL_SetTextureColorMod(pRes->pStartTexture, hoveredButton == 0 ? 200 : 255, hoveredButton == 0 ? 200 : 255, 255);
            SDL_SetTextureColorMod(pRes->pMultiplayerTexture, hoveredButton == 1 ? 200 : 255, hoveredButton == 1 ? 200 : 255, 255);
            SDL_SetTextureColorMod(pRes->pOptionsTexture, hoveredButton == 2 ? 200 : 255, hoveredButton == 2 ? 200 : 255, 255);
            SDL_SetTextureColorMod(pRes->pExitTexture, hoveredButton == 3 ? 200 : 255, hoveredButton == 3 ? 200 : 255, 255);
            SDL_SetTextureColorMod(isMuted ? pRes->pMuteTexture : pRes->pUnmuteTexture,
                                   hoveredButton == 4 ? 200 : 255, hoveredButton == 4 ? 200 : 255, 255);

            // Rendera knappar
            SDL_RenderCopy(pRes->pRenderer, pRes->pStartTexture, NULL, &pRes->startRect);

            SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerTexture, NULL, &pRes->multiplayerRect);
            SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsTexture, NULL, &pRes->optionsRect);
            SDL_RenderCopy(pRes->pRenderer, pRes->pExitTexture, NULL, &pRes->exitRect);
            SDL_RenderCopy(pRes->pRenderer, isMuted ? pRes->pMuteTexture : pRes->pUnmuteTexture, NULL, &pRes->muteRect);
        }

        // Spelläget (via nätverk)
        else if (mode == PLAYING)
        {
            SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 255);
            SDL_RenderClear(pRes->pRenderer);
            const Uint8 *keys = SDL_GetKeyboardState(NULL);

            // För test: styr båda lokalt
            /*
            updateCar(&pRes->car1, keys, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT);
            updateCar(&pRes->car2, keys, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D);
            */

            PlayerData myData, otherData;
            IPaddress clientAddr; // används bara på server

            if (isServer)
            {
                updateCar(&pRes->car1, keys, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT);

                // ta emot klientdata
                if (server_receivePlayerData(&otherData, &clientAddr))
                {
                    pRes->car2.x = otherData.x;
                    pRes->car2.y = otherData.y;
                }
                // skicka till klient
                myData.x = pRes->car1.x;
                myData.y = pRes->car1.y;
                myData.actionCode = 1; // valfritt
                server_sendPlayerData(&myData, &clientAddr);
            }
            else
            {
                updateCar(&pRes->car2, keys, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D);

                myData.playerID = 1;
                myData.x = pRes->car2.x;
                myData.y = pRes->car2.y;
                myData.actionCode = 1;

                client_sendPlayerData(&myData);

                if (client_receiveServerData(&otherData))
                {
                    // uppdatera serverns bil (car1)
                    pRes->car1.x = otherData.x;
                    pRes->car1.y = otherData.y;
                }
            }

            // uppdatera renderingsrektanglar
            pRes->car1.carRect.x = (int)pRes->car1.x;
            pRes->car1.carRect.y = (int)pRes->car1.y;
            pRes->car2.carRect.x = (int)pRes->car2.x;
            pRes->car2.carRect.y = (int)pRes->car2.y;

            renderGrassBackground(pRes->pRenderer, pRes->pTiles, 93);
            renderTrackAndObjects(pRes->pRenderer, pRes->pTiles, tilemap);
            // ingen kamera
            renderCar(pRes->pRenderer, &pRes->car1);
            renderCar(pRes->pRenderer, &pRes->car2);

            // Test: rendera en tile från tileset (för debug/visning)
            SDL_Rect src = getTileSrcByID(2);
            SDL_Rect dest = {400, 300, TILE_SIZE, TILE_SIZE};
            SDL_RenderCopy(pRes->pRenderer, pRes->ptilesetTexture, &src, &dest);
        }

        //  Inställningsmeny
        else if (mode == OPTIONS)
        {
            SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsMenuTex, NULL, NULL);
            SDL_RenderCopy(pRes->pRenderer, pRes->pBackToMenuTexture, NULL, &pRes->backRect);

            // Rita volymstaplar (musik)
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

            // Rita volymstaplar (SFX)
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

        //  Multiplayer-meny
        else if (mode == MULTIPLAYER)
        {
            SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerMenuTex, NULL, NULL);
            SDL_RenderCopy(pRes->pRenderer, pRes->pBackToMultiTexture, NULL, &pRes->backMRect);
            SDL_StartTextInput();

            SDL_Color white = {255, 255, 255};
            SDL_Surface *ipSurf = TTF_RenderText_Solid(pRes->pFont, joinIpText, white);
            SDL_Texture *ipTex = SDL_CreateTextureFromSurface(pRes->pRenderer, ipSurf);

            SDL_Rect ipRect = {550, 375, ipSurf->w, ipSurf->h};
            SDL_RenderCopy(pRes->pRenderer, ipTex, NULL, &ipRect);

            SDL_FreeSurface(ipSurf);
            SDL_DestroyTexture(ipTex);
        }

        // Presentera det som ritats
        SDL_RenderPresent(pRes->pRenderer);
    }
}