@echo off
REM Stänger av kommandotolkens kommandovisning (för snyggare output)

echo Starting server...
start server.exe
REM Öppnar ett nytt fönster med serverprogrammet (måste vara separat körbart)

timeout /t 1 >nul
REM Väntar 1 sekund för att ge servern tid att starta (gömmer "Press any key...")

echo Starting first client (ID 0)...
start Game.exe --ip 127.0.0.1 --id 0
REM Startar första spelarklienten (playerID = 0) i ett nytt fönster

timeout /t 1 >nul
REM Väntar 1 sekund till (för säker anslutning)

echo Starting second client (ID 1)...
start Game.exe --ip 127.0.0.1 --id 1
REM Startar andra spelarklienten (playerID = 1) i ett nytt fönster

echo All programs started. You can now play!
pause
REM Visar "Press any key to continue..." för att hålla batch-fönstret öppet
