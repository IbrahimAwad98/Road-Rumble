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
#include "globals.h"

void gameLoop(GameResources *pRes)
{
    // tillstånd variabeler
    int isMuted = 0;                            // Flagga för ljud av/på
    int musicVolumeLevel = 4;                   // Musikvolym (0–4)
    int sfxLevel = 4;                           // Ljudeffektsvolym (0–4)
    int musicVolumes[5] = {0, 32, 64, 96, 128}; // Steg för musikvolym
    int sfxVolumes[5] = {0, 32, 64, 96, 128};   // Steg för ljudeffekter
    char joinIpText[16] = "";                   // Joina IP
    char playerIdText[4] = "";                  // playerID
    char hostText[32] = " 127.0.0.1";           // Host Texten (lokalt)
    int selectedField = -1;                     // host=0, join=1
    char availableServ[16][5];                  // alla tillgängliga/startade servrar
    char portText[8] = "2000";                  // för att visa i multiplayer meny
    SDL_Event event;
    bool isRunning = true;          // Om spelet ska fortsätta köras
    bool isFullscreen = true;       // flagga
    bool escWasPressedOnce = false; // flagga
    GameMode mode = MENU;           // Startläge: huvudmeny
    MenuMode menuMode = CLASSIC;
    int hoveredButton = -1;            // Vilken menyknapp som musen är över
    Uint32 ping = 0;                   // ping-mätning
    static Uint32 lastPingRequest = 0; // ping-mätning
    // Varv räknanre variabler
    float lastCarX = 0;                              // Spåra senaste bilens position
    float lastCarY = 0;                              // Spåra senaste bilens position
    float prevY[4] = {0};                            // För att spåra föregående Y-position för varje bil
    int laps[4] = {0, 0, 0, 0};                      // antal varv per spelare
    bool crossedStart[4] = {true, true, true, true}; // om finishlinjen korsats
    int winnerID = -1;                               // index för vinnare (–1 = ingen än)
    // justerar automatisk
    SDL_RenderSetLogicalSize(pRes->pRenderer, WIDTH, HEIGHT);

    // Tile bakom 41 är tilemap[4][0]
    float tileRow = 4.7f; // Ändrad till 4.5 för att placera bilarna mellan rad 4 och 5 BANAN
    int tileCol = 1;      // Behåller samma kolumn

    int startX = tileCol * TILE_SIZE;
    int startY = (int)(tileRow * TILE_SIZE); // Konverterar från tile-position till pixel-position.

    // Bil storlek
    int carWidth = 80;
    int carHeight = 48;

    // Centrera bilen i mitten av tilen
    int car1X = (startX + (TILE_SIZE - carWidth) / 2);
    int car1Y = startY + (TILE_SIZE - carHeight) / 2;

    // Bil 2 bredvid bil 1 med mindre utrymme
    int car2X = car1X + carWidth - 35;
    int car2Y = car1Y;

    // Bil 3 och 4 ovanför bil 1 och 2
    int car3X = car1X;
    int car3Y = car1Y - carHeight - 2;
    int car4X = car2X;
    int car4Y = car2Y - carHeight - 2;

    // Initiera bilar
    pRes->pCar1 = createCar(pRes->pRenderer, "resources/Cars/Black_viper.png", car1X, car1Y, carWidth, carHeight);
    pRes->pCar2 = createCar(pRes->pRenderer, "resources/Cars/Mini_truck.png", car2X, car2Y, carWidth, carHeight);
    pRes->pCar3 = createCar(pRes->pRenderer, "resources/Cars/Audi.png", car3X, car3Y, carWidth, carHeight);
    pRes->pCar4 = createCar(pRes->pRenderer, "resources/Cars/car.png", car4X, car4Y, carWidth, carHeight);
    // bil 3
    // bil 4
    if (!pRes->pCar1 || !pRes->pCar2 || !pRes->pCar3 || !pRes->pCar4)
    {
        printf("Failed to create car textures: %s\n", SDL_GetError());
        return;
    }

    setCarAngle(pRes->pCar1, 270.0f);
    setCarSpeed(pRes->pCar1, 0.0f);

    setCarAngle(pRes->pCar2, 270.0f);
    setCarSpeed(pRes->pCar2, 0.0f);

    setCarAngle(pRes->pCar3, 270.0f);
    setCarSpeed(pRes->pCar3, 0.0f);

    setCarAngle(pRes->pCar4, 270.0f);
    setCarSpeed(pRes->pCar4, 0.0f);

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
            if (event.type == SDL_MOUSEBUTTONDOWN && mode == OPTIONS)
            {
                int x = event.button.x, y = event.button.y;

                if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->classicRect))
                {
                    menuMode = (menuMode == CLASSIC) ? DARK : CLASSIC;
                }

                if (menuMode == CLASSIC)
                {
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
                else if (menuMode == DARK)
                {
                    if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->musicVolumeDarkRect))
                    {
                        int seg = (x - pRes->musicVolumeDarkRect.x) / (pRes->musicVolumeDarkRect.w / 5);
                        musicVolumeLevel = (seg < 0) ? 0 : (seg > 4 ? 4 : seg);
                        Mix_VolumeMusic(musicVolumes[musicVolumeLevel]);
                    }
                    if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->SfxDarkRect))
                    {
                        int seg = (x - pRes->SfxDarkRect.x) / (pRes->SfxDarkRect.w / 5);
                        sfxLevel = (seg < 0) ? 0 : (seg > 4 ? 4 : seg);
                        Mix_VolumeMusic(sfxVolumes[sfxLevel]);
                    }
                    if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->backDarkRect))
                    {
                        mode = MENU;
                    }
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

                if (menuMode == CLASSIC)
                {

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
                            PlayerID = atoi(playerIdText) - 1;

                            if (initClient(joinIpText, SERVER_PORT))
                            {
                                printf("Connected to %s successfully as Player %d!\n", joinIpText, PlayerID);
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
                else if (menuMode == DARK)
                {
                    if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->backMDarkRect))
                    {
                        mode = MENU;
                        selectedField = -1;
                        SDL_StopTextInput();
                        joinIpText[0] = '\0';
                        playerIdText[0] = '\0';
                    }
                    else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->joinDarkRect))
                    {
                        selectedField = 1;
                        SDL_StartTextInput();
                    }
                    else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->playerIdDarkRect))
                    {
                        selectedField = 2;
                        SDL_StartTextInput();
                    }
                    else if (SDL_PointInRect(&(SDL_Point){x, y}, &pRes->enterDarkRect))
                    {
                        if (strlen(joinIpText) > 0 && strlen(playerIdText) > 0)
                        {
                            PlayerID = atoi(playerIdText) - 1; // <- Set playerID globally!

                            if (initClient(joinIpText, SERVER_PORT))
                            {
                                printf("Connected to %s successfully as Player %d!\n", joinIpText, PlayerID);
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
        }
        // Rendering
        SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 255);
        SDL_RenderClear(pRes->pRenderer);

        // Huvudmenyn
        if (mode == MENU)
        {
            if (menuMode == CLASSIC)
            {
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
            if (menuMode == DARK)
            {
                SDL_RenderCopy(pRes->pRenderer, pRes->pBackgroundDarkTexture, NULL, NULL);

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
        }

        resolveCollision(pRes->pCar1, pRes->pCar2);
        resolveCollision(pRes->pCar1, pRes->pCar3);
        resolveCollision(pRes->pCar1, pRes->pCar4);
        resolveCollision(pRes->pCar2, pRes->pCar3);
        resolveCollision(pRes->pCar2, pRes->pCar4);
        resolveCollision(pRes->pCar3, pRes->pCar4);

        // Spelläget (via nätverk)
        Car *cars[4] = {pRes->pCar1, pRes->pCar2, pRes->pCar3, pRes->pCar4};

        if (mode == PLAYING)
        {
            SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 255); // rensa skärmen
            SDL_RenderClear(pRes->pRenderer);
            const Uint8 *keys = SDL_GetKeyboardState(NULL); // läs tangentbord

            Uint32 now = SDL_GetTicks();
            if (now - lastPingRequest >= 1000)
            {
                PlayerData pingRequest = {0};
                pingRequest.playerID = PlayerID;
                pingRequest.timestamp = now;
                pingRequest.isPing = 1;
                client_sendPlayerData(&pingRequest);
                lastPingRequest = now;
            }

            // === Uppdatera min egen bil ===
            if (PlayerID >= 0 && PlayerID < 4)
            {
                updateCar(cars[PlayerID], keys, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D);
                // Varv räknanre logik
                float currentX = getCarX(cars[PlayerID]);
                float currentY = getCarY(cars[PlayerID]);

                // Kontrollera om vi korsar start/mållinjen (tile 41)
                int tileCol = (int)(currentX / TILE_SIZE);
                int tileRow = (int)(currentY / TILE_SIZE);
            }

            // === Skicka min position ===
            PlayerData myData = {0};
            myData.playerID = PlayerID;

            if (PlayerID >= 0 && PlayerID < 4)
            {
                myData.x = getCarX(cars[PlayerID]);
                myData.y = getCarY(cars[PlayerID]);
                myData.angle = getCarAngle(cars[PlayerID]);
            }

            client_sendPlayerData(&myData);

            // === Ta emot andra spelares positioner ===
            PlayerData opponentData = {0};

            while (client_receiveServerData(&opponentData))
            {
                // Om det är ett ping-svar till mig
                if (opponentData.isPing == 1 && opponentData.playerID == PlayerID)
                {
                    ping = SDL_GetTicks() - opponentData.timestamp;
                    continue;
                }

                // Vanlig spelardata
                if (opponentData.playerID == PlayerID)
                {
                    continue;
                }

                if (opponentData.playerID >= 0 && opponentData.playerID < 4)
                {
                    setCarPosition(cars[opponentData.playerID], opponentData.x, opponentData.y, opponentData.angle);
                }
            }
            // Uppdatera lap-count
            if (winnerID < 0)
            {
                for (int i = 0; i < 4; i++)
                {
                    float x = getCarX(cars[i]);
                    float y = getCarY(cars[i]);
                    int col = (int)(x / TILE_SIZE);
                    int row = (int)(y / TILE_SIZE);

                    if (tilemap[row][col] == 41 && !crossedStart[i] && y < prevY[i])
                    {
                        crossedStart[i] = true;
                        laps[i]++;
                        if (laps[i] >= 3)
                        {
                            winnerID = i;
                            break;
                        }
                    }
                    else if (tilemap[row][col] != 41)
                    {
                        crossedStart[i] = false;
                    }
                    // Uppdatera föregående Y-position
                    prevY[i] = y;
                }
            }

            if (winnerID >= 0)
            {
                // Bygg texten “Winner PlayerX!”
                char winText[32];
                sprintf(winText, "Winner Player%d!", winnerID + 1);

                // Rita banan + bilar
                SDL_RenderClear(pRes->pRenderer);
                renderGrassBackground(pRes->pRenderer, pRes->pTiles, 93);
                renderTrackAndObjects(pRes->pRenderer, pRes->pTiles, tilemap);
                for (int i = 0; i < 4; i++)
                {
                    renderCar(pRes->pRenderer, cars[i]);
                }

                // Rendera vinnartext mitt på skärmen
                SDL_Color green = {0, 255, 0};
                SDL_Surface *surf = TTF_RenderText_Solid(pRes->pFont, winText, green);
                SDL_Texture *tex = SDL_CreateTextureFromSurface(pRes->pRenderer, surf);
                SDL_Rect rect = {
                    WIDTH / 2 - surf->w / 2,
                    HEIGHT / 2 - surf->h / 2,
                    surf->w,
                    surf->h};
                SDL_FreeSurface(surf);
                SDL_RenderCopy(pRes->pRenderer, tex, NULL, &rect);
                SDL_DestroyTexture(tex);

                // Visa och vänta 3 sekunder
                SDL_RenderPresent(pRes->pRenderer);
                SDL_Delay(3000);

                // Återställ för nästa omgång och gå till huvudmeny
                mode = MENU;
                for (int i = 0; i < 4; i++)
                {
                    laps[i] = 0;
                    crossedStart[i] = true;
                }
                winnerID = -1;

                continue; // hoppa över övrig rendering denna frame
            }

            // === Rendera spelvärlden ===
            renderGrassBackground(pRes->pRenderer, pRes->pTiles, 93);
            renderTrackAndObjects(pRes->pRenderer, pRes->pTiles, tilemap);

            // Rita alla bilar
            for (int i = 0; i < 4; i++)
            {
                renderCar(pRes->pRenderer, cars[i]);
            }

            // Rita ping
            char pingText[64];
            sprintf(pingText, "%dms", (int)ping);
            SDL_Color color;
            if (ping < 30)
            {
                color = (SDL_Color){0, 255, 0}; // Grön
            }
            else if (ping < 80)
            {
                color = (SDL_Color){255, 255, 0}; // Gul
            }
            else
            {
                color = (SDL_Color){255, 0, 0}; // Röd
            }
            SDL_Surface *pingSurface = TTF_RenderText_Solid(pRes->pFont, pingText, color);
            SDL_Texture *pingTex = SDL_CreateTextureFromSurface(pRes->pRenderer, pingSurface);
            SDL_Rect pingRect = {10, 10, pingSurface->w, pingSurface->h};
            SDL_RenderCopy(pRes->pRenderer, pingTex, NULL, &pingRect);
            SDL_FreeSurface(pingSurface);
            SDL_DestroyTexture(pingTex);

            // Visa egen spelares varv i HUD med laps[PlayerID]
            {
                char lapText[32];
                sprintf(lapText, "%d/3", laps[PlayerID]);
                SDL_Color white = {255, 255, 255};
                SDL_Surface *lapSurface = TTF_RenderText_Solid(pRes->pFont, lapText, white);
                SDL_Texture *lapTex = SDL_CreateTextureFromSurface(pRes->pRenderer, lapSurface);
                SDL_Rect lapRect = {
                    WIDTH - lapSurface->w / 2 - 10,
                    110,
                    lapSurface->w / 2,
                    lapSurface->h};
                SDL_FreeSurface(lapSurface);
                SDL_RenderCopy(pRes->pRenderer, lapTex, NULL, &lapRect);
                SDL_DestroyTexture(lapTex);
            }
        }

        //  Inställningsmeny
        else if (mode == OPTIONS)
        {
            if (menuMode == CLASSIC)
            {
                // Full background with texts already inside
                SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsMenuTex, NULL, NULL);
                SDL_RenderCopy(pRes->pRenderer, pRes->pBackToMenuTexture, NULL, &pRes->backRect);
                // Draw volume bars (same for both modes)
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
            else if (menuMode == DARK)
            {
                SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsMenuDarkTex, NULL, NULL);
                SDL_RenderCopy(pRes->pRenderer, pRes->pBackToMenuDarkTexture, NULL, &pRes->backDarkRect);
                // Draw volume bars (same for both modes)
                for (int i = 0; i < 5; i++)
                {
                    SDL_Rect block = {
                        pRes->musicVolumeDarkRect.x + i * (pRes->musicVolumeDarkRect.w / 5),
                        pRes->musicVolumeDarkRect.y,
                        (pRes->musicVolumeDarkRect.w / 5) - 4,
                        pRes->musicVolumeDarkRect.h};
                    SDL_SetRenderDrawColor(pRes->pRenderer, (i <= musicVolumeLevel) ? 80 : 20, 160, 220, 255);
                    SDL_RenderFillRect(pRes->pRenderer, &block);
                }

                for (int i = 0; i < 5; i++)
                {
                    SDL_Rect block = {
                        pRes->SfxDarkRect.x + i * (pRes->SfxDarkRect.w / 5),
                        pRes->SfxDarkRect.y,
                        (pRes->SfxDarkRect.w / 5) - 4,
                        pRes->SfxDarkRect.h};
                    SDL_SetRenderDrawColor(pRes->pRenderer, (i <= sfxLevel) ? 80 : 20, 160, 220, 255);
                    SDL_RenderFillRect(pRes->pRenderer, &block);
                }
            }
        }

        //  Multiplayer-meny
        else if (mode == MULTIPLAYER)
        {

            if (menuMode == CLASSIC)
            {
                // === Classic Multiplayer menu ===
                SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerMenuTex, NULL, NULL);
                SDL_RenderCopy(pRes->pRenderer, pRes->pBackToMultiTexture, NULL, &pRes->backMRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->backMRect);

                // Enter game button
                SDL_RenderCopy(pRes->pRenderer, pRes->pEnterGameTexture, NULL, &pRes->enterRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->enterRect);
                // Join IP input box
                SDL_SetRenderDrawColor(pRes->pRenderer, 10, 25, 45, 255);
                SDL_RenderFillRect(pRes->pRenderer, &pRes->joinRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->joinRect);

                // Render Join IP text
                SDL_Color white = {255, 255, 255};
                const char *displayIp = strlen(joinIpText) == 0 ? " " : joinIpText;

                SDL_Surface *ipSurf = TTF_RenderText_Solid(pRes->pFont, displayIp, white);
                SDL_Texture *ipTex = SDL_CreateTextureFromSurface(pRes->pRenderer, ipSurf);
                SDL_Rect ipRect = {560, 350, ipSurf->w, ipSurf->h};
                SDL_RenderCopy(pRes->pRenderer, ipTex, NULL, &ipRect);
                SDL_FreeSurface(ipSurf);
                SDL_DestroyTexture(ipTex);

                // Port input box (readonly)
                SDL_SetRenderDrawColor(pRes->pRenderer, 10, 25, 45, 255);
                SDL_RenderFillRect(pRes->pRenderer, &pRes->portRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->portRect);

                snprintf(portText, sizeof(portText), "%d", SERVER_PORT);
                SDL_Surface *hostSurf = TTF_RenderText_Solid(pRes->pFont, portText, white);
                SDL_Texture *hostTex = SDL_CreateTextureFromSurface(pRes->pRenderer, hostSurf);
                SDL_Rect hostTextRect = {720, 250, hostSurf->w, hostSurf->h};
                SDL_RenderCopy(pRes->pRenderer, hostTex, NULL, &hostTextRect);
                SDL_FreeSurface(hostSurf);
                SDL_DestroyTexture(hostTex);

                // Player ID input box
                SDL_SetRenderDrawColor(pRes->pRenderer, 10, 25, 45, 255);
                SDL_RenderFillRect(pRes->pRenderer, &pRes->playerIdRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->playerIdRect);

                const char *displayId = strlen(playerIdText) == 0 ? " " : playerIdText;
                SDL_Surface *idSurf = TTF_RenderText_Solid(pRes->pFont, displayId, white);
                SDL_Texture *idTex = SDL_CreateTextureFromSurface(pRes->pRenderer, idSurf);
                SDL_Rect idRect = {750, 450, idSurf->w, idSurf->h};
                SDL_RenderCopy(pRes->pRenderer, idTex, NULL, &idRect);
                SDL_FreeSurface(idSurf);
                SDL_DestroyTexture(idTex);
            }
            else if (menuMode == DARK)
            {
                // === Dark Multiplayer menu ===
                SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerMenuDarkTex, NULL, NULL);
                SDL_RenderCopy(pRes->pRenderer, pRes->pBackToMultiDarkTexture, NULL, &pRes->backMDarkRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->backMDarkRect);

                // Enter game button
                SDL_RenderCopy(pRes->pRenderer, pRes->pEnterGameDarkTexture, NULL, &pRes->enterDarkRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->enterDarkRect);
                // Join IP input box
                SDL_SetRenderDrawColor(pRes->pRenderer, 5, 10, 20, 255);
                SDL_RenderFillRect(pRes->pRenderer, &pRes->joinDarkRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->joinDarkRect);

                // Render Join IP text
                SDL_Color white = {255, 255, 255};
                const char *displayIp = strlen(joinIpText) == 0 ? " " : joinIpText;

                SDL_Surface *ipSurf = TTF_RenderText_Solid(pRes->pFont, displayIp, white);
                SDL_Texture *ipTex = SDL_CreateTextureFromSurface(pRes->pRenderer, ipSurf);
                SDL_Rect ipRect = {560, 350, ipSurf->w, ipSurf->h};
                SDL_RenderCopy(pRes->pRenderer, ipTex, NULL, &ipRect);
                SDL_FreeSurface(ipSurf);
                SDL_DestroyTexture(ipTex);

                // Port input box (readonly)
                SDL_SetRenderDrawColor(pRes->pRenderer, 5, 10, 20, 255);
                SDL_RenderFillRect(pRes->pRenderer, &pRes->portDarkRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->portDarkRect);

                snprintf(portText, sizeof(portText), "%d", SERVER_PORT);
                SDL_Surface *hostSurf = TTF_RenderText_Solid(pRes->pFont, portText, white);
                SDL_Texture *hostTex = SDL_CreateTextureFromSurface(pRes->pRenderer, hostSurf);
                SDL_Rect hostTextRect = {720, 265, hostSurf->w, hostSurf->h};
                SDL_RenderCopy(pRes->pRenderer, hostTex, NULL, &hostTextRect);
                SDL_FreeSurface(hostSurf);
                SDL_DestroyTexture(hostTex);

                // Player ID input box
                SDL_SetRenderDrawColor(pRes->pRenderer, 5, 10, 20, 255);
                SDL_RenderFillRect(pRes->pRenderer, &pRes->playerIdDarkRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->playerIdDarkRect);

                const char *displayId = strlen(playerIdText) == 0 ? " " : playerIdText;
                SDL_Surface *idSurf = TTF_RenderText_Solid(pRes->pFont, displayId, white);
                SDL_Texture *idTex = SDL_CreateTextureFromSurface(pRes->pRenderer, idSurf);
                SDL_Rect idRect = {750, 435, idSurf->w, idSurf->h};
                SDL_RenderCopy(pRes->pRenderer, idTex, NULL, &idRect);
                SDL_FreeSurface(idSurf);
                SDL_DestroyTexture(idTex);
            }
        }

        // Presentera det som ritats
        SDL_RenderPresent(pRes->pRenderer);
        SDL_Delay(16); // cirka 60 FPS
    }
}
