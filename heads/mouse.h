#pragma once
#include "framework.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

struct Mouse
{
    int dx=0;
    int dy=0;
    int wheel=0;

    bool left=false;
    bool right=false;
    bool middle=false;
};


extern Mouse mouse;

bool initmouse(bool grab=true);
void pollmouse();
void closemouse();

struct MouseEnv
{
	MouseEnv();
	~MouseEnv();
};