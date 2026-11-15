#pragma once
#include "framework.h"
#include "_math.h"
#include "blockinfo.h"
// 轮廓线
const Box line[BLOCKNUM]=
{
Box(fvct(0,0,0),fvct(0,0,0)),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
};
// 碰撞箱
const Box coll[BLOCKNUM]=
{
Box(fvct(0,0,0),fvct(0,0,0)),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
};
// 支撑箱
const Box supp[BLOCKNUM]=
{
Box(fvct(0,0,0),fvct(0,0,0)),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(),
};
//光学箱
const Box occl[BLOCKNUM]=
{
Box(fvct(0,0,0),fvct(0,0,0)),
Box(),
Box(),
Box(),
Box(),
Box(fvct(0,0,0),fvct(0,0,0)),
Box(),
Box(),
Box(),
Box(),
Box(),
Box(fvct(0,0,0),fvct(0,0,0)),
Box(),
Box(),
};
// 互动箱
const Box intr[BLOCKNUM]=
{
Box(fvct(0,0,0),fvct(0,0,0)),
Box(fvct(0,0,0),fvct(0,0,0)),
Box(fvct(0,0,0),fvct(0,0,0)),
Box(fvct(0,0,0),fvct(0,0,0)),
Box(fvct(0,0,0),fvct(0,0,0)),
Box(fvct(0,0,0),fvct(0,0,0)),
Box(fvct(0,0,0),fvct(0,0,0)),
Box(fvct(0,0,0),fvct(0,0,0)),
Box(fvct(0,0,0),fvct(0,0,0)),
Box(fvct(0,0,0),fvct(0,0,0)),
Box(fvct(0,0,0),fvct(0,0,0)),
Box(fvct(0,0,0),fvct(0,0,0)),
Box(fvct(0,0,0),fvct(0,0,0)),
Box(fvct(0,0,0),fvct(0,0,0)),
};
static constexpr float Inf=(float)(~0ull);

// 窒息
const bool suff[BLOCKNUM]		={0,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1};

// 部分透明
const bool trans[BLOCKNUM]		={0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,		0,		0};

// 导体
const bool cond[BLOCKNUM]		={0,		1,		1,		1,		1,		0,		1,		1,		1,		1,		1,		0,		1,		1};

//
const bool subs[BLOCKNUM]		={0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0};

// 固体
const bool solid[BLOCKNUM]		={0,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1};

// 硬度
const float hard[BLOCKNUM]		={0.0f,		Inf,	1.0f,	1.0f,	1.0f,	0.3f,	1.0f,	9.0f,	4.0f,	3.0f,	0.0f,	0.2f,	0.3f,	3.0f};

// 爆炸抗性
const float boomhard[BLOCKNUM]	={0.0f,		Inf,	0.6f,	0.5f,	0.5f,	0.3f,	0.5f,	6.0f,	2.0f,	3.0f,	0.0f,	0.2f,	1.5f,	3.0f};

// 透光度
const unsigned trslt[BLOCKNUM]	={0,		16,		16,		16,		16,		0,		16,		16,		16,		16,		16,		1,		0,		0};

// 亮度
const unsigned light[BLOCKNUM]	={0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		15,		0};

// 掉落物
const itemtype blkdrop[BLOCKNUM]=
{
	item::air,item::bedrock,item::grasssoil,item::soil,item::sand,item::glass,
	item::snowsoil,item::stone,item::logwood,item::woodplank,item::TNT,
	item::leaf,item::glowstone,item::rawiron
};


// 方块
struct Block
{
	blocktype tp;

	enum direction
	{
		// p principal 主要
		px0=0u,px1=1u,py0=2u,py1=3u,pz0=4u,pz1=5u,
		// a auxiliary 辅助
		ax0=0u,ax1=8u,ay0=16u,ay1=24u,az0=32u,az1=40u,
		// sym symmetry 对称
		sym=64u
	};

	// 朝向
	unsigned drc;

	// 位置
	fvct pos;

	// 可见
	rndsps vis;

	// 天光
//	unsigned skylight[6];

	// 方块光
//	unsigned blklight[6];

	Block();
	Block(blocktype,unsigned,fvct,rndsps=0x3f/*,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15*/);
};

extern unsigned nowskylight;
inline unsigned glight(vct3<ll>,ull);

// 绘制方块工具
struct BlockDrawUtil
{
	static Texture texture[BLOCKNUM][6];
	static VtxBuf vertexbuffer[BLOCKNUM];
	static void draw(Block,unsigned char dr);
	static void draw(Block,fmat);
	static void drawline(Box);
};

// 初始化方块
class BlockIniterDeler
{
	static ull downnum;
public:
	static bool down();
	BlockIniterDeler();
	~BlockIniterDeler();
};
