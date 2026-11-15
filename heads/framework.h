#pragma once
#include "targetver.h"
#include <windows.h>

typedef unsigned long long ull;
typedef long long ll;
typedef unsigned char byte;
#define sfrls(p) ((p!=nullptr)&&(p->Release(),p=nullptr))
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "direct3d.h"
#include "WorldInfo.h"
#include "_math.h"
#include <d2d1.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <map>
#include <queue>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <basetsd.h>
#include <windef.h>
#include "blockinfo.h"
#include "biomeinfo.h"
#include "particleinfo.h"
#include "iteminfo.h"
#include <cassert>
//#define DBG

extern float fps,tps;
extern bool gameover;

template<typename... Args>
void debug(const char* fmt,Args... args)
{
	char s[10000];
	sprintf(s,fmt,args...);
	//MessageBoxA(nullptr,s,"Debug",MB_OK);
	OutputDebugStringA(s);
}


template<typename... Args>
void error(const char* fmt,Args... args)
{
	char s[10000];
	sprintf(s,fmt,args...);
	debug("\n----Error!----\n%s\n----Error!----\n\n",s);
	throw s;
}


