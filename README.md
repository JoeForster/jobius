# jobius

This is my rudimentary game engine project for playing around with ECS, currently with a fairly simple and naïve implementation.

Next steps at the ECS level are to add ways to query components and schedule updates in a proper way.

Next steps at the game level are to actually make a game that's not just some simple objects falling with a hard-coded update; cwurrently working on a simple behaviour tree implementation to tie to controlling a simple NPC object.

## Building

(untested)

1. Install CMake at least version 3.8
2. Run CMake on the folder and configure paths to SDL libraries
3. Generate project files and build

## Running Package

Run from within Visual Studio, or if using the package just run JobiusShell.exe

## Controls

- WSAD moves the camera, Q and E zooms.
- Return resets the camera.
- Arrow keys to move the block dropper, SPACE to place a block.