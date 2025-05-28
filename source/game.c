#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include "game.h"
#include "car.h"
#include "tilemap.h"
#include "client.h"
#include "server.h"
#include "network.h"
#include "globals.h"
#include "math.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Boost-rendering
void renderOpponentBoostFlame(GameResources *pRes, Car *car, int frame)
{
    float angle = getCarAngle(car);
    float radians = angle * (M_PI / 180.0f);
    float centerX = getCarX(car) + getCarWidth(car) / 2;
    float centerY = getCarY(car) + getCarHeight(car) / 2;
    float offset = getCarHeight(car) / 2 + 10;
    float flameX = centerX - cos(radians) * offset;
    float flameY = centerY - sin(radians) * offset;
    int spriteWidth = 50;
    int spriteHeight = 50;

    SDL_Rect flameRect = {
        (int)(flameX - spriteWidth / 2),
        (int)(flameY - spriteHeight / 2),
        spriteWidth,
        spriteHeight};

    SDL_Point flameCenter = {spriteWidth / 2, spriteHeight / 2};
    SDL_Texture *currentFlame = pRes->pBoostFlameFrames[frame];

    SDL_SetTextureAlphaMod(currentFlame, 200);
    SDL_RenderCopyEx(pRes->pRenderer, currentFlame, NULL, &flameRect, angle, &flameCenter, SDL_FLIP_NONE);
    SDL_SetTextureAlphaMod(currentFlame, 255);
}

// Bil initiering
void initCars(GameResources *pRes)
{
    int carWidth = 80, carHeight = 48;
    int tileCol = 1;
    float tileRow = 4.7f;
    int startX = tileCol * TILE_SIZE;
    int startY = (int)(tileRow * TILE_SIZE);

    // Startpositioner för varje bil
    int car1X = startX + 2, car1Y = startY + 20;
    int car2X = car1X + carWidth - 28, car2Y = car1Y;
    int car3X = car1X, car3Y = car1Y - carHeight - 2;
    int car4X = car2X, car4Y = car2Y - carHeight - 2;

    // Initiera bilar
    pRes->pCar1 = createCar(pRes->pRenderer, "resources/Cars/Black_viper.png", car1X, car1Y, carWidth, carHeight);
    pRes->pCar2 = createCar(pRes->pRenderer, "resources/Cars/Mini_truck.png", car2X, car2Y, carWidth, carHeight);
    pRes->pCar3 = createCar(pRes->pRenderer, "resources/Cars/Audi.png", car3X, car3Y, carWidth, carHeight);
    pRes->pCar4 = createCar(pRes->pRenderer, "resources/Cars/car.png", car4X, car4Y, carWidth, carHeight);

    // Kontrollera om laddning misslyckades
    if (!pRes->pCar1 || !pRes->pCar2 || !pRes->pCar3 || !pRes->pCar4)
    {
        printf("Failed to create car textures: %s\n", SDL_GetError());
        return;
    }

    // Initiera startvinklar och hastighet
    setCarAngle(pRes->pCar1, 270.0f);
    setCarSpeed(pRes->pCar1, 0.0f);
    setCarAngle(pRes->pCar2, 270.0f);
    setCarSpeed(pRes->pCar2, 0.0f);
    setCarAngle(pRes->pCar3, 270.0f);
    setCarSpeed(pRes->pCar3, 0.0f);
    setCarAngle(pRes->pCar4, 270.0f);
    setCarSpeed(pRes->pCar4, 0.0f);
}

