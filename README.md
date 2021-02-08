# CTHead Viewer

## Setup instructions

### Dependencies

* SFML
* Cmake

### Setup Instructions

1. Clone imgui: https://github.com/ocornut/imgui
2. Clone sfml-imgui bindings: https://github.com/eliasdaler/imgui-sfml

```bash
cd imgui-sfml
mkdir build && cd build
cmake .. -DIMGUI_DIR=../../imgui -DBUILD_SHARED_LIBS=ON
sudo cmake --build . --target install
```

> **NOTE:** If you build without `-DBUILD_SHARED_LIBS=ON`, on Fedora at least, you'll get weird errros being unable to find SFML. This is because it's trying to build a static library, but SFML doesn't have static libraries packaged for Fedora.

### Build and Run

```bash
cd cthead-viewer
mkdir build && cd build
cmake ..
make
./CTHeadViewer <PATH TO CTHEAD FILE>
```