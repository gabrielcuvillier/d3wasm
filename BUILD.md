
# Building on Ubuntu 18.04

#### 1) Install Emscripten
a) Get the Emscripten SDK and install Emscripten "upstream" variant:
```
git clone https://github.com/emscripten-core/emsdk.git
./emsdk/emsdk install latest-upstream
./emsdk/emsdk activate latest-upstream
 ``` 
b) Enable Emscripten tools from the command line
```
source ./emsdk/emsdk_env.sh
```

#### 2) Build d3wasm
a) Get d3wasm source code 
```
git clone https://github.com/gabrielcuvillier/d3wasm.git
cd d3wasm
```
b) Build the project
```
mkdir build-wasm
cd build-wasm
emcmake cmake ./neo -DCMAKE_BUILD_TYPE=Release
emmake make
```
Normaly, this should have generated *d3wasm.html*, *d3wasm.js*, and *d3wasm.wasm* files.

#### 4) Package the game demo data
a) Get the Doom 3 Demo from Fileplanet (or other sources): https://www.fileplanet.com/archive/p-15998/DOOM-3-Demo

b) Install the Demo (using wine on Linux)

c) Copy __<D3Demo_install_path>/demo/demo00.pk4__ to __<D3wasm_path>/build-wasm/data/demo__ folder

d) Package the data using the *package_demo_data* cmake target
```
make package_demo_data
```
Normally, this should have generated *demo00.data* and *demo00.js* files

#### 5) Deploy locally
a) Run a local web server from the build directory. The simplest is to use python SimpleHTTPServer:
```
python -m SimpleHTTPServer 8080
```
or (python 3)
```
python -m http.server 8080
```
b) open your favorite Browser to http://localhost:8080/d3wasm.html

#### 6) Enjoy!

**NB**: it is possible to do a native build also, to compare the performance for example. 
To do so:
```
  mkdir build-native
  cd build-native
  cmake ../neo -DCMAKE_BUILD_TYPE=Release
  make
```
Packaging the data works differently for native builds. Have a look at https://github.com/dhewm/dhewm3/wiki/FAQ

# Building on Windows 10

While it is possible to build using the Windows version of Emscripten toolchain, it is a bit more tedious to do (require to manually install Windows version of Cmake, GNU Make or Ninja, and setup environment paths).

A simpler way is to use **WSL** (*Windows Subsystem for Linux*) with Ubuntu 18.04 installed: the instructions are then exactly the same as the Ubuntu build. Nice!
