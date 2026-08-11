#HammerRect
This short doc will give you examples of how to do collisons with Hammer

##Example 1: Checking 2D Rotated Rectangle Collision

```
#include <iostream>
#include "HammerRect.h"

int main() {
    // 45 degrees in radians
    float angle45 = 45.0f * (3.14159265f / 180.0f);

    // Create Rect A at (0,0) with 100x100 dimensions rotated 45 degrees
    HammerRectSquareF rectA(0.0f, 0.0f, 100.0f, 100.0f, angle45);

    // Create Rect B at (50,50) unrotated
    HammerRectSquareF rectB(50.0f, 50.0f, 100.0f, 100.0f, 0.0f);

    // Perform OBB rotated collision check
    if (rectA.HammerRectCollideRotatedSquareF(rectB)) {
        std::cout << "Rotated Rectangles Collided!\n";
    }

    return 0;
}
```

##Example 2: Circle vs Rotated Rectangle

```
#include <iostream>
#include "HammerRect.h"

int main() {
    float angle30 = 30.0f * (3.14159265f / 180.0f);

    // Rectangle rotated by 30 degrees
    HammerRectSquareF rect(100.0f, 100.0f, 200.0f, 100.0f, angle30);

    // Circle centered nearby
    HammerRectCircleF circle(150.0f, 120.0f, 30.0f);

    if (circle.HammerRectCollideRotatedSquareF(rect)) {
        std::cout << "Circle hit the rotated rectangle!\n";
    }

    return 0;
}
```

##Example 3: 3D Rotated Cube Collision

```
#include <iostream>
#include "HammerRect.h"

int main() {
    float pitch = 0.2f, yaw = 0.5f, roll = 0.0f;

    HammerRectCubeF cubeA(0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 10.0f, pitch, yaw, roll);
    HammerRectCubeF cubeB(5.0f, 5.0f, 5.0f, 10.0f, 10.0f, 10.0f, 0.0f, 0.0f, 0.0f);

    if (cubeA.HammerRectCollideRotatedCubeF(cubeB)) {
        std::cout << "3D Rotated Cubes Collided!\n";
    }

    return 0;
}
```