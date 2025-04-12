@echo off
echo Startar test & debug...
game.exe --test --debug

IF %ERRORLEVEL% EQU 0 (
    echo  TEST PASSED (kod 0)
) ELSE (
    echo TEST FAILED (kod %ERRORLEVEL%)
)

echo.
echo Loggfilens:
type testlog.txt
pause
