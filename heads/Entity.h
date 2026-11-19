#pragma once
#include "framework.h"

// ʵ��
struct Entity
{
	fvct p,v;
	Box coll;
	bool exist=1;
	virtual void upd()=0;
	virtual void render()=0;
	virtual fvct realpos()=0;
	virtual ~Entity() {}
	bool entitymove(fvct move,Box coll,float=1.0f/512);
	bool entitymove(fvct move,float=1.0f/512);
};

struct fallsand :Entity
{
	fallsand(fvct);
	void upd();
	void render();
	~fallsand();
	inline fvct realpos() { return p+fvct(0.5f,0.5f,0.5f); }
};
#include "Player.h"
#include "direct3d.h"
struct enderpearl :Entity
{
	// VtxBuf vb;
	void* pp=nullptr;
	static constexpr float g=-12.0f;
	enderpearl(void*);
	void upd();
	void render();
	~enderpearl();
	inline fvct realpos() { return p; }
};

struct drop :Entity
{
	itemtype tp;
	float angle;
	drop(fvct,itemtype);
	void upd();
	void render();
	~drop();
	inline fvct realpos() { return p+fvct(0.5f,0.5f,0.5f); }
};
struct actTNT :Entity
{
	float time;
	actTNT(fvct);
	void upd();
	void render();
	~actTNT();
	inline fvct realpos() { return p+fvct(0.5f,0.5f,0.5f); }
};
struct AmmoM1943 :Entity
{
	// VtxBuf vb;
	// Texture tex;
	AmmoM1943(fvct,fvct);
	void upd();
	void render();
	~AmmoM1943();
	inline fvct realpos() { return p; }
};
struct Grenade :Entity
{
	// VtxBuf vb;
	// Texture tex;
	Grenade(fvct,fvct);
	void upd();
	void render();
	~Grenade();
	inline fvct realpos() { return p; }
};
struct GuidedMissile :Entity
{
	//VtxBuf vb;
	//Texture tex;
	GuidedMissile(fvct,fvct);
	void upd();
	void render();
	~GuidedMissile();
	inline fvct realpos() { return p; }
};

extern std::vector<Entity*> ve;

extern GuidedMissile* pmissile;