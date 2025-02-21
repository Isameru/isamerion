
# *Isamerion* :: A collection of loosely-coupled C++ files

This toy project is intended to be featured on my personal website.
Nothing particularly interesting at the moment.

## Building from Sources

_CMake_ is required to build the project in any configuration.

### Building on Windows using Visual Studio

Install CMake support addon for Visual Studio and open the project directory as CMake project.
The directory `deps-win` contains the necessary dependencies, including the binaries (_.dll_ files).
After building the project with Visual Studio, you have to copy all _.dll_ files to the location of the executable file.

### Building on Linux or WSL

```
sudo apt update
sudo apt install \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-ttf-dev \
    libglew-dev

mkdir _build-linux && cd _build-linux
cmake ..
make -j
```

### Building WebAssembly on Linux

```
. ~/emsdk/emsdk_env.sh
mkdir _build-wasm && cd _build-wasm
emcmake cmake ..
make -j
```
