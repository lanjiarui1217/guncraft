#pragma once
#include "Chunk.h"
#include "Player.h"
#include <thread>
#include <mutex>
#include <future>
#include <unordered_map>
typedef unsigned LightChunk[Chunksize][WorldHeight][Chunksize][6];
typedef std::unordered_map<std::pair<ll,ll>,LightChunk,pllhash> LightMap;
struct World
{
	std::map<std::pair<ll,ll>,Chunk*> mp;
	LightMap skylight,blklight;
	std::map<std::pair<ll,ll>,bool> ltmp;
	std::map<std::pair<ll,ll>,std::future<void>> mpthread;
	std::map<std::pair<ll,ll>,std::mutex> mpmutex;
	std::mutex totmutex;

	std::multiset<std::pair<float,vct3<ll>>,std::greater<std::pair<float,vct3<ll>>>> trb;
	void _vison(vct3<ll>);
	void _updvis(vct3<ll>);
	void vison(vct3<ll>);
	void updvis(vct3<ll>);
	Block _getbl(vct3<ll>);
	bool _setbl(vct3<ll>,blocktype);
	void genlight_round(vct3<ll>,int);
	Block getbl(vct3<ll>);
	bool setbl(vct3<ll>,blocktype);
	bool bxbl(const Box&);
	bool bxbl(fvct);
	World();
	~World();
	void pick(Player&);
	void use(Player&);
	void newchunk(ll,ll);
	void update(bool=0);
	bool shdupd();
	void render();
	Chunk* getchunk(vct3<ll>);
};
extern World wrld;