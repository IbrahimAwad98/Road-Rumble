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
void gameLoop(GameResources *pRes, int localPlayerID)
{
    // Ljudinställningar och tillstånd
    int isMuted = 0;                            // Flagga för ljud av/på
    int musicVolumeLevel = 4;                   // Musikvolym (0–4)
    int sfxLevel = 4;                           // Ljudeffektsvolym (0–4)
    int musicVolumes[5] = {0, 32, 64, 96, 128}; // Steg för musikvolym
    int sfxVolumes[5] = {0, 32, 64, 96, 128};   // Steg för ljudeffekter
    char joinIpText[16] = "";                   // Joina IP
    char playerIdText[4] = "";
    char hostText[32] = " 127.0.0.1"; // Host Texten (lokalt)
    int selectedField = -1;           // host=0, join=1
    char availableServ[16][5];        // alla tillgängliga/startade servrar
    char portText[8] = "55000";       // för att visa i multiplayer meny
    SDL_Event event;
    bool isRunning = true;          // Om spelet ska fortsätta köras
    bool isFullscreen = true;       // flagga
    bool escWasPressedOnce = false; // flagga
    GameMode mode = MENU;           // Startläge: huvudmeny
    int hoveredButton = -1;         // Vilken menyknapp som musen är över
    Uint32 ping = 0;                // ping-mätning

    // justerar automatisk
    SDL_RenderSetLogicalSize(pRes->pRenderer, WIDTH, HEIGHT);


     // Skapa bilarna EN gång för att undivika att skapa varje frame
    // nummerna = pixlar horisontellt, vertikalt och storlek: bredd x höjd
    pRes->car1 = createCar(pRes->pRenderer, "resources/Cars/Black_viper.png", 300, 300, 128, 64, WIDTH, HEIGHT);
    pRes->car2 = createCar(pRes->pRenderer, "resources/Cars/Police.png", 100, 100, 128, 64, WIDTH, HEIGHT);
   if (!pRes->car1 || !pRes->car2) {
        SDL_Log("Failed to create car(s).");

    // Initiera bilar
    if (!initCar(pRes->pRenderer, &pRes->car1, "resources/Cars/Black_viper.png", 128, 640, 128, 64) ||
        !initCar(pRes->pRenderer, &pRes->car2, "resources/Cars/Police.png", 1024, 640, 128, 64))
    {
        printf("Failed to create car texture: %s\n", SDL_GetError());
        return;
    }

    setCarAngle(pRes->car1, 0.0f);
    setCarSpeed(pRes->car1, 3.0f);
    setCarAngle(pRes->car2, 0.0f);
    setCarSpeed(pRes->car2, 3.0f);


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

            // trycka på join felt eller player felt
            if (event.type == SDL_TEXTINPUT && mode == MULTIPLAYER)
            {
                if (selectedField == 1 && strlen(joinIpText) < sizeof(joinIpText) - 1)
                {
                    strcat(joinIpText, event.text.text);
                }
                else if (selectedField == 2 && strlen(playerIdText) < sizeof(playerIdText) - 1)
                {
                    strcat(playerIdText, event.text.text);
                }
            }

            // trycker på enter när man är klar med ip add....
            if (event.type == SDL_KEYDOWN && mode == MULTIPLAYER && selectedField == 1)
            {
                if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(joinIpText) > 0)
                {
                    joinIpText[strlen(joinIpText) - 1] = '\0';
                }
                else if (event.key.keysym.sym == SDLK_RETURN)
                {
                    if (initClient(joinIpText, SERVER_PORT))
                    {
                        printf("Client connected to %s!\n", joinIpText);
                        mode = PLAYING;
                    }
                    else
                    {
                        printf("Failed to connect to server at %s\n", joinIpText);
                    }
                }
            }

            // klick på Player felt...

            if (event.type == SDL_KEYDOWN && mode == MULTIPLAYER && selectedField == 2)
            {
                if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(playerIdText) > 0)
                {
                    playerIdText[strlen(playerIdText) - 1] = '\0';
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
                    selectedField = -1;
                    SDL_StopTextInput();
                    joinIpText[0] = '\0';
                    playerIdText[0] = '\0';
                }
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->joinRect))
                {
                    selectedField = 1;
                    SDL_StartTextInput();
                }
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->playerIdRect))
                {
                    selectedField = 2;
                    SDL_StartTextInput();
                    
                }
                else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->enterRect))
                {
                    if (strlen(joinIpText) > 0 && strlen(playerIdText) > 0)
                    {
                        localPlayerID = atoi(playerIdText); // <- Set playerID globally!
                        
                        if (initClient(joinIpText, SERVER_PORT))
                        {
                            printf("Connected to %s successfully as Player %d!\n", joinIpText,localPlayerID);
                            mode = PLAYING;
                        }
                        else
                        {
                            printf("Failed to connect to server at %s\n", joinIpText);
                        }
                    }    
                    else
                    {
                        printf("Please fill IP address and Player ID!\n");
                    }
                }
                
                else
                {
                    selectedField = -1;
                    SDL_StopTextInput();
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
            SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 255); // resna skärmen
            SDL_RenderClear(pRes->pRenderer);
            const Uint8 *keys = SDL_GetKeyboardState(NULL); // läs tangent

            if (localPlayerID == 0)
            {
                updateCar(pRes->car1, keys, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D);
            }
            else // localPlayerID == 1
            {
                updateCar(pRes->car2, keys, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D);
            }

            // Multiplayerdata
            PlayerData myData = {0};
            myData.playerID = localPlayerID;
            if (localPlayerID == 0)
            {
                myData.x = getCarX(pRes->car1);
                myData.y = getCarY(pRes->car1);
                myData.angle = getCarAngle(pRes->car1);
            }
            else
            {
                myData.x = getCarX(pRes->car2);
                myData.y = getCarY(pRes->car2);
                myData.angle = getCarAngle(pRes->car2);
            }
            client_sendPlayerData(&myData);

            // Det här en buffer för att inte tappa paketet
            static PlayerData lastOpponent = {0}; // Behåll senaste datan

            // Ta emot motståndarens bil
            PlayerData opponentData = {0};
            bool opponentConnected = false;
            if (client_receiveServerData(&opponentData))
            {
                // beräkning av ping
                Uint32 now = SDL_GetTicks();
                if (opponentData.timestamp > 0)
                {
                    ping = now - opponentData.timestamp;
                }

                if (opponentData.playerID != localPlayerID)
                {
                    lastOpponent = opponentData;
                    opponentConnected = true;
                }

                if (localPlayerID == 0)
                {
                    setCarX(pRes->car2, lastOpponent.x);
                    setCarY(pRes->car2, lastOpponent.y);
                    setCarAngle(pRes->car2, lastOpponent.angle);
                }
                else
                {
                    setCarX(pRes->car1, lastOpponent.x);
                    setCarY(pRes->car1, lastOpponent.y);
                    setCarAngle(pRes->car1, lastOpponent.angle);
                }
            }
            // uppdatera renderingsrektanglar
            SDL_Rect rect1 = getCarRect(pRes->car1);
            rect1.x = (int)getCarX(pRes->car1);
            rect1.y = (int)getCarY(pRes->car1);
            setCarRect(pRes->car1, rect1);

            SDL_Rect rect2 = getCarRect(pRes->car2);
            rect2.x = (int)getCarX(pRes->car2);
            rect2.y = (int)getCarY(pRes->car2);
            setCarRect(pRes->car2, rect2);



            renderGrassBackground(pRes->pRenderer, pRes->pTiles, 93);
            renderTrackAndObjects(pRes->pRenderer, pRes->pTiles, tilemap);


            renderCar(pRes->pRenderer, pRes->car1);
            renderCar(pRes->pRenderer, pRes->car2);

            renderCar(pRes->pRenderer, &pRes->car1);
            renderCar(pRes->pRenderer, &pRes->car2);

            // Rita ping
            char pingText[64];
            sprintf(pingText, "Ping: %d ms", ping);
            SDL_Color white = {255, 255, 255};
            SDL_Surface *pingSurface = TTF_RenderText_Solid(pRes->pFont, pingText, white);
            SDL_Texture *pingTex = SDL_CreateTextureFromSurface(pRes->pRenderer, pingSurface);
            SDL_Rect pingRect = {20, 20, pingSurface->w, pingSurface->h};
            SDL_RenderCopy(pRes->pRenderer, pingTex, NULL, &pingRect);
            SDL_FreeSurface(pingSurface);
            SDL_DestroyTexture(pingTex);

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
            // Rita svart box och back knapp
            SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerMenuTex, NULL, NULL);
            SDL_RenderCopy(pRes->pRenderer, pRes->pBackToMultiTexture, NULL, &pRes->backMRect);
            SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180); // Transparent black border
            SDL_RenderDrawRect(pRes->pRenderer, &pRes->backMRect);
            SDL_RenderCopy(pRes->pRenderer, pRes->pEnterGameTexture, NULL, &pRes->enterRect);
            SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180); // Transparent black border
            SDL_RenderDrawRect(pRes->pRenderer, &pRes->enterRect);

            // rita input felt (blå)
            SDL_SetRenderDrawColor(pRes->pRenderer, 10, 25, 45, 255); // Blue-ish
            SDL_RenderFillRect(pRes->pRenderer, &pRes->joinRect);

            SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180); // Transparent black border
            SDL_RenderDrawRect(pRes->pRenderer, &pRes->joinRect);

            // Render join ip texten
            SDL_Color white = {255, 255, 255};
            const char *displayIp = strlen(joinIpText) == 0 ? " " : joinIpText;

            SDL_Surface *ipSurf = TTF_RenderText_Solid(pRes->pFont, displayIp, white);
            SDL_Texture *ipTex = SDL_CreateTextureFromSurface(pRes->pRenderer, ipSurf);

            SDL_Rect ipRect = {520, 350, ipSurf->w, ipSurf->h};
            SDL_RenderCopy(pRes->pRenderer, ipTex, NULL, &ipRect);

            SDL_FreeSurface(ipSurf);
            SDL_DestroyTexture(ipTex);

            // rita input host felt (blå)
            SDL_SetRenderDrawColor(pRes->pRenderer, 10, 25, 45, 255); // Blue-ish
            SDL_RenderFillRect(pRes->pRenderer, &pRes->portRect);

            SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180); // Transparent black border
            SDL_RenderDrawRect(pRes->pRenderer, &pRes->portRect);

            SDL_Surface *hostSurf = TTF_RenderText_Solid(pRes->pFont, portText, white);
            SDL_Texture *hostTex = SDL_CreateTextureFromSurface(pRes->pRenderer, hostSurf);

            SDL_Rect hostTextRect = {720, 250, hostSurf->w, hostSurf->h};
            SDL_RenderCopy(pRes->pRenderer, hostTex, NULL, &hostTextRect);

            SDL_FreeSurface(hostSurf);
            SDL_DestroyTexture(hostTex);

            // rita Player Id box
            SDL_SetRenderDrawColor(pRes->pRenderer, 10, 25, 45, 255); // Blue-ish
            SDL_RenderFillRect(pRes->pRenderer, &pRes->playerIdRect);

            SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180); // Transparent black border
            SDL_RenderDrawRect(pRes->pRenderer, &pRes->playerIdRect);

            // rendera player Id texten
            const char *displayId = strlen(playerIdText) == 0 ? " " : playerIdText;

            SDL_Surface *idSurf = TTF_RenderText_Solid(pRes->pFont, displayId, white);
            SDL_Texture *idTex = SDL_CreateTextureFromSurface(pRes->pRenderer, idSurf);

            SDL_Rect idRect = {750, 450, idSurf->w, idSurf->h};
            SDL_RenderCopy(pRes->pRenderer, idTex, NULL, &idRect);

            SDL_FreeSurface(idSurf);
            SDL_DestroyTexture(idTex);
        }

        // Presentera det som ritats
        SDL_RenderPresent(pRes->pRenderer);
        SDL_Delay(16); // cirka 60 FPS
    }

    destroyCar(pRes->car1);
    destroyCar(pRes->car2);

}