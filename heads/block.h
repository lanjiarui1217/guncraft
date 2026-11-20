#pragma once
#include "framework.h"
#include "render.h"
#include "_math.h"
#include "blockinfo.h"
// ������
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
// ��ײ��
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
// ֧����
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
//��ѧ��
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
// ������
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

// ��Ϣ
const bool suff[BLOCKNUM]		={0,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1};

// ����͸��
const bool trans[BLOCKNUM]		={0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		1,		0,		0};

// ����
const bool cond[BLOCKNUM]		={0,		1,		1,		1,		1,		0,		1,		1,		1,		1,		1,		0,		1,		1};

//
const bool subs[BLOCKNUM]		={0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0};

// ����
const bool solid[BLOCKNUM]		={0,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1,		1};

// Ӳ��
const float hard[BLOCKNUM]		={0.0f,		Inf,	1.0f,	1.0f,	1.0f,	0.3f,	1.0f,	9.0f,	4.0f,	3.0f,	0.0f,	0.2f,	0.3f,	3.0f};

// ��ը����
const float boomhard[BLOCKNUM]	={0.0f,		Inf,	0.6f,	0.5f,	0.5f,	0.3f,	0.5f,	6.0f,	2.0f,	3.0f,	0.0f,	0.2f,	1.5f,	3.0f};

// ͸���
const unsigned trslt[BLOCKNUM]	={0,		16,		16,		16,		16,		0,		16,		16,		16,		16,		16,		1,		0,		0};

// ����
const unsigned light[BLOCKNUM]	={0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		15,		0};

// ������
const itemtype blkdrop[BLOCKNUM]=
{
	item::air,item::bedrock,item::grasssoil,item::soil,item::sand,item::glass,
	item::snowsoil,item::stone,item::logwood,item::woodplank,item::TNT,
	item::leaf,item::glowstone,item::rawiron
};


// ����
struct Block
{
	blocktype tp;

	enum direction
	{
		// p principal ��Ҫ
		px0=0u,px1=1u,py0=2u,py1=3u,pz0=4u,pz1=5u,
		// a auxiliary ����
		ax0=0u,ax1=8u,ay0=16u,ay1=24u,az0=32u,az1=40u,
		// sym symmetry �Գ�
		sym=64u
	};

	// ����
	unsigned drc;

	// λ��
	fvct pos;

	// �ɼ�
	rndsps vis;

	// ���
//	unsigned skylight[6];

	// �����
//	unsigned blklight[6];

	Block();
	Block(blocktype,unsigned,fvct,rndsps=0x3f/*,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15,unsigned=15*/);
};

extern unsigned nowskylight;
unsigned glight(vct3<ll>,ull);

// ���Ʒ��鹤��
struct BlockDrawUtil
{
	// static Texture texture[BLOCKNUM][6];
	// static VtxBuf vertexbuffer[BLOCKNUM];
	static void draw(Block,unsigned char dr);
	// static void draw(Block,fmat);
	static void drawline(Box);
};

// ��ʼ������
class BlockIniterDeler
{
	static ull downnum;
public:
	static bool down();
	BlockIniterDeler();
	~BlockIniterDeler();
};
