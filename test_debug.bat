@echo off
REM Stäng av kommandovisningen för att göra output renare

echo Startar test & debug...
REM Kör spelet med test- och debug-flaggor
game.exe --test --debug

REM Kontrollera om programmet avslutades utan fel
IF %ERRORLEVEL% EQU 0 (
    echo  TEST PASSED (kod 0)
) ELSE (
    echo TEST FAILED (kod %ERRORLEVEL%)
)

echo.
echo Loggfilens:
REM Skriv ut innehållet i testloggfilen
type testlog.txt

REM Pausa så att användaren hinner läsa resultatet innan fönstret stängs
pause
