#pragma once
#include "framework.h"
#include "direct3d.h"
#include "block.h"
#include <set>
struct Chunk
{
	constexpr static ull blsize=5000;
	ll x,z;
	Chunk(ll x,ll z):x(x),z(z)
	{
		memset(b,0,sizeof(b));
		for(ull i(0);i<Chunksize;++i)
		{
			for(ull j(0);j<WorldHeight;++j)
			{
				for(ull k(0);k<Chunksize;++k)
				{
					b[i][j][k].pos=fvct((float)(ll)(i+Chunksize*x),(float)j,(float)(ll)(k+Chunksize*z));
				}
			}
		}
		rd.clear();
	//	memset(height,0,sizeof(height));
		memset(temp,0,sizeof(temp));
		memset(humi,0,sizeof(humi));
		memset(biom,0,sizeof(biom));
	}
	template<typename T>
	Chunk(ll _x,ll _z,T gen)
	{
		x=_x,z=_z;
		for(ull i(0);i<Chunksize;++i)
		{
			for(ull j(0);j<WorldHeight;++j)
			{
				for(ull k(0);k<Chunksize;++k)
				{
					b[i][j][k].pos=fvct((float)(ll)(i+Chunksize*x),(float)j,(float)(ll)(k+Chunksize*z));
				}
			}
		}
		gen(*this);
		updvis();
	}
	Block b[Chunksize][WorldHeight][Chunksize];
//	unsigned skylight[Chunksize][WorldHeight][Chunksize][6],blklight[Chunksize][WorldHeight][Chunksize][6];
	std::unordered_set<vct3<ull>,vct3hash<ull>> rd;
	void updvis();
	void render(rndsps dr);
//	unsigned char height[Chunksize][Chunksize];
	float temp[Chunksize][Chunksize],humi[Chunksize][Chunksize];
	biome biom[Chunksize][Chunksize];
};
#define getposChunk(v) (vct3<ull>(((v)[1]%Chunksize+Chunksize)%Chunksize,(v)[2],((v)[3]%Chunksize+Chunksize)%Chunksize))
#define getChunkpos(v) vct3<ll>((ll)floor((v)[1]*1.0f/(1.0f*Chunksize)),0,(ll)floor((v)[3]*1.0f/(1.0f*Chunksize)))