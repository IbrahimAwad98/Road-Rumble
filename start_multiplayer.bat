@echo off
REM Stänger av att kommandon visas i terminalen (renare output)

echo Starting server...
REM Startar servern i ett nytt fönster och visar "SERVER LOG" först
start cmd /k "echo SERVER LOG && server.exe"

REM Väntar 2 sekunder för att ge servern tid att initieras
timeout /t 2 >nul

echo Starting first client (ID 0)...
REM Startar första klienten i nytt terminalfönster, med "CLIENT 0" överst
start cmd /k "echo CLIENT 0 && Game.exe --ip 127.0.0.1 --id 0"

REM Väntar 1 sekund till så att första klienten hinner koppla upp sig
timeout /t 1 >nul

echo Starting second client (ID 1)...
REM Startar andra klienten i nytt terminalfönster, med "CLIENT 1" överst
start cmd /k "echo CLIENT 1 && Game.exe --ip 127.0.0.1 --id 1"

echo All programs started. You can now play!
REM Väntar på att användaren trycker på en tangent innan fönstret stängs
pause
