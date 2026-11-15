#include "particle.h"
#include "Player.h"
#include "World.h"

Texture d3dParticle::texture[PARTICLENUM];
VtxBuf d3dParticle::vertexbuffer[PARTICLENUM];
ull ParticleIniterDeler::downnum;
std::vector<Particle> vptc;
void d3dParticle::draw(particle tp,fvct p)
{
	d3ddev->SetPixelShader(NULL);
	unsigned vl;
	Block b=wrld.getbl(vct3<ll>((ll)floor(p.x),(ll)floor(p.y),(ll)floor(p.z)));
	//unsigned char ht=wrld.getht(vct3<ll>((ll)floor(p.x),(ll)floor(p.y),(ll)floor(p.z)));
	if(p.y>=WorldHeight||p.y<0) vl=nowskylight*16;
	else
	{
		vl=glight(floor(p),0)*16;
		for(ull face(1);face<6;++face) vl=max(vl,glight(floor(p),face)*16);
	}
	d3ddev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
	d3ddev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	d3ddev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	d3ddev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,0);
	d3ddev->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_TFACTOR);
	d3ddev->SetRenderState(D3DRS_TEXTUREFACTOR,D3DCOLOR_ARGB(255,vl,vl,vl));
	d3ddev->SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);
	d3ddev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_MODULATE);
	fmat matt,matr,matw;
	D3DXMatrixTranslation(&matt,p.x,p.y,p.z);
	fmat r1,r2;
	fvct up(1.0f,0.0f,0.0f),wup(0.0f,1.0f,0.0f),lk=p-tspl.cmp,c;
	lk/=len(lk);
	D3DXMatrixRotationY(&r1,atan2f(lk.x,lk.z));
	up.x=r1(0,0),up.y=r1(1,0),up.z=r1(2,0);
	float ax=atan2f(lk.y,sqrt(lk.x*lk.x+lk.z*lk.z));
	D3DXVec3Cross(&c,&wup,&up); if(c.z<0) ax=-ax;
	D3DXMatrixRotationAxis(&r2,&up,ax);
	D3DXMatrixMultiply(&matr,&r2,&r1);
	D3DXMatrixMultiply(&matw,&matr,&matt);
//	d3ddev->SetTexture(0,texture[tp]);
	d3ddev->SetStreamSource(0,vertexbuffer[tp],0,sizeof(Vtx3::Vertex));
	d3ddev->SetTransform(D3DTS_WORLD,&matw);
	d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST,0,2u);
	d3ddev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
}

bool ParticleIniterDeler::down()
{
	return downnum>0;
}

ParticleIniterDeler::ParticleIniterDeler()
{
	if(downnum)
	{
		++downnum;
		return;
	}
	++downnum;
	for(particle tp(0);tp<PARTICLENUM;++tp)
	{
		float w=ptcw[tp],h=ptch[tp];static VOID* pVoid;
		Vtx3::Vertex v[][3]=
		{
			{{0,0,0,0,1},{0,h,0,0,0},{w,h,0,1,0}},
			{{0,0,0,0,1},{w,h,0,1,0},{w,0,0,1,1}},
		};
		d3ddev->CreateVertexBuffer(sizeof(v),0,Vtx3::FVF,D3DPOOL_DEFAULT,&d3dParticle::vertexbuffer[tp],nullptr);
		d3dParticle::vertexbuffer[tp]->Lock(0,0,(void**)&pVoid,0);
		memcpy(pVoid,v,sizeof(v));
		d3dParticle::vertexbuffer[tp]->Unlock();
		SetTexture(d3dParticle::texture[tp],ptctexid[tp]);
	}
}

ParticleIniterDeler::~ParticleIniterDeler()
{
	--downnum;
	if(downnum) return;
	for(ull i(0);i<PARTICLENUM;++i)
	{
		if(d3dParticle::vertexbuffer[i]!=nullptr)
		{
			d3dParticle::vertexbuffer[i]->Release();
			d3dParticle::texture[i]->Release();
		}
	}
}

bool Particle::exist()
{
	switch(tp)
	{
	case particletype::block:
		return (!wrld.bxbl(p))&&p.y>=-1000.0f;
	case particletype::rain:
		return (!wrld.bxbl(p))&&p.y>=-1000.0f;
	}
}
