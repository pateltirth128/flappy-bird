@echo off
rem Builds FlappyBird.exe using the free raylib installer for Windows
rem (it installs raylib and a C++ compiler into C:\raylib).

set RAYLIB=C:\raylib
set CXX=%RAYLIB%\w64devkit\bin\g++.exe

if not exist "%CXX%" (
    echo Could not find %CXX%
    echo Install raylib for Windows first: https://raysan5.itch.io/raylib
    pause
    exit /b 1
)

"%CXX%" src\main.cpp src\Game.cpp src\Bird.cpp src\Pipe.cpp -o FlappyBird.exe ^
    -std=c++17 -O2 -Wall ^
    -I"%RAYLIB%\raylib\src" -L"%RAYLIB%\raylib\src" ^
    -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows

if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

echo Built FlappyBird.exe - starting the game...
start FlappyBird.exe
