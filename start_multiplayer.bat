@echo off
REM Stänger av att kommandon visas i terminalen (renare output)

echo Starting server...
REM Startar servern i ett nytt fönster och visar "SERVER LOG" först
start cmd /k "echo SERVER LOG && server.exe"

REM Väntar 2 sekunder för att ge servern tid att initieras
timeout /t 2 >nul

echo Starting client 1...
REM Startar klient 1 i nytt terminalfönster, med "CLIENT 1" överst
start cmd /k "echo CLIENT 1 && Game.exe --ip 127.0.0.1 --id 1"

REM Väntar 1 sekund så att klient 1 hinner koppla upp sig
timeout /t 1 >nul

echo Starting client 2...
REM Startar klient 2 i nytt terminalfönster, med "CLIENT 2" överst
start cmd /k "echo CLIENT 2 && Game.exe --ip 127.0.0.1 --id 2"

REM Väntar 1 sekund så att klient 2 hinner koppla upp sig
timeout /t 1 >nul

echo Starting client 3...
REM Startar klient 3 i nytt terminalfönster, med "CLIENT 3" överst
start cmd /k "echo CLIENT 3 && Game.exe --ip 127.0.0.1 --id 3"

REM Väntar 1 sekund så att klient 3 hinner koppla upp sig
timeout /t 1 >nul

echo Starting client 4...
REM Startar klient 4 i nytt terminalfönster, med "CLIENT 4" överst
start cmd /k "echo CLIENT 4 && Game.exe --ip 127.0.0.1 --id 4"

echo All programs started. You can now play!
REM Väntar på att användaren trycker på en tangent innan fönstret stängs
pause
