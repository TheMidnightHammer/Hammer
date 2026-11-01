/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
 * LICENSE file in the root directory of this source tree.
 */

#include "../include/HammerEngine/HammerRect.h"
#include <cmath>
#include <algorithm>
#include <limits>

// ==========================================
// HELPER MATH FUNCTIONS
// ==========================================

static inline float Dot(const HammerVec2& a, const HammerVec2& b) {
    return a.x * b.x + a.y * b.y;
}

static inline float Dot3(const HammerVec3& a, const HammerVec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline HammerVec3 Sub3(const HammerVec3& a, const HammerVec3& b) {
    return HammerVec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline HammerVec3 Cross3(const HammerVec3& a, const HammerVec3& b) {
    return HammerVec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

static inline float Clamp(float val, float minVal, float maxVal) {
    return std::max(minVal, std::min(val, maxVal));
}

// 2D Separating Axis Theorem (SAT) logic
static bool TestSAT2D(const HammerVec2 cornersA[4], const HammerVec2 cornersB[4]) {
    HammerVec2 axes[4] = {
        HammerVec2(cornersA[1].x - cornersA[0].x, cornersA[1].y - cornersA[0].y),
        HammerVec2(cornersA[3].x - cornersA[0].x, cornersA[3].y - cornersA[0].y),
        HammerVec2(cornersB[1].x - cornersB[0].x, cornersB[1].y - cornersB[0].y),
        HammerVec2(cornersB[3].x - cornersB[0].x, cornersB[3].y - cornersB[0].y)
    };

    for (int i = 0; i < 4; ++i) {
        const HammerVec2& axis = axes[i];
        
        float lenSq = Dot(axis, axis);
        if (lenSq < 1e-8f) continue;

        float minA = std::numeric_limits<float>::max();
        float maxA = -std::numeric_limits<float>::max();
        float minB = std::numeric_limits<float>::max();
        float maxB = -std::numeric_limits<float>::max();

        for (int j = 0; j < 4; ++j) {
            float projA = Dot(cornersA[j], axis);
            minA = std::min(minA, projA);
            maxA = std::max(maxA, projA);

            float projB = Dot(cornersB[j], axis);
            minB = std::min(minB, projB);
            maxB = std::max(maxB, projB);
        }

        if (maxA < minB || maxB < minA) {
            return false; // Found a gap
        }
    }

    return true; // Overlap detected on all axes
}

// ==========================================
// Rect cube noF
// ==========================================

HammerRectCube::HammerRectCube(int X, int Y, int Z, int W, int H, int D, float RotX, float RotY, float RotZ)
    : x(X), y(Y), z(Z), w(W), h(H), d(D), rotX(RotX), rotY(RotY), rotZ(RotZ) {}

int HammerRectCube::HammerRectCollideFaceCube(const HammerRectCube& box) const {
    if (this->x < box.x + box.w &&
        this->x + this->w > box.x &&
        this->y < box.y + box.h &&
        this->y + this->h > box.y &&
        this->z < box.z + box.d &&
        this->z + this->d > box.z) 
    {
        int overlapLeft   = (box.x + box.w) - this->x;
        int overlapRight  = (this->x + this->w) - box.x;
        int overlapBottom = (box.y + box.h) - this->y;
        int overlapTop    = (this->y + this->h) - box.y;
        int overlapFront  = (box.z + box.d) - this->z;
        int overlapBack   = (this->z + this->d) - box.z;

        int minOverlap = overlapLeft;
        int face = 1;

        if (overlapTop < minOverlap) { minOverlap = overlapTop; face = 2; }
        if (overlapRight < minOverlap) { minOverlap = overlapRight; face = 3; }
        if (overlapBottom < minOverlap) { minOverlap = overlapBottom; face = 4; }
        if (overlapFront < minOverlap) { minOverlap = overlapFront; face = 5; }
        if (overlapBack < minOverlap) { minOverlap = overlapBack; face = 6; }

        return face;
    }
    return 0;
}

bool HammerRectCube::HammerRectCollideCube(const HammerRectCube& rect) const {
    return (
        this->x < rect.x + rect.w && this->x + this->w > rect.x &&
        this->y < rect.y + rect.h && this->y + this->h > rect.y &&
        this->z < rect.z + rect.d && this->z + this->d > rect.z
    );
}

void HammerRectCube::GetCorners(HammerVec3 corners[8]) const {
    HammerRectCubeF cubeF((float)x, (float)y, (float)z, (float)w, (float)h, (float)d, rotX, rotY, rotZ);
    cubeF.GetCorners(corners);
}

bool HammerRectCube::HammerRectCollideRotatedCube(const HammerRectCube& rect) const {
    HammerRectCubeF a((float)x, (float)y, (float)z, (float)w, (float)h, (float)d, rotX, rotY, rotZ);
    HammerRectCubeF b((float)rect.x, (float)rect.y, (float)rect.z, (float)rect.w, (float)rect.h, (float)rect.d, rect.rotX, rect.rotY, rect.rotZ);
    return a.HammerRectCollideRotatedCubeF(b);
}

// ==========================================
// Rect cube F
// ==========================================

HammerRectCubeF::HammerRectCubeF(float X, float Y, float Z, float W, float H, float D, float RotX, float RotY, float RotZ)
    : x(X), y(Y), z(Z), w(W), h(H), d(D), rotX(RotX), rotY(RotY), rotZ(RotZ) {}

bool HammerRectCubeF::HammerRectCollideCubeF(const HammerRectCubeF& rect) const {
    return (
        this->x < rect.x + rect.w && this->x + this->w > rect.x &&
        this->y < rect.y + rect.h && this->y + this->h > rect.y &&
        this->z < rect.z + rect.d && this->z + this->d > rect.z
    );
}

int HammerRectCubeF::HammerRectCollideFaceCubeF(const HammerRectCubeF& box) const {
    if (this->x < box.x + box.w &&
        this->x + this->w > box.x &&
        this->y < box.y + box.h &&
        this->y + this->h > box.y &&
        this->z < box.z + box.d &&
        this->z + this->d > box.z) 
    {
        float overlapLeft   = (box.x + box.w) - this->x;
        float overlapRight  = (this->x + this->w) - box.x;
        float overlapBottom = (box.y + box.h) - this->y;
        float overlapTop    = (this->y + this->h) - box.y;
        float overlapFront  = (box.z + box.d) - this->z;
        float overlapBack   = (this->z + this->d) - box.z;

        float minOverlap = overlapLeft;
        int face = 1;

        if (overlapTop < minOverlap) { minOverlap = overlapTop; face = 2; }
        if (overlapRight < minOverlap) { minOverlap = overlapRight; face = 3; }
        if (overlapBottom < minOverlap) { minOverlap = overlapBottom; face = 4; }
        if (overlapFront < minOverlap) { minOverlap = overlapFront; face = 5; }
        if (overlapBack < minOverlap) { minOverlap = overlapBack; face = 6; }

        return face;
    }
    return 0;
}

void HammerRectCubeF::GetCorners(HammerVec3 corners[8]) const {
    float cx = x + w / 2.0f;
    float cy = y + h / 2.0f;
    float cz = z + d / 2.0f;

    float hw = w / 2.0f;
    float hh = h / 2.0f;
    float hd = d / 2.0f;

    HammerVec3 local[8] = {
        {-hw, -hh, -hd}, { hw, -hh, -hd}, { hw,  hh, -hd}, {-hw,  hh, -hd},
        {-hw, -hh,  hd}, { hw, -hh,  hd}, { hw,  hh,  hd}, {-hw,  hh,  hd}
    };

    float cx_r = std::cos(rotX), sx_r = std::sin(rotX);
    float cy_r = std::cos(rotY), sy_r = std::sin(rotY);
    float cz_r = std::cos(rotZ), sz_r = std::sin(rotZ);

    for (int i = 0; i < 8; ++i) {
        float y1 = local[i].y * cx_r - local[i].z * sx_r;
        float z1 = local[i].y * sx_r + local[i].z * cx_r;
        float x1 = local[i].x;

        float x2 =  x1 * cy_r + z1 * sy_r;
        float z2 = -x1 * sy_r + z1 * cy_r;
        float y2 =  y1;

        float x3 = x2 * cz_r - y2 * sz_r;
        float y3 = x2 * sz_r + y2 * cz_r;
        float z3 = z2;

        corners[i] = HammerVec3(cx + x3, cy + y3, cz + z3);
    }
}

bool HammerRectCubeF::HammerRectCollideRotatedCubeF(const HammerRectCubeF& rect) const {
    if (rotX == 0.0f && rotY == 0.0f && rotZ == 0.0f &&
        rect.rotX == 0.0f && rect.rotY == 0.0f && rect.rotZ == 0.0f) {
        return HammerRectCollideCubeF(rect);
    }

    HammerVec3 cornersA[8];
    HammerVec3 cornersB[8];
    GetCorners(cornersA);
    rect.GetCorners(cornersB);

    std::vector<HammerVec3> axes;
    
    // Face normals
    axes.push_back(Sub3(cornersA[1], cornersA[0]));
    axes.push_back(Sub3(cornersA[3], cornersA[0]));
    axes.push_back(Sub3(cornersA[4], cornersA[0]));

    axes.push_back(Sub3(cornersB[1], cornersB[0]));
    axes.push_back(Sub3(cornersB[3], cornersB[0]));
    axes.push_back(Sub3(cornersB[4], cornersB[0]));

    // Cross products
    for (int i = 0; i < 3; ++i) {
        for (int j = 3; j < 6; ++j) {
            axes.push_back(Cross3(axes[i], axes[j]));
        }
    }

    for (const auto& axis : axes) {
        float lenSq = Dot3(axis, axis);
        if (lenSq < 1e-6f) continue;

        float minA = std::numeric_limits<float>::max();
        float maxA = -std::numeric_limits<float>::max();
        float minB = std::numeric_limits<float>::max();
        float maxB = -std::numeric_limits<float>::max();

        for (int i = 0; i < 8; ++i) {
            float projA = Dot3(cornersA[i], axis);
            minA = std::min(minA, projA);
            maxA = std::max(maxA, projA);

            float projB = Dot3(cornersB[i], axis);
            minB = std::min(minB, projB);
            maxB = std::max(maxB, projB);
        }

        if (maxA < minB || maxB < minA) {
            return false;
        }
    }

    return true;
}

// ==========================================
// Rect square noF
// ==========================================

HammerRectSquare::HammerRectSquare(int X, int Y, int W, int H, float Angle)
    : x(X), y(Y), w(W), h(H), rotation(Angle) {}

HammerVec2 HammerRectSquare::GetCenter() const {
    return HammerVec2((float)x + (float)w / 2.0f, (float)y + (float)h / 2.0f);
}

void HammerRectSquare::GetCorners(HammerVec2 corners[4]) const {
    HammerRectSquareF sqF((float)x, (float)y, (float)w, (float)h, rotation);
    sqF.GetCorners(corners);
}

int HammerRectSquare::HammerRectCollideFaceSquare(const HammerRectSquare& rect) const {
    if (this->x < rect.x + rect.w &&
        this->x + this->w > rect.x &&
        this->y < rect.y + rect.h &&
        this->y + this->h > rect.y) 
    {
        int overlapLeft   = (rect.x + rect.w) - this->x;
        int overlapRight  = (this->x + this->w) - rect.x;
        int overlapTop    = (rect.y + rect.h) - this->y;
        int overlapBottom = (this->y + this->h) - rect.y;

        int minOverlap = overlapLeft;
        int face = 1;

        if (overlapTop < minOverlap) { minOverlap = overlapTop; face = 2; }
        if (overlapRight < minOverlap) { minOverlap = overlapRight; face = 3; }
        if (overlapBottom < minOverlap) { minOverlap = overlapBottom; face = 4; }

        return face;
    }
    return 0;
}

bool HammerRectSquare::HammerRectCollideSquare(const HammerRectSquare& rect) const {
    return (
        this->x < rect.x + rect.w && this->x + this->w > rect.x &&
        this->y < rect.y + rect.h && this->y + this->h > rect.y
    );
}

bool HammerRectSquare::HammerRectCollideRotatedSquare(const HammerRectSquare& rect) const {
    if (this->rotation == 0.0f && rect.rotation == 0.0f) {
        return HammerRectCollideSquare(rect);
    }

    HammerVec2 cornersA[4];
    HammerVec2 cornersB[4];
    GetCorners(cornersA);
    rect.GetCorners(cornersB);

    return TestSAT2D(cornersA, cornersB);
}

// ==========================================
// Rect square F
// ==========================================

HammerRectSquareF::HammerRectSquareF(float X, float Y, float W, float H, float Angle)
    : x(X), y(Y), w(W), h(H), rotation(Angle) {}

HammerVec2 HammerRectSquareF::GetCenter() const {
    return HammerVec2(x + w / 2.0f, y + h / 2.0f);
}

void HammerRectSquareF::GetCorners(HammerVec2 corners[4]) const {
    HammerVec2 center = GetCenter();
    float hw = w / 2.0f;
    float hh = h / 2.0f;

    float c = std::cos(rotation);
    float s = std::sin(rotation);

    HammerVec2 local[4] = {
        HammerVec2(-hw, -hh),
        HammerVec2( hw, -hh),
        HammerVec2( hw,  hh),
        HammerVec2(-hw,  hh)
    };

    for (int i = 0; i < 4; ++i) {
        corners[i].x = center.x + (local[i].x * c - local[i].y * s);
        corners[i].y = center.y + (local[i].x * s + local[i].y * c);
    }
}

int HammerRectSquareF::HammerRectCollideFaceSquareF(const HammerRectSquareF& rect) const {
    if (this->x < rect.x + rect.w &&
        this->x + this->w > rect.x &&
        this->y < rect.y + rect.h &&
        this->y + this->h > rect.y) 
    {
        float overlapLeft   = (rect.x + rect.w) - this->x;
        float overlapRight  = (this->x + this->w) - rect.x;
        float overlapTop    = (rect.y + rect.h) - this->y;
        float overlapBottom = (this->y + this->h) - rect.y;

        float minOverlap = overlapLeft;
        int face = 1;

        if (overlapTop < minOverlap) { minOverlap = overlapTop; face = 2; }
        if (overlapRight < minOverlap) { minOverlap = overlapRight; face = 3; }
        if (overlapBottom < minOverlap) { minOverlap = overlapBottom; face = 4; }

        return face;
    }
    return 0;
}

bool HammerRectSquareF::HammerRectCollideSquareF(const HammerRectSquareF& rect) const {
    return (
        this->x < rect.x + rect.w && this->x + this->w > rect.x &&
        this->y < rect.y + rect.h && this->y + this->h > rect.y
    );
}

bool HammerRectSquareF::HammerRectCollideRotatedSquareF(const HammerRectSquareF& rect) const {
    if (this->rotation == 0.0f && rect.rotation == 0.0f) {
        return HammerRectCollideSquareF(rect);
    }

    HammerVec2 cornersA[4];
    HammerVec2 cornersB[4];
    GetCorners(cornersA);
    rect.GetCorners(cornersB);

    return TestSAT2D(cornersA, cornersB);
}

// ==========================================
// Dedicated OBB Class
// ==========================================

HammerRectOBBF::HammerRectOBBF(float CX, float CY, float HalfW, float HalfH, float Rotation)
    : cx(CX), cy(CY), halfW(HalfW), halfH(HalfH), rotation(Rotation) {}

HammerRectOBBF::HammerRectOBBF(const HammerRectSquareF& rect) {
    HammerVec2 center = rect.GetCenter();
    this->cx = center.x;
    this->cy = center.y;
    this->halfW = rect.w / 2.0f;
    this->halfH = rect.h / 2.0f;
    this->rotation = rect.rotation;
}

void HammerRectOBBF::GetCorners(HammerVec2 corners[4]) const {
    float c = std::cos(rotation);
    float s = std::sin(rotation);

    HammerVec2 local[4] = {
        HammerVec2(-halfW, -halfH),
        HammerVec2( halfW, -halfH),
        HammerVec2( halfW,  halfH),
        HammerVec2(-halfW,  halfH)
    };

    for (int i = 0; i < 4; ++i) {
        corners[i].x = cx + (local[i].x * c - local[i].y * s);
        corners[i].y = cy + (local[i].x * s + local[i].y * c);
    }
}

bool HammerRectOBBF::HammerRectCollideOBBF(const HammerRectOBBF& other) const {
    HammerVec2 cornersA[4];
    HammerVec2 cornersB[4];
    GetCorners(cornersA);
    other.GetCorners(cornersB);

    return TestSAT2D(cornersA, cornersB);
}

// ==========================================
// Circle Classes
// ==========================================

HammerRectCircle::HammerRectCircle(int X, int Y, int R)
    : x(X), y(Y), r(R) {}

bool HammerRectCircle::HammerRectCollideCircle(const HammerRectCircle& rect) const {
    int dx = this->x - rect.x;
    int dy = this->y - rect.y;
    return (dx * dx + dy * dy) <= ((this->r + rect.r) * (this->r + rect.r));
}

bool HammerRectCircle::HammerRectCollideSquare(const HammerRectSquare& rect) const {
    int closestX = std::max(rect.x, std::min(this->x, rect.x + rect.w));
    int closestY = std::max(rect.y, std::min(this->y, rect.y + rect.h));

    int dx = this->x - closestX;
    int dy = this->y - closestY;

    return (dx * dx + dy * dy) <= (this->r * this->r);
}

bool HammerRectCircle::HammerRectCollideRotatedSquare(const HammerRectSquare& rect) const {
    HammerRectCircleF cf((float)x, (float)y, (float)r);
    HammerRectSquareF sqF((float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h, rect.rotation);
    return cf.HammerRectCollideRotatedSquareF(sqF);
}

HammerRectCircleF::HammerRectCircleF(float X, float Y, float R)
    : x(X), y(Y), r(R) {}

bool HammerRectCircleF::HammerRectCollideCircleF(const HammerRectCircleF& rect) const {
    float dx = this->x - rect.x;
    float dy = this->y - rect.y;
    return (dx * dx + dy * dy) <= ((this->r + rect.r) * (this->r + rect.r));
}

bool HammerRectCircleF::HammerRectCollideSquareF(const HammerRectSquareF& rect) const {
    float closestX = std::max(rect.x, std::min(this->x, rect.x + rect.w));
    float closestY = std::max(rect.y, std::min(this->y, rect.y + rect.h));

    float dx = this->x - closestX;
    float dy = this->y - closestY;

    return (dx * dx + dy * dy) <= (this->r * this->r);
}

bool HammerRectCircleF::HammerRectCollideRotatedSquareF(const HammerRectSquareF& rect) const {
    if (rect.rotation == 0.0f) {
        return HammerRectCollideSquareF(rect);
    }

    HammerVec2 center = rect.GetCenter();
    
    // Transform circle center into rect's unrotated local space
    float dx = this->x - center.x;
    float dy = this->y - center.y;

    float c = std::cos(-rect.rotation);
    float s = std::sin(-rect.rotation);

    float localX = dx * c - dy * s;
    float localY = dx * s + dy * c;

    float halfW = rect.w / 2.0f;
    float halfH = rect.h / 2.0f;

    float closestX = Clamp(localX, -halfW, halfW);
    float closestY = Clamp(localY, -halfH, halfH);

    float distX = localX - closestX;
    float distY = localY - closestY;

    return (distX * distX + distY * distY) <= (this->r * this->r);
}

// ==========================================
// Sphere Classes
// ==========================================

HammerRectSphere::HammerRectSphere(int X, int Y, int Z, int R)
    : x(X), y(Y), z(Z), r(R) {}

bool HammerRectSphere::HammerRectCollideSphere(const HammerRectSphere& rect) const {
    int dx = this->x - rect.x;
    int dy = this->y - rect.y;
    int dz = this->z - rect.z;
    return (dx * dx + dy * dy + dz * dz) <= ((this->r + rect.r) * (this->r + rect.r));
}

bool HammerRectSphere::HammerRectCollideCube(const HammerRectCube& cube) const {
    int closestX = std::max(cube.x, std::min(this->x, cube.x + cube.w));
    int closestY = std::max(cube.y, std::min(this->y, cube.y + cube.h));
    int closestZ = std::max(cube.z, std::min(this->z, cube.z + cube.d));

    int dx = this->x - closestX;
    int dy = this->y - closestY;
    int dz = this->z - closestZ;

    return (dx * dx + dy * dy + dz * dz) <= (this->r * this->r);
}

bool HammerRectSphere::HammerRectCollideRotatedCube(const HammerRectCube& cube) const {
    HammerRectSphereF sf((float)x, (float)y, (float)z, (float)r);
    HammerRectCubeF cbF((float)cube.x, (float)cube.y, (float)cube.z, (float)cube.w, (float)cube.h, (float)cube.d, cube.rotX, cube.rotY, cube.rotZ);
    return sf.HammerRectCollideRotatedCubeF(cbF);
}

HammerRectSphereF::HammerRectSphereF(float X, float Y, float Z, float R)
    : x(X), y(Y), z(Z), r(R) {}

bool HammerRectSphereF::HammerRectCollideSphereF(const HammerRectSphereF& rect) const {
    float dx = this->x - rect.x;
    float dy = this->y - rect.y;
    float dz = this->z - rect.z;
    return (dx * dx + dy * dy + dz * dz) <= ((this->r + rect.r) * (this->r + rect.r));
}

bool HammerRectSphereF::HammerRectCollideCubeF(const HammerRectCubeF& cube) const {
    float closestX = std::max(cube.x, std::min(this->x, cube.x + cube.w));
    float closestY = std::max(cube.y, std::min(this->y, cube.y + cube.h));
    float closestZ = std::max(cube.z, std::min(this->z, cube.z + cube.d));

    float dx = this->x - closestX;
    float dy = this->y - closestY;
    float dz = this->z - closestZ;

    return (dx * dx + dy * dy + dz * dz) <= (this->r * this->r);
}

bool HammerRectSphereF::HammerRectCollideRotatedCubeF(const HammerRectCubeF& cube) const {
    if (cube.rotX == 0.0f && cube.rotY == 0.0f && cube.rotZ == 0.0f) {
        return HammerRectCollideCubeF(cube);
    }

    float cx = cube.x + cube.w / 2.0f;
    float cy = cube.y + cube.h / 2.0f;
    float cz = cube.z + cube.d / 2.0f;

    HammerVec3 localSphereCenter(this->x - cx, this->y - cy, this->z - cz);

    float cz_r = std::cos(-cube.rotZ), sz_r = std::sin(-cube.rotZ);
    float cy_r = std::cos(-cube.rotY), sy_r = std::sin(-cube.rotY);
    float cx_r = std::cos(-cube.rotX), sx_r = std::sin(-cube.rotX);

    // Inverse Z
    float x1 = localSphereCenter.x * cz_r - localSphereCenter.y * sz_r;
    float y1 = localSphereCenter.x * sz_r + localSphereCenter.y * cz_r;
    float z1 = localSphereCenter.z;

    // Inverse Y
    float x2 =  x1 * cy_r + z1 * sy_r;
    float z2 = -x1 * sy_r + z1 * cy_r;
    float y2 =  y1;

    // Inverse X
    float y3 = y2 * cx_r - z2 * sx_r;
    float z3 = y2 * sx_r + z2 * cx_r;
    float x3 = x2;

    float hw = cube.w / 2.0f;
    float hh = cube.h / 2.0f;
    float hd = cube.d / 2.0f;

    float closestX = Clamp(x3, -hw, hw);
    float closestY = Clamp(y3, -hh, hh);
    float closestZ = Clamp(z3, -hd, hd);

    float dx = x3 - closestX;
    float dy = y3 - closestY;
    float dz = z3 - closestZ;

    return (dx * dx + dy * dy + dz * dz) <= (this->r * this->r);
}