# jobius

https://github.com/JoeForster/jobius

This is my rudimentary game engine project for playing around with ECS, currently with a fairly simple and naïve implementation in serious need of optimisation and work.

Next steps at the ECS level are to add ways to query components and schedule updates in a proper way, optimise, and demonstrate all this with some little games. Also working on a Behaviour Tree implementation with OOP and looking into how that can be re-worked into ECS.

## Building

(untested on any machine not mine!)

1. Install CMake at least version 3.8
2. Run CMake on the folder and configure paths to SDL libraries (provided in lib folder, but you may need to fix up the paths)
4. Generate project files and build
5. If in VS, set the start-up project to `JobiusShell`

## Games

To switch games, you will need to change the `JOBIUS_SHELL_GAME` preprocessor variable in `WinMain.cpp`.
Current main work is the BlockoLife cellular automata demo, which simulates a simple ecosystem of plants, herbivores and carnivores.

There's also the ChaosSpace demo game, which is just some space ships that don't do much yet.

### Running Package

Package has the .exe copied to the root folder from build/src/Engine for demonstration.
Run from within Visual Studio, or if using the package just run JobiusShell.exe

### Controls

- WSAD moves the camera, Q and E zooms.
- Return resets the camera.
- Arrow keys to move the block dropper, SPACE to place a block.