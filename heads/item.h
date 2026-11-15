#pragma once
#include "framework.h"
#include "direct3d.h"
#include "iteminfo.h"
struct Item
{
	itemtype tp;
	ull num;
};
struct ItemDrawUtil
{
	static Texture texture[ITEMNUM],hotbar,hotbar1;
	static VtxBuf vertexbuffer[ITEMNUM];
	static void draw();
};

class ItemIniterDeler
{
	static ull downnum;
public:
	static bool down();
	ItemIniterDeler();
	~ItemIniterDeler();
};
