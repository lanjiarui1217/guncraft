#pragma once
typedef unsigned long long ull;
constexpr ull Chunksize=16;
constexpr ull WorldHeight=256;
constexpr float waterl=64;
extern ull Rendersize,tick;

struct worldtype
{
	enum
	{
		Overworld
	};
};
typedef unsigned WorldType;
