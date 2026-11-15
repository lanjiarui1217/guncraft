#pragma once
#include "Entity.h"
#include "item.h"
struct Player :Entity
{
	clock_t lstenderpearltime;
	clock_t lstshoottime;
	clock_t lstgrenadetime;
	fvct cmp,lkt;
	float angle[3];
	Player();
	Player(fvct);
	~Player();
	void upd();
	void render();
	fvct realpos() { return p+fvct(0,31.0f/32,0); }
	void updatepos(float,float);
	void updatelkt();
	ull chooseind;
	Item itemlist[10];
};
extern std::vector<Player> vp;
extern ull thisind;

// this player
#define tspl vp[thisind]
