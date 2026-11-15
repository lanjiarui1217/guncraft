#pragma warning(push)
#pragma warning(disable:4005)
#include "block.h"
#include "direct3d.h"
#include "../Resource.h"
#include "World.h"
#pragma warning(pop)
extern HWND hwnd;
D3DXHANDLE pixelconst;
static ID3DXConstantTable* consttable=0;
static ID3DXBuffer* tmpshader=0;
static ID3DXBuffer* errorbf=0;
static IDirect3DPixelShader9* shader=0;
static D3DXHANDLE lgthd;
static D3DXCONSTANT_DESC lgtdesc;

// Ìì¹â
unsigned nowskylight;

inline unsigned glight(vct3<ll> v,ull i)
{
	if(v.y>=WorldHeight||v.y<0) return 15;
	vct3<ll> b=getChunkpos(v);
	vct3<ull> p=getposChunk(v);
	auto it1=wrld.skylight.find(std::make_pair(b.x,b.z));
	auto it2=wrld.blklight.find(std::make_pair(b.x,b.z));
	unsigned sl=(it1==wrld.skylight.end())?0:((it1->second)[p.x][p.y][p.z][i]);
	unsigned bl=(it2==wrld.blklight.end())?0:((it2->second)[p.x][p.y][p.z][i]);
	return max(bl+15,nowskylight+sl)-15;
}
void BlockDrawUtil::draw(Block b,unsigned char dr)
{
	d3ddev->SetPixelShader(shader);
//	unsigned vl=15;
	fmtx mt;
	d3ddev->SetTransform(D3DTS_WORLD,D3DXMatrixTranslation(&mt,b.pos.x,b.pos.y,b.pos.z));
	d3ddev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
	d3ddev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	d3ddev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	d3ddev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	if(b.tp==block::glass)
	{
		for(ull i(0);i<6;++i)
		{
			if(dr&(1<<i))
			{
				unsigned vl=glight(floor(b.pos),i)*16u;
				float v[4]={vl/256.0f,vl/256.0f,vl/256.0f,vl/256.0f};
				consttable->SetFloatArray(d3ddev,lgthd,v,4);
				d3ddev->SetTexture(0,texture[b.tp][i]);
				d3ddev->SetStreamSource(0,vertexbuffer[b.tp],(unsigned)i*sizeof(Vtx3::Vertex)*6,sizeof(Vtx3::Vertex));
				d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST,0,2u);
			}
		}
		d3ddev->SetPixelShader(NULL);
		return;
	}
	if(b.tp==block::leaf)
	{
		for(ull i(0);i<6;++i)
		{
			if(dr&(1<<i))
			{
				unsigned vl=glight(floor(b.pos),i)*16u;
				float v[4]={vl/256.0f,vl/256.0f,vl/256.0f,vl/256.0f};
				consttable->SetFloatArray(d3ddev,lgthd,v,4);
				d3ddev->SetTexture(0,texture[b.tp][i]);
				d3ddev->SetStreamSource(0,vertexbuffer[b.tp],(unsigned)i*sizeof(Vtx3::Vertex)*12,sizeof(Vtx3::Vertex));
				d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST,0,4u);
			}
		}
		d3ddev->SetPixelShader(NULL);
		return;
	}
	for(ull i(0);i<6;++i)
	{
		if(dr&(1<<i))
		{
			unsigned vl=glight(floor(b.pos),i)*16u;
			float v[4]={vl/256.0f,vl/256.0f,vl/256.0f,vl/256.0f};
			consttable->SetFloatArray(d3ddev,lgthd,v,4);
			d3ddev->SetTexture(0,texture[b.tp][i]);
			d3ddev->SetStreamSource(0,vertexbuffer[b.tp],(unsigned)i*sizeof(Vtx3::Vertex)*4,sizeof(Vtx3::Vertex));
			d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2u);
		}
	}
	d3ddev->SetPixelShader(NULL);
}
void BlockDrawUtil::draw(Block b,fmat mt)
{
	d3ddev->SetPixelShader(shader);
	d3ddev->SetTransform(D3DTS_WORLD,&mt);
	d3ddev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
	d3ddev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	d3ddev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	d3ddev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	if(b.tp==block::glass)
	{
		for(ull i(0);i<6;++i)
		{
			unsigned vl=glight(floor(b.pos),i)*16u;
			float v[4]={vl/256.0f,vl/256.0f,vl/256.0f,vl/256.0f};
			consttable->SetFloatArray(d3ddev,lgthd,v,4);
			d3ddev->SetTexture(0,texture[b.tp][i]);
			d3ddev->SetStreamSource(0,vertexbuffer[b.tp],(unsigned)i*sizeof(Vtx3::Vertex)*6,sizeof(Vtx3::Vertex));
			d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST,0,2u);
		}
		d3ddev->SetPixelShader(NULL);
		return;
	}
	if(b.tp==block::leaf)
	{
		for(ull i(0);i<6;++i)
		{
			unsigned vl=glight(floor(b.pos),i)*16u;
			float v[4]={vl/256.0f,vl/256.0f,vl/256.0f,vl/256.0f};
			consttable->SetFloatArray(d3ddev,lgthd,v,4);
			d3ddev->SetTexture(0,texture[b.tp][i]);
			d3ddev->SetStreamSource(0,vertexbuffer[b.tp],(unsigned)i*sizeof(Vtx3::Vertex)*12,sizeof(Vtx3::Vertex));
			d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST,0,4u);
		}
		d3ddev->SetPixelShader(NULL);
		return;
	}
	for(ull i(0);i<6;++i)
	{
		unsigned vl=glight(floor(b.pos),i)*16u;
		float v[4]={vl/256.0f,vl/256.0f,vl/256.0f,vl/256.0f};
		consttable->SetFloatArray(d3ddev,lgthd,v,4);
		d3ddev->SetTexture(0,texture[b.tp][i]);
		d3ddev->SetStreamSource(0,vertexbuffer[b.tp],(unsigned)i*sizeof(Vtx3::Vertex)*4,sizeof(Vtx3::Vertex));
		d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2u);
	}
	d3ddev->SetPixelShader(NULL);
}
void BlockDrawUtil::drawline(Box b)
{
	static Texture tex=nullptr;
	if(tex==nullptr)
	{
		SetTexture(tex,IDB_BLACK);
	}
	d3ddev->SetTexture(0,tex);
	d3ddev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
	D3DXMATRIX mt;
	d3ddev->SetTransform(D3DTS_WORLD,D3DXMatrixTranslation(&mt,0,0,0));
	d3ddev->SetTransform(D3DTS_WORLD,&mt);
	float x1=b.min.x,x2=b.max.x,y1=b.min.y,y2=b.max.y,z1=b.min.z,z2=b.max.z;
	Vtx3::Vertex v[][2]=
	{
		{{x1,y1,z1,0,1},{x1,y1,z2,1,1}},
		{{x1,y1,z2,1,1},{x1,y2,z2,1,0}},
		{{x2,y1,z2,1,1},{x2,y1,z1,0,1}},
		{{x2,y2,z2,1,0},{x2,y1,z2,1,1}},
		{{x1,y1,z1,0,1},{x2,y1,z1,1,1}},
		{{x2,y1,z1,1,1},{x2,y2,z1,1,0}},
		{{x2,y2,z1,1,1},{x1,y2,z1,0,1}},
		{{x2,y2,z2,1,0},{x2,y2,z1,1,1}},
		{{x1,y2,z1,1,1},{x1,y1,z1,0,1}},
		{{x1,y2,z1,1,0},{x1,y2,z2,1,1}},
		{{x1,y1,z2,0,1},{x2,y1,z2,1,1}},
		{{x1,y2,z2,1,1},{x2,y2,z2,1,0}},
	};
	d3ddev->DrawPrimitiveUP(D3DPT_LINELIST,12u,v,sizeof(Vtx3::Vertex));
}
Texture BlockDrawUtil::texture[BLOCKNUM][6];
VtxBuf BlockDrawUtil::vertexbuffer[BLOCKNUM];
#ifdef DBG
extern HWND hwnd;
#endif

