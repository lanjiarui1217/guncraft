#pragma once

#include "framework.h"
#include <linux/input.h>

struct Keyboard
{
    bool keys[KEY_MAX+1]{};

    bool operator[](int key) const
    {
        return key>=0 && key<=KEY_MAX && keys[key];
    }

    bool shift() const
    {
        return keys[KEY_LEFTSHIFT] || keys[KEY_RIGHTSHIFT];
    }

    bool ctrl() const
    {
        return keys[KEY_LEFTCTRL] || keys[KEY_RIGHTCTRL];
    }

    bool alt() const
    {
        return keys[KEY_LEFTALT] || keys[KEY_RIGHTALT];
    }

    bool capslock() const
    {
        return keys[KEY_CAPSLOCK];
    }
};

extern Keyboard keyboard;

bool initkeyboard();
void pollkeyboard();
void closekeyboard();

struct KeyboardEnv
{
    KeyboardEnv();
    ~KeyboardEnv();
};