#pragma once

#include "framework.h"

constexpr ull screenmaxw=900,screenmaxh=400;

struct RGB { uint8_t r,g,b; };

extern RGB screen[screenmaxh][screenmaxw];
extern ull screenh,screenw;

void render(ull h=screenh,ull w=screenw,RGB in[screenmaxh][screenmaxw]=screen);

struct RenderEnv
{
	RenderEnv();
	~RenderEnv();
};

struct Camera
{
	fvct pos;
	fvct target;
	fvct up;
	float fov;
	float nearClip;
	float farClip;
	Camera(
		fvct p=fvct(0,0,-5),
		fvct t=fvct(0,0,0),
		fvct u=fvct(0,1,0),
		float f=90.0f,
		float n=0.01f,
		float far=1000.0f)
		: pos(p),target(t),up(u),fov(f),nearClip(n),farClip(far)
	{}
};

void movecamera(Camera& cm,int dx,int dy);

extern Camera camera;
void clearscreen();

fvct pia(const Camera& cm,fvct);
void renderline(fvct p1,fvct p2,RGB c1,RGB c2);
void renderline0(fvct p1,fvct p2,RGB c1,RGB c2);
void rendertriangle(fvct p1,fvct p2,fvct p3,RGB c1,RGB c2,RGB c3);
void rendertriangle0(fvct p1,fvct p2,fvct p3,RGB c1,RGB c2,RGB c3);