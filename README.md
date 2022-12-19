# jobius

https://github.com/JoeForster/jobius

This is my rudimentary game engine project for playing around with ECS, currently with a fairly simple and naïve implementation in serious need of optimisation and work.

Next steps at the ECS level are to add ways to query components and schedule updates in a proper way, optimise, and demonstrate all this with some little games. Also working on a Behaviour Tree implementation with OOP and looking into how that can be re-worked into ECS.

## Building

1. Install CMake at least version 3.8
2. Run CMake on the folder
3. Configure in SDL, filling in the missing SDL folders and selecting the desired game (examples below).
4. Generate project files and build
6. With a fresh CMake build folder or config, you'll need to copy the `SDL2.dll`, `SDL2_image.dll` and `SDL2_ttf.dll` files from their respective lib (probably `lib/x64`) folders to your build output folder, e.g. `build\src\Engine\Release`
5. If in VS, set the start-up project to `JobiusShell`

### CMake Configuration

When you Configure in CMake, it may complain about missing folders. Each time it does, make sure to point it to the SDL paths at the bundled lib folder. You can also point it at an installed libraries if desired. Examples:

- `SDL2_DIR` as `C:/Dev/jobius/lib/SDL2-2.26.1/cmake`
- `SDL2_image_DIR` as `C:/Dev/jobius/lib/SDL2_image-2.6.2/cmake`
- `SDL2_ttf_DIR` as `C:/Dev/jobius/lib/SDL2_ttf-2.20.1/cmake`
- One of `JOBIUS_SHELL_GAME_<GAME>` should be ticked, by default `BLOCKO_LIFE`.

## Games

To switch games, you will need to change the `JOBIUS_SHELL_GAME` preprocessor variable in `WinMain.cpp`. This can also be set via CMake, see above.
Current main work is the BlockoLife cellular automata demo, which simulates a simple ecosystem of plants, herbivores and carnivores.

There's also the ChaosSpace demo game, which is just some space ships that don't do much yet.

### Running Package

Package has the .exe copied to the root folder from build/src/Engine for demonstration.
Run from within Visual Studio, or if using the package just run JobiusShell.exe

### Controls

- WSAD moves the camera, Q and E zooms.
- Return resets the camera.
- Arrow keys to move the block dropper, SPACE to place a block.