# 3DS Dodger Game

This is a 3DS Homebrew application that is a simple 2D game where the user dodges projectiles from the top of the screen, gaining points and increase the difficulty - similar to Space Invaders.

# Features

-- Player and Enemies
-- Movement Controls 
-- Lives and Overlap Mechanics
-- Score System
-- Simple UI creation for score | lives | restarting

# Screenshots
### Game Running
![Game Running] (Assets/Game-Running.png)

# Tech Stack
Backend - C++
Library/Framework - DevKitPro
Version Control - Git, Github

## Getting Started
### Prerequisites
- devkitpro from devkitpro.org
- latest version of C++ on VSCode or dedicated compiler
- emulator that runs 3DSX files such as Azahar or 3DS

### Installation
**1. Clone the repository

**2. Install Dependencies
```bash
sudo dkp-pacman -S 3ds-dev 3ds-examples
```
- Check that compiler is installed

```bash
arm-none-eabi-g++ --version
```
**3. Copy project template into new folder
```bash
cp -r $DEVKITPRO/examples/3ds/templates/application dodger
cd dodger
ls
```

**4. Make a new folder called .vscode in the directory and add this.
```bash
{
  "configurations": [
    {
      "name": "Nintendo 3DS",
      "includePath": [
        "${workspaceFolder}/**",
        "/opt/devkitpro/libctru/include",
        "/opt/devkitpro/devkitARM/arm-none-eabi/include"
      ],
      "compilerPath": "/opt/devkitpro/devkitARM/bin/arm-none-eabi-g++",
      "cStandard": "c11",
      "cppStandard": "c++17",
      "intelliSenseMode": "gcc-arm"
    }
  ],
  "version": 4
}
```

**5. Once this is set up, it's ready.
```bash
make clean
make
```
**6. Run the 3DSX file on emulator.

## Project Structure
```text
dodger/
├── .vscode/
├── build/
├── source/
    └── main.cpp
└── dodger.3dsx
└── dodger.elf
└── Makefle
└── README.md
```

## Future Improvement
- [ ] Replace the characters with better looking sprites
- [ ] Make a home screen UI, settings UI, etc
- [ ] Add some music

