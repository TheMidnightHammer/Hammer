Welcome to the Hammer documentation. This tutorial will walk you through the evolution of a Hammer project, starting with basic 2D rendering, moving meshes and collision using HammerRect.
This tutorial assumes that you have a medium understanding of C++.

Now let's get started.

## 1. Creating a Hammer Engine instance.

The foundation of any Hammer project involves configuring the engine settings, initializing Vulkan, and setting up a basic camera.

First we need to an instance of Hammer:

`HammerEngine engine;`

Now we have an instance of the engine called “engine”.

## 2. Configuring the Engine.
### 2.1 Validation layers
Validation layers are there to give you more detailed errors than the one Vulkan gives you. For development we will turn them on.

`Engine.enableValidationLayers = true;`

You can always turn them off by just changing the true for false.

### 2.2 Window
In Hammer Engine you can change a few things of the window. These things include: window size (w and h) but also the title of the screen.

```
Engine.WindowWidth = 1000;
Engine.WindowHeight = 1000;
Engine.windowName = "Hammer Engine";
```

### 2.3 Camera position
You can set the starting position of the camera.

`Engine.cameraPosition = glm::vec3(0, 0, 2);`

### 2.4 The texture atlas path
You can give Hammer Engine the path to a texture, but only 1 in the future Hammer Engine will have a way to have many textures but for now only 1.

`Engine.texturePath = "textures/texture.png";`

### 2.5 The mouse
If you are building an app that you need to have the control over the mouse cursor you can call change the cursor state.
If mouse lock it set to 0 the mouse will be free to move, if it 1 the mouse can no longer move.

`Engine.mouseLock = 0;`

### 2.6 Max textures
Hammer needs to know how many textures in total there will be, I recommend putting 1.5 times as much as you will need. If you get any weird errors from the validation layers try increasing the number.
For this example we will put 1000 to avoid future head aks.

`Engine.MaxTextures = 1000;`

### 2.7 Camera speed

If you are using the build in cameras from Hammer the cameraSpeed variable will change the speed at which the camera move in all axis.

`Engine.cameraSpeed = 0.1f;`

### 2.8 Render distance

Hammer has a variable that changes the render distance of the app.

`Engine.renderDistance = 16.0f;`

### 2.9 Sensitivity

When you use the build in cameras from Hammer you can change the sensitivity of the camera.

`Engine.renderDistance = 16.0f;`

### 3. Initialization

Before we can load textures or create shapes, we have to wake the engine up. This involves two critical calls:

```
Engine.initWindow();
Engine.initVulkan();
```

    Note: You must call these function afther configuring the Hammer instance.

### 4. Pipelines

In graphics programming graphics pipelines need to be configured to use effects or for to put anything on the screen. HammerPipeline takes a few parameters: the Hammer instance, vertex shader path, fragment shader path, render triangle mod, render both sides of the triangle.

The shaders MUST be precompiled using a SPIR-V compiler, the triangle mod is when you want to render the triangle normaly or only the wireframe (each 3 lines of a triangle), the render 2 side triangle mod determines if you want to see both sides of trianges most games only render 1 side.

```
std::string vPath = "shaders/vert.spv";
std::string fPath = "shaders/frag.spv";
HammerPipline* mainPipeline = HammerPipeline(
    Engine, vPath, fPath, 1, true
);
```

### 4.1 Loading a Texture

To renderer anything we need to add a texture to the mesh. The mesh will be created later.
We use pointers to manage the memory of our texture. We also specify a filter (like Nearest for crisp pixel art).

`HammerTexture* mainTexture = HammerTexture(Engine, "textures/texture.png", HammerTextureFilter::Nearest);`

### 5. Creating a Mesh

A Mesh is the actual object in your world. It requires three things: Vertices (points in space), Indices (the order to connect those points), and the Texture/Pipeline we created above.

The following code creates a quade. A quade has 4 points so there will be 4 vertex. Indices are the instruction on how to connecte them. If you are rendering both side of the triangles (See the pipeline section) the order does not really matter. (reverse them will change what side is renderer if you are not using the render 2 side triangle mod)
```
std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    {{ 0.5f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
    {{ 0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
    {{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}
};
std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };
```

### 5.5 Adding giving the mesh to Hammer

uh no, there is a function to do this i.g some where

`Engine.meshs.push_back(std::make_unique<HammerMesh>(Engine, mainPipeline.get(), mainTexture.get(), vertices, indices));`

### 6. The Main Loop

Finally, we create the "heartbeat" of your application. This loop runs every frame until you close the window.

```
Engine.drawPassStart(); // Prepare the GPU for drawing

while (!glfwWindowShouldClose(Engine.window)) {
    Engine.updateFrameTimeStart(); // Start the frame timer

    // --- Input & Logic ---
    if (glfwGetKey(Engine.window, GLFW_KEY_W) == GLFW_PRESS) {
        // Handle movement or logic here!
    }

    Engine.updateCameraDefault2D(); // Update the camera view
    Engine.drawFrame();             // Render everything in Engine.meshs
    
    Engine.updateFrameTimeEnd();   // End the frame timer
}

Engine.drawPassEnd(); // Clean up GPU tasks
```

### 7. Memory Cleanup

Because we are working with C++ and Vulkan, we need to be responsible. Before the program ends, ensure you reset your unique pointers to destroy the Vulkan resources in the correct order.
```
mainTexture.reset(); 
mainPipeline.reset();
```

Congratulations!

You've just built a functional 2D rendering loop in HammerEngine. Your engine is now initialized, your textures are loaded, and your mesh is ready for action.