ull BlockIniterDeler::downnum=0;
bool BlockIniterDeler::down()
{
	return downnum>0;
}
BlockIniterDeler::BlockIniterDeler()
{
#ifdef DBG
	MessageBox(hwnd,L"blockinit",L"init",MB_OK);
#endif
	if(downnum)
	{
		++downnum;
		return;
	}
	HRESULT hr=D3DXCompileShaderFromFile(L"heads/PixelShader_block.hlsl",0,0,"main","ps_2_0",D3DXSHADER_DEBUG,&tmpshader,&errorbf,&consttable);
	if(errorbf)
	{
		debug("%s\n",errorbf->GetBufferPointer());
		sfrls(errorbf);
	}
	d3ddev->CreatePixelShader((DWORD*)tmpshader->GetBufferPointer(),&shader);
	sfrls(tmpshader);
	lgthd=consttable->GetConstantByName(0,"light");
	consttable->GetConstantDesc(lgthd,&lgtdesc,0);
	for(blocktype i(0);i<BLOCKNUM;++i)
	{
		if(i==block::glass)
		{
			float x1=line[i].min.x,x2=line[i].max.x,y1=line[i].min.y,y2=line[i].max.y,z1=line[i].min.z,z2=line[i].max.z;
			Vtx3::Vertex v[][3]=
			{
				{{x1,y1,z1,0,1},{x1,y1,z2,1,1},{x1,y2,z1,0,0}},
				{{x1,y1,z2,1,1},{x1,y2,z2,1,0},{x1,y2,z1,0,0}},
				{{x2,y1,z2,1,1},{x2,y1,z1,0,1},{x2,y2,z1,0,0}},
				{{x2,y2,z2,1,0},{x2,y1,z2,1,1},{x2,y2,z1,0,0}},
				{{x1,y1,z1,0,1},{x2,y1,z1,1,1},{x1,y1,z2,0,0}},
				{{x2,y1,z1,1,1},{x2,y1,z2,1,0},{x1,y1,z2,0,0}},
				{{x2,y2,z1,1,1},{x1,y2,z1,0,1},{x1,y2,z2,0,0}},
				{{x2,y2,z2,1,0},{x2,y2,z1,1,1},{x1,y2,z2,0,0}},
				{{x2,y1,z1,1,1},{x1,y1,z1,0,1},{x1,y2,z1,0,0}},
				{{x2,y2,z1,1,0},{x2,y1,z1,1,1},{x1,y2,z1,0,0}},
				{{x1,y1,z2,0,1},{x2,y1,z2,1,1},{x1,y2,z2,0,0}},
				{{x2,y1,z2,1,1},{x2,y2,z2,1,0},{x1,y2,z2,0,0}},
			};
			d3ddev->CreateVertexBuffer(sizeof(v),0,Vtx3::FVF,D3DPOOL_DEFAULT,&BlockDrawUtil::vertexbuffer[i],nullptr);
			BlockDrawUtil::vertexbuffer[i]->Lock(0,0,(void**)&pvd,0);
			memcpy(pvd,v,sizeof(v));
			BlockDrawUtil::vertexbuffer[i]->Unlock();
			for(ull k(0);k<6;++k) SetTexture(BlockDrawUtil::texture[i][k],blktexid[i][k]);
			continue;
		}
		if(i==block::leaf)
		{
			float x1=line[i].min.x,x2=line[i].max.x,y1=line[i].min.y,y2=line[i].max.y,z1=line[i].min.z,z2=line[i].max.z;
			Vtx3::Vertex v[][3]=
			{
				{{x1,y1,z1,0,1},{x1,y2,z1,0,0},{x1,y1,z2,1,1}},
				{{x1,y1,z1,0,1},{x1,y1,z2,1,1},{x1,y2,z1,0,0}},

				{{x1,y1,z2,1,1},{x1,y2,z1,0,0},{x1,y2,z2,1,0}},
				{{x1,y1,z2,1,1},{x1,y2,z2,1,0},{x1,y2,z1,0,0}},

				{{x2,y1,z2,1,1},{x2,y2,z1,0,0},{x2,y1,z1,0,1}},
				{{x2,y1,z2,1,1},{x2,y1,z1,0,1},{x2,y2,z1,0,0}},

				{{x2,y2,z2,1,0},{x2,y2,z1,0,0},{x2,y1,z2,1,1}},
				{{x2,y2,z2,1,0},{x2,y1,z2,1,1},{x2,y2,z1,0,0}},

				{{x1,y1,z1,0,1},{x1,y1,z2,0,0},{x2,y1,z1,1,1}},
				{{x1,y1,z1,0,1},{x2,y1,z1,1,1},{x1,y1,z2,0,0}},

				{{x2,y1,z1,1,1},{x1,y1,z2,0,0},{x2,y1,z2,1,0}},
				{{x2,y1,z1,1,1},{x2,y1,z2,1,0},{x1,y1,z2,0,0}},

				{{x2,y2,z1,1,1},{x1,y2,z2,0,0},{x1,y2,z1,0,1}},
				{{x2,y2,z1,1,1},{x1,y2,z1,0,1},{x1,y2,z2,0,0}},

				{{x2,y2,z2,1,0},{x1,y2,z2,0,0},{x2,y2,z1,1,1}},
				{{x2,y2,z2,1,0},{x2,y2,z1,1,1},{x1,y2,z2,0,0}},

				{{x2,y1,z1,1,1},{x1,y2,z1,0,0},{x1,y1,z1,0,1}},
				{{x2,y1,z1,1,1},{x1,y1,z1,0,1},{x1,y2,z1,0,0}},

				{{x2,y2,z1,1,0},{x1,y2,z1,0,0},{x2,y1,z1,1,1}},
				{{x2,y2,z1,1,0},{x2,y1,z1,1,1},{x1,y2,z1,0,0}},

				{{x1,y1,z2,0,1},{x1,y2,z2,0,0},{x2,y1,z2,1,1}},
				{{x1,y1,z2,0,1},{x2,y1,z2,1,1},{x1,y2,z2,0,0}},

				{{x2,y1,z2,1,1},{x1,y2,z2,0,0},{x2,y2,z2,1,0}},
				{{x2,y1,z2,1,1},{x2,y2,z2,1,0},{x1,y2,z2,0,0}},
			};
			d3ddev->CreateVertexBuffer(sizeof(v),0,Vtx3::FVF,D3DPOOL_DEFAULT,&BlockDrawUtil::vertexbuffer[i],nullptr);
			BlockDrawUtil::vertexbuffer[i]->Lock(0,0,(void**)&pvd,0);
			memcpy(pvd,v,sizeof(v));
			BlockDrawUtil::vertexbuffer[i]->Unlock();
			for(ull k(0);k<6;++k) SetTexture(BlockDrawUtil::texture[i][k],blktexid[i][k]);
			continue;
		}
		float x1=line[i].min.x,x2=line[i].max.x,y1=line[i].min.y,y2=line[i].max.y,z1=line[i].min.z,z2=line[i].max.z;
		Vtx3::Vertex v[][4]=
		{
			{{x1,y1,z1,0,1},{x1,y1,z2,1,1},{x1,y2,z1,0,0},{x1,y2,z2,1,0}},
			{{x2,y1,z1,0,1},{x2,y2,z1,0,0},{x2,y1,z2,1,1},{x2,y2,z2,1,0}},
			{{x1,y1,z1,0,1},{x2,y1,z1,1,1},{x1,y1,z2,0,0},{x2,y1,z2,1,0}},
			{{x1,y2,z1,0,1},{x1,y2,z2,0,0},{x2,y2,z1,1,1},{x2,y2,z2,1,0}},
			{{x1,y1,z1,0,1},{x1,y2,z1,0,0},{x2,y1,z1,1,1},{x2,y2,z1,1,0}},
			{{x1,y1,z2,0,1},{x2,y1,z2,1,1},{x1,y2,z2,0,0},{x2,y2,z2,1,0}},
		};
		d3ddev->CreateVertexBuffer(sizeof(v),0,Vtx3::FVF,D3DPOOL_DEFAULT,&BlockDrawUtil::vertexbuffer[i],nullptr);
		BlockDrawUtil::vertexbuffer[i]->Lock(0,0,(void**)&pvd,0);
		memcpy(pvd,v,sizeof(v));
		BlockDrawUtil::vertexbuffer[i]->Unlock();
		for(ull k(0);k<6;++k) SetTexture(BlockDrawUtil::texture[i][k],blktexid[i][k]);
	}
	++downnum;
}
BlockIniterDeler::~BlockIniterDeler()
{
	--downnum;
	if(downnum) return;
	for(ull i(0);i<BLOCKNUM;++i)
	{
		sfrls(BlockDrawUtil::vertexbuffer[i]);
		for(ull j(0);j<6;++j) sfrls(BlockDrawUtil::texture[i][j]);
	}
	sfrls(shader);
	sfrls(consttable);
}


Block::Block()
{
	tp=block::air;
	drc=direction::py0|direction::ax0;
	pos=fvct(0,0,0);
	vis=0x3f;
//	for(ull k(0);k<6;++k) skylight[k]=15,blklight[k]=15;
}
Block::Block(blocktype tp,unsigned drc,fvct pos,rndsps vis
			 /*,unsigned l0,unsigned l1,unsigned l2,unsigned l3,unsigned l4,unsigned l5
			 ,unsigned l6,unsigned l7,unsigned l8,unsigned l9,unsigned la,unsigned lb*/) : tp(tp),drc(drc),pos(pos),vis(vis)
{/*
	skylight[0]=l0;
	skylight[1]=l1;
	skylight[2]=l2;
	skylight[3]=l3;
	skylight[4]=l4;
	skylight[5]=l5;
	blklight[0]=l6;
	blklight[1]=l7;
	blklight[2]=l8;
	blklight[3]=l9;
	blklight[4]=la;
	blklight[5]=lb;*/
}