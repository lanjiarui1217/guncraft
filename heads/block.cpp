#include "block.h"
#include "render.h"
#include "render.h"
#include "World.h"
#include "../resource.h"

// 天光
unsigned nowskylight=15;

unsigned glight(vct3<ll> v,ull i)
{
	if(v.y>=WorldHeight||v.y<0) return 15;
	vct3<ll> b=getChunkpos(v);
	vct3<ull> p=getposChunk(v);
	auto it1=wrld.skylight.find(std::make_pair(b.x,b.z));
	auto it2=wrld.blklight.find(std::make_pair(b.x,b.z));
	unsigned sl=(it1==wrld.skylight.end())?0:((it1->second)[p.x][p.y][p.z][i]);
	unsigned bl=(it2==wrld.blklight.end())?0:((it2->second)[p.x][p.y][p.z][i]);
	return std::max(bl+15,nowskylight+sl)-15;
}

// 每种方块 6 个面的单色（先纯色、不要贴图、不透明）
// 顺序：ZY1,ZY2,XZ1,XZ2,XY1,XY2
const RGB blockcolor[BLOCKNUM][6]=
{
	{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}, // air
	{{110,110,110},{70,70,70},{70,70,70},{70,70,70},{70,70,70},{70,70,70}}, // bedrock
	{{90,160,60},{110,80,40},{90,60,30},{110,180,60},{90,160,60},{90,160,60}}, // grasssoil
	{{121,85,58},{100,70,48},{100,70,48},{100,70,48},{121,85,58},{121,85,58}}, // soil
	{{194,178,128},{170,152,105},{170,152,105},{170,152,105},{194,178,128},{194,178,128}}, // sand
	{{200,230,255},{170,200,230},{170,200,230},{170,200,230},{200,230,255},{200,230,255}}, // glass
	{{240,245,250},{200,210,220},{200,210,220},{235,240,250},{240,245,250},{240,245,250}}, // snowsoil
	{{128,128,128},{100,100,100},{100,100,100},{100,100,100},{128,128,128},{128,128,128}}, // stone
	{{101,67,33},{80,53,27},{120,84,44},{120,84,44},{101,67,33},{101,67,33}}, // logwood
	{{160,120,60},{140,105,55},{140,105,55},{140,105,55},{160,120,60},{160,120,60}}, // woodplank
	{{200,40,40},{150,25,25},{180,60,60},{180,60,60},{200,40,40},{200,40,40}}, // TNT
	{{40,120,40},{30,100,30},{30,100,30},{30,100,30},{40,120,40},{40,120,40}}, // leaf
	{{240,240,180},{220,200,120},{220,200,120},{220,200,120},{240,240,180},{240,240,180}}, // glowstone
	{{140,140,140},{120,120,120},{120,120,120},{120,120,120},{140,140,140},{140,140,140}}, // ironore
};

// 6 个面的四个顶点（局部坐标，0..1，面内顺时针）
struct FaceV { fvct v[4]; };
static const FaceV facev[6]=
{
	{{{0,0,0},{0,0,1},{0,1,1},{0,1,0}}}, // ZY1 (x=0)
	{{{1,0,0},{1,1,0},{1,1,1},{1,0,1}}}, // ZY2 (x=1)
	{{{0,0,0},{1,0,0},{1,0,1},{0,0,1}}}, // XZ1 (y=0)
	{{{0,1,0},{0,1,1},{1,1,1},{1,1,0}}}, // XZ2 (y=1)
	{{{0,0,0},{0,1,0},{1,1,0},{1,0,0}}}, // XY1 (z=0)
	{{{0,0,1},{1,0,1},{1,1,1},{0,1,1}}}, // XY2 (z=1)
};

inline RGB shade(const RGB& c,unsigned vl)
{
	// vl: 0~15；加环境光下限，避免完全没光照的面变成纯黑
	const unsigned amb=6; // 约 20% 亮度
	unsigned l=amb+vl*2; // 6~36
	if(l>36) l=36;
	return {(uint8_t)((c.r*l)/36),(uint8_t)((c.g*l)/36),(uint8_t)((c.b*l)/36)};
}

void BlockDrawUtil::draw(Block b,unsigned char dr)
{
	if(b.tp>=BLOCKNUM||b.tp==block::air) return;
	vct3<ll> ip=floor(b.pos);
	for(ull i(0);i<6;++i)
	{
		if(!(dr&(1<<i))) continue;
		unsigned vl=glight(ip,i);
		RGB c=shade(blockcolor[b.tp][i],vl);
		const FaceV& f=facev[i];
		fvct a=f.v[0]+b.pos,c1=f.v[1]+b.pos,d=f.v[2]+b.pos,d1=f.v[3]+b.pos;
		rendertriangle(a,c1,d,c,c,c);
		rendertriangle(a,d,d1,c,c,c);
	}
}

void BlockDrawUtil::drawline(Box b)
{
	float x1=b.min.x,x2=b.max.x,y1=b.min.y,y2=b.max.y,z1=b.min.z,z2=b.max.z;
	const RGB c{255,255,255};
	const fvct p[8]={
		{x1,y1,z1},{x1,y1,z2},{x1,y2,z1},{x1,y2,z2},
		{x2,y1,z1},{x2,y1,z2},{x2,y2,z1},{x2,y2,z2}};
	static const int e[12][2]={{0,1},{1,3},{3,2},{2,0},{4,5},{5,7},{7,6},{6,4},{0,4},{1,5},{2,6},{3,7}};
	for(int i(0);i<12;++i) renderline(p[e[i][0]],p[e[i][1]],c,c);
}

ull BlockIniterDeler::downnum=0;
bool BlockIniterDeler::down()
{
	return downnum>0;
}
BlockIniterDeler::BlockIniterDeler()
{
	if(downnum)
	{
		++downnum;
		return;
	}
	++downnum;
}
BlockIniterDeler::~BlockIniterDeler()
{
	if(downnum) --downnum;
}

Block::Block()
{
	tp=block::air;
	drc=direction::py0|direction::ax0;
	pos=fvct(0,0,0);
	vis=0x3f;
}
Block::Block(blocktype tp,unsigned drc,fvct pos,rndsps vis) : tp(tp),drc(drc),pos(pos),vis(vis)
{}
