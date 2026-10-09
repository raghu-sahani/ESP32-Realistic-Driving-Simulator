@echo off
cd /d "%~dp0"
where py >nul 2>nul
if %errorlevel% equ 0 (
  py -3 run_game.py
  pause
  exit /b
)
where python >nul 2>nul
if %errorlevel% equ 0 (
  python run_game.py
  pause
  exit /b
)
echo Python 3 was not found. Install Python 3, or open index.html using VS Code Live Server.
pause
