@echo off
setlocal

set "SCRIPT_PATH=%~dp0CreateExeFile.ps1"

if not exist "%SCRIPT_PATH%" (
    echo [ERROR] CreateExeFile.ps1 was not found.
    echo Path: "%SCRIPT_PATH%"
    pause
    exit /b 1
)

echo === Creating executable package... ===
powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File "%SCRIPT_PATH%"
set "SCRIPT_EXIT_CODE=%ERRORLEVEL%"

if not "%SCRIPT_EXIT_CODE%"=="0" (
    echo.
    echo [ERROR] Package creation failed. Exit code: %SCRIPT_EXIT_CODE%
    pause
    exit /b %SCRIPT_EXIT_CODE%
)

echo.
echo === Package creation completed successfully. ===
pause
exit /b 0
