# GLBufferGenerator
Extract VBO/EBO data for later usage in OpenGL/Vulkan.

## Build
first install assimp system wide:
```bash
// arch
sudo pacman -S assimp

// ubuntu/debian
sudo apt-get install libassimp-dev assimp-utils

// fedora/RHEL
sudo dnf install assimp-devel
```
now just use the Makefile:
```bash
make
```

## Usage
```bash
./generator [model file]
```
