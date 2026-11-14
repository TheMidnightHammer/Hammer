This doc will show you how to compile and run the demos that come with Hammer, and how to clean up after them.
It assumes you are on Linux with a medium understanding of the terminal. If you want to learn how to use the engine itself, start with Tutorial2d.md.

Now let's get started.

## Building a single demo

The demos need cmake, GCC, Vulkan (with the loader and headers), glm and glfw3. The engine uses more libs but those come prepackaged in the engine source.
The easiest way to get all of them and to build is to run the `build.sh` from the root of the repository:

`./build.sh`

It detects your distribution (Debian/Ubuntu/Mint/Pop, Fedora/RHEL/CentOS, Arch and openSUSE are supported) and installs everything with your package manager.
After installing, the script asks you for the path of a project to configure and build — you can point it at a demo folder and it will build it for you.
If your distro is not supported, install the equivalent packages by hand: `cmake`, the Vulkan development packages, `glm` and `glfw3`.

## Compiling a single demo manually

Every demo lives in its own folder under `demo/` and is a self-contained cmake project.
To build one, create a build folder inside the demo, run cmake and then make:

Change to the correct directory of the demo which you want to build (in this case it's the 2d world demo, you can do any of them the steps are the same).
```
cd demo/2d_world
```

Create the build directory which will hold all build artifacts.
```
mkdir build
```

Change directory to the directory we just created above.
```
cd build
```

Now we will configure the build using cmake, in this case we will configure with the `Debug` flags, if you want you can change it to `Release`. The `Release`
flag will tell the compiler to optimize this build by removing safety checks and skipping code which will never be run, and a bunch of other things.

Hammer also supports Vulkan validation layers, they give you much more detailed errors from the GPU (vulkan), but they are turned off by default.
To enable them for a demo, add the option to the cmake step:

`-DENABLE_VULKAN_VALIDATION_LAYERS=ON`

This is a compile-time feature: when enabled the Vulkan validation layer code is compiled into the engine, when disabled it is not compiled.
For more details see section 2.1 of Tutorial2d.md.
```
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

Now we build the demo using configuration we made previously. The command `nproc` returns the number of threads your CPU has.
```
make -j$(nproc)
```

The executable is called `voxel_Engine` and ends up in the `build/` folder.
To run it, go back to the demo's root folder and start it from there:

```
cd ..
./build/voxel_Engine
```

It is important to run the demo from the demo's root folder, because the demos load their shaders and textures with relative paths.

## Compiling and running every demo (very simple).

If you don't want to do the steps above by hand for all 9 demos, run this from the root of the repository:

`./build_and_run_demos.sh`

The script goes through every folder in `demo/` and for each one it:
creates the `build/` folder, runs `cmake -DCMAKE_BUILD_TYPE=Debug`, builds with `make -j$(nproc)`, moves the executable to the demo's root folder and runs it.
If any build step fails, the script stops right there instead of continuing with the next demo. Pretty much the same thing as in the *Compiling a single demo manually* section.

Each demo opens a window, so the script waits until you close it before moving on to the next demo.