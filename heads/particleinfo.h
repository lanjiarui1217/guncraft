#pragma once
#include <string>
#include "../Resource.h"
typedef unsigned long long ull;
typedef unsigned particle;
struct particletype { enum _ { block,rain,PARTICLENUM }; };
constexpr ull PARTICLENUM=(ull)particletype::PARTICLENUM;
constexpr float ptcw[PARTICLENUM]={1.0/8,1.0/16},
				ptch[PARTICLENUM]={1.0/8,1.0/16};
constexpr int ptctexid[PARTICLENUM]={IDP_BLK,IDP_RAIN};