void gameLoop(GameResources *pRes)
{
    // Allmänna tillståndsvariabler
    int isMuted = 0;
    int musicVolumeLevel = 4;
    int sfxLevel = 4;
    int musicVolumes[5] = {0, 32, 64, 96, 128};
    int sfxVolumes[5] = {0, 32, 64, 96, 128};

    // Multiplayer och IP
    char joinIpText[16] = "";
    char playerIdText[4] = "";
    char availableServ[16][5];
    char portText[8] = "2000";
    int selectedField = -1;

    // Fönster- och loopstatus
    SDL_Event event;
    bool isRunning = true;
    bool isFullscreen = true;
    bool escWasPressedOnce = false;

    // Meny- och spelläge
    GameMode mode = MENU;
    MenuMode menuMode = CLASSIC;
    ControllerMode cMode = WASD;
    int hoveredButton = -1;

    // Nätverk och ping
    Uint32 ping = 0;
    static Uint32 lastPingRequest = 0;

    // Varvhantering
    float lastCarX = 0, lastCarY = 0;
    float prevY[4] = {0};
    int laps[4] = {0, 0, 0, 0};
    bool crossedStart[4] = {true, true, true, true};
    bool wasOnFinish[4] = {false};
    int winnerID = -1;

    // Boostsystem
    bool boostActive[4] = {false};
    Uint32 boostStart[4] = {0};
    Uint32 lastBoostUsed[4] = {0};
    bool boostAvailable[4] = {false};
    bool boostUsed[4] = {false};
    const Uint32 boostDuration = 5000;
    static int boostFrame = 0;
    static Uint32 lastBoostFrameTime = 0;
    const Uint32 BOOST_ANIM_SPEED = 60;
    pRes->raceStarted = false;
    pRes->countdownStarted = false;
    pRes->countdownStartTime = 0;
    pRes->COUNTDOWN_TOTAL_DURATION = 4000;
    bool raceHasBegun;

    // Fönsterstorlek
    SDL_RenderSetLogicalSize(pRes->pRenderer, WIDTH, HEIGHT);

    // Bilarnas startkoordinater
    initCars(pRes);

    // Huvudloop
    while (isRunning)
    {
        //  Händelsehantering
        while (SDL_PollEvent(&event))
        {
            // Avsluta spel
            if (event.type == SDL_QUIT)
                isRunning = false;
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
                    isRunning = false; // Andra trycket, avsluta spelet
            }
            // Menyinteraktion med musen
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
            if (event.type == SDL_KEYDOWN)
                mode = (event.key.keysym.sym == SDLK_p) ? PLAYING : (event.key.keysym.sym == SDLK_m) ? MENU
                                                                                                     : mode;
            if (event.type == SDL_MOUSEBUTTONDOWN && mode == OPTIONS)
            {
                int x = event.button.x, y = event.button.y;

                if (menuMode == CLASSIC && SDL_PointInRect(&(SDL_Point){x, y}, &pRes->classicRect))
                    menuMode = DARK;
                else if (menuMode == DARK && SDL_PointInRect(&(SDL_Point){x, y}, &pRes->darkRect))
                    menuMode = CLASSIC;
                if (menuMode == CLASSIC && SDL_PointInRect(&(SDL_Point){x, y}, &pRes->WASDRect))
                    cMode = (cMode == WASD) ? ARROWS : WASD;
                else if (menuMode == DARK && SDL_PointInRect(&(SDL_Point){x, y}, &pRes->WASDDarkRect))
                    cMode = (cMode == WASD) ? ARROWS : WASD;

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
                        mode = MENU;
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
            if (event.type == SDL_TEXTINPUT && mode == MULTIPLAYER)
            {
                if (selectedField == 1 && strlen(joinIpText) < sizeof(joinIpText) - 1)
                    strcat(joinIpText, event.text.text);
                else if (selectedField == 2 && strlen(playerIdText) < sizeof(playerIdText) - 1)
                    strcat(playerIdText, event.text.text);
            }
            if (event.type == SDL_KEYDOWN && mode == MULTIPLAYER && selectedField == 1)
            {
                if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(joinIpText) > 0)
                    joinIpText[strlen(joinIpText) - 1] = '\0';
                else if (event.key.keysym.sym == SDLK_RETURN)
                {
                    if (initClient(joinIpText, SERVER_PORT))
                        mode = PLAYING;
                    else
                        printf("Failed to connect to server at %s\n", joinIpText);
                }
            }
            if (event.type == SDL_KEYDOWN && mode == MULTIPLAYER && selectedField == 2)
            {
                if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(playerIdText) > 0)
                    playerIdText[strlen(playerIdText) - 1] = '\0';
            }
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
                                mode = PLAYING;
                            else
                                printf("Failed to connect to server at %s\n", joinIpText);
                        }
                        else
                            printf("Please fill IP address and Player ID!\n");
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
                            PlayerID = atoi(playerIdText) - 1;

                            if (initClient(joinIpText, SERVER_PORT))
                                mode = PLAYING;
                            else
                                printf("Failed to connect to server at %s\n", joinIpText);
                        }
                        else
                            printf("Please fill IP address and Player ID!\n");
                    }
                    else
                    {
                        selectedField = -1;
                        SDL_StopTextInput();
                    }
                }
            }
        }
        // Rendering: rensa skärm
        SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 255);
        SDL_RenderClear(pRes->pRenderer);
        // Rendering: menyer och spellägen
        if (mode == MENU)
        {
            // Meny: rendera UI baserat på theme
            if (menuMode == CLASSIC)
            {
                SDL_RenderCopy(pRes->pRenderer, pRes->pBackgroundTexture, NULL, NULL);

                // Färgtoning för hovereffekt
                SDL_SetTextureColorMod(pRes->pStartTexture, hoveredButton == 0 ? 200 : 255, hoveredButton == 0 ? 200 : 255, 255);
                SDL_SetTextureColorMod(pRes->pMultiplayerTexture, hoveredButton == 1 ? 200 : 255, hoveredButton == 1 ? 200 : 255, 255);
                SDL_SetTextureColorMod(pRes->pOptionsTexture, hoveredButton == 2 ? 200 : 255, hoveredButton == 2 ? 200 : 255, 255);
                SDL_SetTextureColorMod(pRes->pExitTexture, hoveredButton == 3 ? 200 : 255, hoveredButton == 3 ? 200 : 255, 255);
                SDL_SetTextureColorMod(isMuted ? pRes->pMuteTexture : pRes->pUnmuteTexture, hoveredButton == 4 ? 200 : 255, hoveredButton == 4 ? 200 : 255, 255);

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
                SDL_SetTextureColorMod(isMuted ? pRes->pMuteTexture : pRes->pUnmuteTexture, hoveredButton == 4 ? 200 : 255, hoveredButton == 4 ? 200 : 255, 255);

                // Rendera knappar
                SDL_RenderCopy(pRes->pRenderer, pRes->pStartTexture, NULL, &pRes->startRect);
                SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerTexture, NULL, &pRes->multiplayerRect);
                SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsTexture, NULL, &pRes->optionsRect);
                SDL_RenderCopy(pRes->pRenderer, pRes->pExitTexture, NULL, &pRes->exitRect);
                SDL_RenderCopy(pRes->pRenderer, isMuted ? pRes->pMuteTexture : pRes->pUnmuteTexture, NULL, &pRes->muteRect);
            }
        }

        // Skapa bil-array
        Car *cars[4] = {pRes->pCar1, pRes->pCar2, pRes->pCar3, pRes->pCar4};

        // Kollisioner mellan bilpar
        for (int i = 0; i < 4; i++)
        {
            for (int j = i + 1; j < 4; j++)
            {
                resolveCollision(cars[i], cars[j]);
            }
        }

        // Hela spelets grafik, biluppdateringar, kollisionskontroll
        // Spelrendering: tilemap, bilar, boost, trail, ping, varv, vinnare
        if (mode == PLAYING)
        {
            SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 255);
            SDL_RenderClear(pRes->pRenderer);
            const Uint8 *keys = SDL_GetKeyboardState(NULL);

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

            raceHasBegun = (pRes->countdownStarted && SDL_GetTicks() - pRes->countdownStartTime >= pRes->COUNTDOWN_TOTAL_DURATION) ? true : false;

            // Uppdatera min egen bil
            if (PlayerID >= 0 && PlayerID < 4)
            {
                if (raceHasBegun)
                {
                    float boostFactor = boostActive[PlayerID] ? 2.0f : 1.0f;
                    if (cMode == WASD)
                        updateCar(cars[PlayerID], keys, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D, boostFactor);
                    else if (cMode == ARROWS)
                        updateCar(cars[PlayerID], keys, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, boostFactor);
                }
                if (boostActive[PlayerID] && SDL_GetTicks() - boostStart[PlayerID] > boostDuration)
                    boostActive[PlayerID] = false;

                // Varv räknanre logik
                float currentX = getCarX(cars[PlayerID]);
                float currentY = getCarY(cars[PlayerID]);

                // Kontrollera om vi korsar start/mållinjen (tile 41)
                int tileCol = (int)(currentX / TILE_SIZE);
                int tileRow = (int)(currentY / TILE_SIZE);
            }

            // Skicka min position
            PlayerData myData = {0};
            myData.playerID = PlayerID;

            if (PlayerID >= 0 && PlayerID < 4)
            {
                myData.x = getCarX(cars[PlayerID]);
                myData.y = getCarY(cars[PlayerID]);
                myData.angle = getCarAngle(cars[PlayerID]);
                myData.speed = getCarSpeed(cars[PlayerID]);
                myData.isBoosting = boostActive[PlayerID];
                const Uint8 *keys = SDL_GetKeyboardState(NULL);
                bool left = (cMode == WASD) ? keys[SDL_SCANCODE_A] : keys[SDL_SCANCODE_LEFT];
                bool right = (cMode == WASD) ? keys[SDL_SCANCODE_D] : keys[SDL_SCANCODE_RIGHT];

                float speed = getCarSpeed(cars[PlayerID]);
                myData.isDrifting = ((left || right) && fabs(speed) > 1.2f);
            }

            client_sendPlayerData(&myData);

            // Ta emot andra spelares positioner
            PlayerData opponentData = {0};

            while (client_receiveServerData(&opponentData))
            {
                if (opponentData.isStartSignal)
                {
                    if (!pRes->countdownStarted)
                    {
                        pRes->countdownStarted = true;
                        pRes->countdownStartTime = SDL_GetTicks();
                        printf("Received START signal. Countdown begins.\n");
                    }
                    continue; // Hoppa över vanlig positionsuppdatering (t.ex. för ping-svar eller lokal spelare)
                }
                if (opponentData.isPing == 1 && opponentData.playerID == PlayerID)
                {
                    ping = SDL_GetTicks() - opponentData.timestamp;
                    continue;
                }
                if (opponentData.playerID == PlayerID || opponentData.playerID < 0 || opponentData.playerID > 3)
                    continue;

                Car *remoteCar = cars[opponentData.playerID];
                setCarPosition(remoteCar, opponentData.x, opponentData.y, opponentData.angle);
                setCarSpeed(remoteCar, opponentData.speed);
                setCarDrifting(remoteCar, opponentData.isDrifting);
                boostActive[opponentData.playerID] = opponentData.isBoosting;

                // Rendera drift-spår och boost-låga direkt baserat på mottagen data från nätverket
                addTrailIfDrifting(remoteCar);

                if (boostActive[opponentData.playerID])
                {
                    Uint32 now = SDL_GetTicks();
                    if (now - lastBoostFrameTime >= BOOST_ANIM_SPEED)
                    {
                        boostFrame = (boostFrame + 1) % BOOST_FRAME_COUNT;
                        lastBoostFrameTime = now;
                    }
                    printf("Rendering boost flame for player %d\n", opponentData.playerID);
                    renderOpponentBoostFlame(pRes, remoteCar, boostFrame);
                }
            }

            // Rendera spelvärlden
            renderGrassBackground(pRes->pRenderer, pRes->pTiles, 93);
            renderTrackAndObjects(pRes->pRenderer, pRes->pTiles, tilemap, laps[PlayerID], pRes);

            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < getTrailCount(cars[i]); j++)
                {
                    int x = getTrailMarkX(cars[i], j);
                    int y = getTrailMarkY(cars[i], j);
                    float angle = getTrailMarkAngle(cars[i], j);
                    SDL_Rect trailRect =
                        {
                            x - 12,
                            y - 12,
                            24, 24};
                    SDL_Point center = {trailRect.w / 2, trailRect.h / 2};
                    int trailCount = getTrailCount(cars[i]);
                    if (trailCount > 1)
                    {
                        int alpha = (int)(255.0 * ((float)j / (trailCount - 1)));
                        SDL_SetTextureAlphaMod(pRes->pTireTrailTexture, alpha);
                    }
                    SDL_RenderCopyEx(pRes->pRenderer, pRes->pTireTrailTexture, NULL, &trailRect, angle, &center, SDL_FLIP_NONE);
                }

                renderCar(pRes->pRenderer, cars[i]);
            }
            SDL_SetTextureAlphaMod(pRes->pTireTrailTexture, 255);

            float px = getCarX(cars[PlayerID]);
            float py = getCarY(cars[PlayerID]);
            int pcol = (int)(px / TILE_SIZE);
            int prow = (int)(py / TILE_SIZE);

            // Aktivera boost
            if (prow >= 0 && prow < MAP_HEIGHT &&
                pcol >= 0 && pcol < MAP_WIDTH &&
                tilemap[prow][pcol] == 9 &&
                laps[PlayerID] == 2 &&
                !boostUsed[PlayerID] && !boostAvailable[PlayerID])
            {
                boostAvailable[PlayerID] = true;
                printf("BOOST unlocked! You can use it anytime now.\n");
            }

            if (boostAvailable[PlayerID] &&
                !boostUsed[PlayerID] &&
                !boostActive[PlayerID] &&
                keys[SDL_SCANCODE_RSHIFT])
            {
                boostActive[PlayerID] = true;
                boostUsed[PlayerID] = true;
                boostStart[PlayerID] = SDL_GetTicks();
                printf("BOOST activated by SHIFT!\n");
            }

            for (int i = 0; i < 4; i++)
            {
                if (boostActive[i])
                {
                    float angle = getCarAngle(cars[i]);
                    float radians = angle * (M_PI / 180.0f);

                    float centerX = getCarX(cars[i]) + getCarWidth(cars[i]) / 2;
                    float centerY = getCarY(cars[i]) + getCarHeight(cars[i]) / 2;

                    float offset = getCarHeight(cars[i]) / 2 + 10;
                    float flameX = centerX - cos(radians) * offset;
                    float flameY = centerY - sin(radians) * offset;

                    int spriteWidth = 50;
                    int spriteHeight = 50;

                    SDL_Rect flameRect = {
                        (int)(flameX - spriteWidth / 2),
                        (int)(flameY - spriteHeight / 2),
                        spriteWidth,
                        spriteHeight};

                    SDL_Point flameCenter = {spriteWidth / 2, spriteHeight / 2};

                    Uint32 now = SDL_GetTicks();
                    if (now - lastBoostFrameTime >= BOOST_ANIM_SPEED)
                    {
                        boostFrame = (boostFrame + 1) % BOOST_FRAME_COUNT;
                        lastBoostFrameTime = now;
                    }

                    SDL_Texture *currentFlame = pRes->pBoostFlameFrames[boostFrame];
                    SDL_SetTextureAlphaMod(currentFlame, 200);
                    SDL_RenderCopyEx(pRes->pRenderer, currentFlame, NULL, &flameRect, angle, &flameCenter, SDL_FLIP_NONE);
                    SDL_SetTextureAlphaMod(currentFlame, 255);
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

                    if (row < 0 || row >= MAP_HEIGHT || col < 0 || col >= MAP_WIDTH)
                        continue;

                    bool currentlyOnFinish = (tilemap[row][col] == 7);
                    if (currentlyOnFinish && !wasOnFinish[i])
                    {
                        laps[i]++;
                        if (laps[i] > 3)
                        {
                            winnerID = i;
                        }
                    }
                    wasOnFinish[i] = currentlyOnFinish;
                }
            }

            if (winnerID >= 0)
            {
                // vinnare
                char winText[32];
                sprintf(winText, "Winner Player%d!", winnerID + 1);

                // Rita banan + bilar
                SDL_RenderClear(pRes->pRenderer);
                renderGrassBackground(pRes->pRenderer, pRes->pTiles, 93);
                renderTrackAndObjects(pRes->pRenderer, pRes->pTiles, tilemap, laps[PlayerID], pRes);

                for (int i = 0; i < 4; i++)
                    renderCar(pRes->pRenderer, cars[i]);

                // Rendera vinnartext mitt på skärmen
                SDL_Color green = {0, 255, 0};
                SDL_Surface *surf = TTF_RenderText_Solid(pRes->pFont, winText, green);
                SDL_Texture *tex = SDL_CreateTextureFromSurface(pRes->pRenderer, surf);
                SDL_Rect rect = {WIDTH / 2 - surf->w / 2, HEIGHT / 2 - surf->h / 2, surf->w, surf->h};
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
                    boostAvailable[i] = false;
                    boostUsed[i] = false;
                    boostActive[i] = false;
                }
                winnerID = -1;

                // Återställ bilarnas startpositioner och egenskaper efter vinst
                initCars(pRes);
                continue; // hoppa över övrig rendering denna frame
            }

            // Rita ping
            char pingText[64];
            sprintf(pingText, "%dms", (int)ping);
            SDL_Color color;
            if (ping < 30)
                color = (SDL_Color){0, 255, 0}; // Grön
            else if (ping < 80)
                color = (SDL_Color){255, 255, 0}; // Gul
            else
                color = (SDL_Color){255, 0, 0}; // Röd

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
                SDL_Rect lapRect = {WIDTH - lapSurface->w / 2 - 10, 110, lapSurface->w / 2, lapSurface->h};
                SDL_FreeSurface(lapSurface);
                SDL_RenderCopy(pRes->pRenderer, lapTex, NULL, &lapRect);
                SDL_DestroyTexture(lapTex);
            }
        }
        // Inställningar: ljud, kontroller, färgtema
        else if (mode == OPTIONS)
        {
            if (menuMode == CLASSIC)
            {
                SDL_RenderCopy(pRes->pRenderer, pRes->pOptionsMenuTex, NULL, NULL);
                SDL_RenderCopy(pRes->pRenderer, pRes->pBackToMenuTexture, NULL, &pRes->backRect);

                if (cMode == WASD)
                    SDL_RenderCopy(pRes->pRenderer, pRes->pWASDTexture, NULL, &pRes->WASDRect);
                else
                    SDL_RenderCopy(pRes->pRenderer, pRes->pArrowTexture, NULL, &pRes->arrowRect);

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

                if (cMode == WASD)
                    SDL_RenderCopy(pRes->pRenderer, pRes->pWASDDarkTexture, NULL, &pRes->WASDDarkRect);
                else
                    SDL_RenderCopy(pRes->pRenderer, pRes->pArrowDarkTexture, NULL, &pRes->arrowDarRect);

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
        // IP-anslutning, inmatning av ID
        else if (mode == MULTIPLAYER)
        {

            if (menuMode == CLASSIC)
            {
                // Classic Multiplayer menu
                SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerMenuTex, NULL, NULL);
                SDL_RenderCopy(pRes->pRenderer, pRes->pBackToMultiTexture, NULL, &pRes->backMRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->backMRect);
                SDL_RenderCopy(pRes->pRenderer, pRes->pEnterGameTexture, NULL, &pRes->enterRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->enterRect);
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

                SDL_RenderCopy(pRes->pRenderer, pRes->pMultiplayerMenuDarkTex, NULL, NULL);
                SDL_RenderCopy(pRes->pRenderer, pRes->pBackToMultiDarkTexture, NULL, &pRes->backMDarkRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->backMDarkRect);

                SDL_RenderCopy(pRes->pRenderer, pRes->pEnterGameDarkTexture, NULL, &pRes->enterDarkRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->enterDarkRect);

                SDL_SetRenderDrawColor(pRes->pRenderer, 5, 10, 20, 255);
                SDL_RenderFillRect(pRes->pRenderer, &pRes->joinDarkRect);
                SDL_SetRenderDrawColor(pRes->pRenderer, 0, 0, 0, 180);
                SDL_RenderDrawRect(pRes->pRenderer, &pRes->joinDarkRect);

                SDL_Color white = {255, 255, 255};
                const char *displayIp = strlen(joinIpText) == 0 ? " " : joinIpText;

                SDL_Surface *ipSurf = TTF_RenderText_Solid(pRes->pFont, displayIp, white);
                SDL_Texture *ipTex = SDL_CreateTextureFromSurface(pRes->pRenderer, ipSurf);
                SDL_Rect ipRect = {560, 350, ipSurf->w, ipSurf->h};
                SDL_RenderCopy(pRes->pRenderer, ipTex, NULL, &ipRect);
                SDL_FreeSurface(ipSurf);
                SDL_DestroyTexture(ipTex);

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
        // Presentera rendering
        SDL_RenderPresent(pRes->pRenderer);
        SDL_Delay(16); // cirka 60 FPS
    }
}