/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the MIT license
 * LICENSE file in the root directory of this source tree.
 */

#ifndef HAMMERRECT_H
#define HAMMERRECT_H

class HammerRectCube{
public:
	int x;
	int y;
	int z;

	int w;
	int h;
	int d;

	bool HammerRectCollideCube(HammerRectCube rect);
	int HammerRectCollideFaceCube(HammerRectCube rect);
	HammerRectCube(int X, int Y, int Z, int W, int H, int D);
};


class HammerRectCubeF{
public:
	float x;
	float y;
	float z;

	float w;
	float h;
	float d;
	bool HammerRectCollideCubeF(HammerRectCubeF rect);
	int HammerRectCollideFaceCubeF(HammerRectCubeF rect);
	HammerRectCubeF(float X, float Y, float Z, float W, float H, float D);
};



class HammerRectSquare{
public:
	int x;
	int y;

	int w;
	int h;
	bool HammerRectCollideSquare(HammerRectSquare rect);
	int HammerRectCollideFaceSquare(HammerRectSquare rect);
	HammerRectSquare(int X, int Y, int W, int H);
};

class HammerRectSquareF{
public:
	float x;
	float y;

	float w;
	float h;
	bool HammerRectCollideSquareF(HammerRectSquareF rect);
	int HammerRectCollideFaceSquareF(HammerRectSquareF rect);
	HammerRectSquareF(float X, float Y, float W, float H);
};


class HammerRectCircle{
public:
    int x;
    int y;
    int r;

    bool HammerRectCollideCircle(HammerRectCircle rect);
    bool HammerRectCollideSquare(HammerRectSquare rect);
    HammerRectCircle(int X, int Y, int R);
};

class HammerRectCircleF{
public:
    float x;
    float y;
    float r;

    bool HammerRectCollideCircleF(HammerRectCircleF rect);
    bool HammerRectCollideSquareF(HammerRectSquareF rect);
    HammerRectCircleF(float X, float Y, float R);
};

// this was such a shity proccess, fuck this shit
// whoever has writen this shit need to stop drugs
// idk if it works i hope, i am not fixing this shit

class HammerRectSphere{
public:
    int x;
    int y;
    int z;
    int r;

    bool HammerRectCollideSphere(HammerRectSphere rect);
    bool HammerRectCollideCube(HammerRectCube cube);
    HammerRectSphere(int X, int Y, int Z, int R);
};

class HammerRectSphereF{
public:
    float x;
    float y;
    float z;
    float r;

    bool HammerRectCollideSphereF(HammerRectSphereF rect);
    bool HammerRectCollideCubeF(HammerRectCubeF cube);
    HammerRectSphereF(float X, float Y, float Z, float R);
};

#endif
