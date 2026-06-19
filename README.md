# Hammer
Hammer is an open source engine using Vulkan as backend.
It supports simple collision detection, model loading, there is an example of all features in the demo folder.

[Showoff.webm](https://github.com/user-attachments/assets/d3d87ad2-bde6-4431-944c-7280afff7d88)

# The goal of Hammer Engine
The goal of the Hammer Engine is combining ease of use and the speed of Vulkan to make 2d/3d games as fast and fun as possible. With single-board computers like the Raspberry Pi 5, even old games like Doom 3 (dhewm3) will struggle, this is why i made a vulkan engine with a very optimized rendering pipeline.

# The performance of Hammer

The #1 goal of Hammer is performance on low-end machines. It out-performs many other engines while remaining compatible with many machines.

# Cross-platform

Hammer supports for all major linux distro, windows (In testing) and chromebook via a emulation layer.

# Upcoming features
1. Directional lights
2. More physics
3. Text rendering (in work now)
4. Compute shaders
5. Multithreading support (recording command buffers on different cores, Help needed)

# What does Hammer support?

Native Vulkan support: with a very optimized rendering pipeline
SSBO (shader storage buffer object): enables you to send custom data to the GPU
Simple collision detection: simple but useful
Unlimited textures: many old engine had a hard limmit of how many textures you could you, Hammer does not have any
Model loading (.obj only): makes it easier to load complex models

<img width="632" height="706" alt="image" src="https://github.com/user-attachments/assets/157a91fb-d69d-4deb-9de6-efdec45e3402" />
<img width="1228" height="966" alt="image" src="https://github.com/user-attachments/assets/7d6a529a-c3ba-4a30-881c-aeba3b27118b" />


# Docs

The official tutorial is at docs/

# Run demos

## Linux

1. Run the build.sh
`./build.sh`

2. The build will ask which demo you want to run, you can put the path to any demo in the demo folder.

3. Run the demo and give me a star.

## Windows & macOS

We are currently looking for contributors to help establish build instructions for macOS. If you have successfully compiled Hammer on this platform, please consider submitting a Pull Request or opening an issue with your steps!

For windows, please check if the current build instructions work.

# Want to contribute?
We are looking for contributors! If you want to contribute to Hammer, go to the issues tab, pick an issue and create a fix for it. Or if you find a bug, please report it in the issue tab. If you have an idea for a new feature, you can create an issue with the label New feature or implement it and create a pull request. We are especially looking for help with:

1. Optimizing the Engine even more for low-end machines.
2. Extended physics/collision features.
5. Add documentation, to help people learn Hammer.
6. Add more demos to the demo/ folder.
7. Implement multi-command buffer recording on different cores

# How to use?

The Hammer Engine tutorial is being made right now and should be finished very soon. You can find it at https://github.com/MidnightHammer-code/Hammer/tree/main/docs.
