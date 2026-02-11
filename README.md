# 3D Graphics Programming

This project is an attempt to organize bits of information
about programming 3D graphics and present it to the general audience.
The materials are used during offline seminars and workshops.

## Complying examples

Configure project into `build` folder using default generator and compile using default compiler.

```
cd path/to/source
cmake -S . -B build 
cmake --build build
```

Use custom generator and compiler for the project.

```
cmake -S path/to/source -B path/to/build -G Ninja 
    -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build path/to/build
```

## Running examples

Executable should be started from the root folder of the repository. It looks for `assets` folder in runtime.

```
cd path/to/source
build\opengl-demo\opengl-demo.exe
```