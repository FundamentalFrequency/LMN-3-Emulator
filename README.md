# LMN-3-Emulator
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

![screenshot of emulator](images/emulator.png)

This emulator allows the user to emulate the LMN-3 Hardware and produces MIDI signals that can be used to control the 
[LMN-3-DAW](https://github.com/FundamentalFrequency/LMN-3-DAW).

If you would like to support the project, please consider becoming a [sponsor](https://github.com/sponsors/stonepreston).

## Releases
Releases can be downloaded [here](https://github.com/FundamentalFrequency/LMN-3-Emulator/releases)

## Building From Source

### Cloning the Repository
This repository relies on git submodules. You must clone the submodules as well using:
```bash
git clone --recurse-submodules https://github.com/FundamentalFrequency/LMN-3-Emulator
```

### Dependencies
You will need to run the following command to install the build dependencies:
```bash
sudo apt install apt install libasound2-dev libjack-jackd2-dev \
    ladspa-sdk \
    libcurl4-openssl-dev  \
    libfreetype6-dev \
    libx11-dev libxcomposite-dev libxcursor-dev libxcursor-dev libxext-dev libxinerama-dev libxrandr-dev libxrender-dev \
    libwebkit2gtk-4.0-dev \
    libglu1-mesa-dev mesa-common-dev \ 
    build-essential cmake clang llvm lld
```

### Building the Project
CMake is used to build the project. Execute the following commands to build all project targets:
```bash
cmake -B build -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_FLAGS=-fuse-ld=lld -DCMAKE_BUILD_TYPE=Release
cmake --build build -j8
```

## Contributing
Pull requests are welcome and very much appreciated.

### Code Style
Please format your code before submitting a PR. It will fail the CI linting check otherwise. You can run the following
commands to format it according to the provided .clang-format file:
```bash
sudo apt install clang-format # install clang-format if you don't already have it
find Source/ -iname *.h -o -iname *.cpp | xargs clang-format -i --style=file
```