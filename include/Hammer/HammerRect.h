/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
 * LICENSE file in the root directory of this source tree.
 */

#ifndef HAMMERRECT_H
#define HAMMERRECT_H

#include <vector>

// 2D Vector structure for geometric calculations
struct HammerVec2 {
    float x;
    float y;

    HammerVec2(float X = 0.0f, float Y = 0.0f) : x(X), y(Y) {}
};

// 3D Vector structure for 3D geometric calculations
struct HammerVec3 {
    float x;
    float y;
    float z;

    HammerVec3(float X = 0.0f, float Y = 0.0f, float Z = 0.0f) : x(X), y(Y), z(Z) {}
};

// ==========================================
// 3D CUBE CLASSES
// ==========================================

class HammerRectCube {
public:
    int x;
    int y;
    int z;

    int w;
    int h;
    int d;

    // Rotation angles in radians (Euler angles: Pitch/X, Yaw/Y, Roll/Z)
    float rotX;
    float rotY;
    float rotZ;

    HammerRectCube(int X = 0, int Y = 0, int Z = 0, int W = 0, int H = 0, int D = 0, 
                   float RotX = 0.0f, float RotY = 0.0f, float RotZ = 0.0f);

    bool HammerRectCollideCube(const HammerRectCube& rect) const;
    bool HammerRectCollideRotatedCube(const HammerRectCube& rect) const;
    int HammerRectCollideFaceCube(const HammerRectCube& box) const;
    
    void GetCorners(HammerVec3 corners[8]) const;
};

class HammerRectCubeF {
public:
    float x;
    float y;
    float z;

    float w;
    float h;
    float d;

    // Rotation angles in radians (Euler angles: Pitch/X, Yaw/Y, Roll/Z)
    float rotX;
    float rotY;
    float rotZ;

    HammerRectCubeF(float X = 0.0f, float Y = 0.0f, float Z = 0.0f, float W = 0.0f, float H = 0.0f, float D = 0.0f, 
                    float RotX = 0.0f, float RotY = 0.0f, float RotZ = 0.0f);

    bool HammerRectCollideCubeF(const HammerRectCubeF& rect) const;
    bool HammerRectCollideRotatedCubeF(const HammerRectCubeF& rect) const;
    int HammerRectCollideFaceCubeF(const HammerRectCubeF& box) const;

    void GetCorners(HammerVec3 corners[8]) const;
};

// ==========================================
// 2D SQUARE / RECTANGLE CLASSES
// ==========================================

class HammerRectSquare {
public:
    int x;
    int y;

    int w;
    int h;

    // Rotation angle in radians around center
    float rotation;

    HammerRectSquare(int X = 0, int Y = 0, int W = 0, int H = 0, float Angle = 0.0f);

    bool HammerRectCollideSquare(const HammerRectSquare& rect) const;
    bool HammerRectCollideRotatedSquare(const HammerRectSquare& rect) const;
    int HammerRectCollideFaceSquare(const HammerRectSquare& rect) const;

    HammerVec2 GetCenter() const;
    void GetCorners(HammerVec2 corners[4]) const;
};

class HammerRectSquareF {
public:
    float x;
    float y;

    float w;
    float h;

    // Rotation angle in radians around center
    float rotation;

    HammerRectSquareF(float X = 0.0f, float Y = 0.0f, float W = 0.0f, float H = 0.0f, float Angle = 0.0f);

    bool HammerRectCollideSquareF(const HammerRectSquareF& rect) const;
    bool HammerRectCollideRotatedSquareF(const HammerRectSquareF& rect) const;
    int HammerRectCollideFaceSquareF(const HammerRectSquareF& rect) const;

    HammerVec2 GetCenter() const;
    void GetCorners(HammerVec2 corners[4]) const;
};

// Dedicated Oriented Bounding Box (OBB) Class
class HammerRectOBBF {
public:
    float cx;       // Center X position
    float cy;       // Center Y position
    float halfW;    // Half width
    float halfH;    // Half height
    float rotation; // Rotation in radians

    HammerRectOBBF(float CX = 0.0f, float CY = 0.0f, float HalfW = 0.0f, float HalfH = 0.0f, float Rotation = 0.0f);
    explicit HammerRectOBBF(const HammerRectSquareF& rect);

    void GetCorners(HammerVec2 corners[4]) const;
    bool HammerRectCollideOBBF(const HammerRectOBBF& other) const;
};

// ==========================================
// CIRCLE & SPHERE CLASSES
// ==========================================

class HammerRectCircle {
public:
    int x;
    int y;
    int r;

    HammerRectCircle(int X = 0, int Y = 0, int R = 0);

    bool HammerRectCollideCircle(const HammerRectCircle& rect) const;
    bool HammerRectCollideSquare(const HammerRectSquare& rect) const;
    bool HammerRectCollideRotatedSquare(const HammerRectSquare& rect) const;
};

class HammerRectCircleF {
public:
    float x;
    float y;
    float r;

    HammerRectCircleF(float X = 0.0f, float Y = 0.0f, float R = 0.0f);

    bool HammerRectCollideCircleF(const HammerRectCircleF& rect) const;
    bool HammerRectCollideSquareF(const HammerRectSquareF& rect) const;
    bool HammerRectCollideRotatedSquareF(const HammerRectSquareF& rect) const;
};

class HammerRectSphere {
public:
    int x;
    int y;
    int z;
    int r;

    HammerRectSphere(int X = 0, int Y = 0, int Z = 0, int R = 0);

    bool HammerRectCollideSphere(const HammerRectSphere& rect) const;
    bool HammerRectCollideCube(const HammerRectCube& cube) const;
    bool HammerRectCollideRotatedCube(const HammerRectCube& cube) const;
};

class HammerRectSphereF {
public:
    float x;
    float y;
    float z;
    float r;

    HammerRectSphereF(float X = 0.0f, float Y = 0.0f, float Z = 0.0f, float R = 0.0f);

    bool HammerRectCollideSphereF(const HammerRectSphereF& rect) const;
    bool HammerRectCollideCubeF(const HammerRectCubeF& cube) const;
    bool HammerRectCollideRotatedCubeF(const HammerRectCubeF& cube) const;
};

#endif // HAMMERRECT_H