#pragma warning(push)
#pragma warning(disable:4005)

#include "Entity.h"
#include "block.h"
#include "World.h"
#include "_math.h"
#pragma warning(pop)

bool Entity::entitymove(fvct move,Box coll,float d)
{
	if(len((p+move)-p)<eps) return true;
	Ray r(p,p+move,d,len(move)/d);
	for(;~r;++r) if((!(isnan(((fvct)(r)).x)||isnan(((fvct)(r)).y)||isnan(((fvct)(r)).z))&&(!wrld.bxbl(coll+r)))) p=r; else break;
	return !(~r);
}
bool Entity::entitymove(fvct move,float d)
{
	if(len((p+move)-p)<eps) return true;
	Ray r(p,p+move,d,len(move)/d);
	for(;~r;++r) if((!(isnan(((fvct)(r)).x)||isnan(((fvct)(r)).y)||isnan(((fvct)(r)).z))&&(!wrld.bxbl(r)))) p=r; else break;
	return !(~r);
}

std::vector<Entity*> ve;

fallsand::fallsand(fvct _p)
{
	p=_p;v=fvct(0,0,0);
	coll=Box();
	exist=1;
}

void fallsand::upd()
{
	if(!entitymove(v/tps,coll)) exist=0;
	if(v.y<-1000) exist=0;
	v+=fvct(0,-9.8f,0)/tps;
}

void fallsand::render()
{
	Block b=wrld.getbl(floor(realpos()));
	b.tp=block::sand;
	b.drc=0;
	b.pos=p;
	b.vis=0x3f;
	BlockDrawUtil::draw(b,0x3f);
}

fallsand::~fallsand()
{
	wrld.setbl(floor(p),block::sand);
}
#include "Player.h"
enderpearl::enderpearl(void*pp):pp(pp)
{
	float w=1.0f/4,h=1.0f/4;
	VOID* pVoid;
	Vtx3::Vertex vv[][3]=
	{
		{{0,0,0,0,1},{0,h,0,0,0},{w,h,0,1,0}},
		{{0,0,0,0,1},{w,h,0,1,0},{w,0,0,1,1}},
	};
	d3ddev->CreateVertexBuffer(sizeof(vv),0,Vtx3::FVF,D3DPOOL_DEFAULT,&vb,nullptr);
	vb->Lock(0,0,(void**)&pVoid,0);
	memcpy(pVoid,vv,sizeof(vv));
	vb->Unlock();
	p=((Player*)pp)->cmp;
	v=((Player*)pp)->v+(((Player*)pp)->lkt-((Player*)pp)->cmp)/len(((Player*)pp)->cmp-((Player*)pp)->lkt)*sqrt(-30*2*g);
	coll=Box(fvct(0,0,0),fvct(0,0,0));
	exist=1;
}

void enderpearl::upd()
{
	if(!entitymove(v/tps,((Player*)pp)->coll)) exist=0,((Player*)pp)->p=p;
	if(v.y<-1000) exist=0;
	v+=fvct(0,g,0)/tps;
}
void enderpearl::render()
{
	unsigned vl;
//	Block b=wrld.getbl(vct3<ll>((ll)floor(p.x),(ll)floor(p.y),(ll)floor(p.z)));
	vl=glight(floor(p),rand()%6)*16;
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
	d3ddev->SetTexture(0,ItemDrawUtil::texture[item::ender_pearl]);
	d3ddev->SetStreamSource(0,vb,0,sizeof(Vtx3::Vertex));
	d3ddev->SetTransform(D3DTS_WORLD,&matw);
	d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST,0,2u);
	d3ddev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
}

enderpearl::~enderpearl()
{
	if(vb!=nullptr) vb->Release();
}


drop::drop(fvct ps,itemtype tp):tp(tp)
{
	p=ps;v=fvct(0,0,0);
	angle=0;
	coll=Box(fvct(-0.1f,-0.1f,-0.1f),fvct(0.1f,0.1f,0.1f));
	exist=1;
}

void drop::upd()
{
	p+=v/tps;
	if(p.y<-1000) exist=0;
	v+=fvct(0,-3.0f,0)/tps;
	angle+=0.1f/tps;
	if(wrld.bxbl(Box(fvct(-0.1f,-1.0f,-0.1f),fvct(0.1f,0,0.1f))+p))
	{
		float l=0,r=1.0f;
		if(wrld.bxbl(p)) v=fvct(0,1,0);
		else
		{
			while(r-l>0.01)
			{
				float mid=(l+r)/2;
				if(wrld.bxbl(Box(fvct(-0.1f,-mid,-0.1f),fvct(0.1f,0,0.1f))+p)) r=mid;
				else l=mid;
			}
			if(l>=0.05f) v+=fvct(0,0.75f/l,0)/tps;
		}
	}
	if(!wrld.bxbl(p))
	{
		ull pos=10;
		for(ull i(9);~i;--i) if(tspl.itemlist[i].num==0) pos=i;
		for(ull i(0);i<10;++i) if(tspl.itemlist[i].tp==tp) pos=i;
		if(pos!=10)
		{
			if(tspl.cmp.y<=p.y)
			{
				float dis=len(p-tspl.cmp);
				if(dis<0.5) exist=0,tspl.itemlist[pos].tp=tp,++tspl.itemlist[pos].num;
				else if(dis<1.7f) v+=3*(tspl.cmp-p)/dis/dis/tps;
			}
			else if(tspl.p.y<=p.y)
			{
				fvct plp(tspl.p);
				plp.y=p.y;
				float dis=len(p-plp);
				if(dis<0.5) exist=0,tspl.itemlist[pos].tp=tp,++tspl.itemlist[pos].num;
				else if(dis<1.7f) v+=3*(plp-p)/dis/dis/tps;
			}
			else
			{
				float dis=len(p-tspl.p);
				if(dis<0.5) exist=0,tspl.itemlist[pos].tp=tp,++tspl.itemlist[pos].num;
				else if(dis<1.7f) v+=3*(tspl.p-p)/dis/dis/tps;
			}
		}
	}
}


void drop::render()
{
	if(tp==item::rawiron)
	{
		fmat mt;
		fmat mt1,msc,mro,mt2;
		D3DXMatrixTranslation(&mt1,-0.5f,0.0f,-0.5f);
		D3DXMatrixScaling(&msc,0.3f,0.3f,0.3f);
		D3DXMatrixRotationY(&mro,angle);
		D3DXMatrixTranslation(&mt2,p.x,p.y,p.z);
		mt=mt1*msc*mro*mt2;
		d3ddev->SetTransform(D3DTS_WORLD,&mt);
		d3ddev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
		d3ddev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		d3ddev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
		d3ddev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,0);
		d3ddev->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_TFACTOR);
		d3ddev->SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);
		d3ddev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_MODULATE);
		Block b=wrld.getbl(floor(p));
		unsigned vl=glight(floor(p),0)*16;
		for(ull face(1);face<6;++face) vl=max(vl,glight(floor(p),face)*16);
		d3ddev->SetRenderState(D3DRS_TEXTUREFACTOR,D3DCOLOR_ARGB(255,vl,vl,vl));
		d3ddev->SetTexture(0,ItemDrawUtil::texture[item::rawiron]);
		for(ull i(0);i<6;++i)
		{
			Vtx3::Vertex vtx[][3]=
			{
				{{0.5f,0,0,1,1},{0.5f,0,1,0,1},{0.5f,1,1,0,0}},
				{{0.5f,0,0,1,1},{0.5f,1,0,1,0},{0.5f,1,1,0,0}},
				{{0.5f,0,0,1,1},{0.5f,1,1,0,0},{0.5f,0,1,0,1}},
				{{0.5f,0,0,1,1},{0.5f,1,1,0,0},{0.5f,1,0,1,0}}
			};
			d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,4,vtx,sizeof(Vtx3::Vertex));
		}
		d3ddev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
		return ;
	}
	fmat mt;
	fmat mt1,msc,mro,mt2;
	D3DXMatrixTranslation(&mt1,-0.5f,0,-0.5f);
	D3DXMatrixScaling(&msc,0.3f,0.3f,0.3f);
	D3DXMatrixRotationY(&mro,angle);
	D3DXMatrixTranslation(&mt2,p.x,p.y,p.z);
	mt=mt1*msc*mro*mt2;
	Block bins=wrld.getbl(floor(p));
	Block b=bins;
	b.tp=toblock[tp];
	b.drc=0;
	b.pos=p;
	b.vis=0x3f;
	BlockDrawUtil::draw(b,mt);
}

drop::~drop()
{}

actTNT::actTNT(fvct ps)
{
	p=ps;
	float ang=rand()*2.0f*D3DX_PI/RAND_MAX;
	v=fvct(sin(ang)*0.4f,0.4f,cos(ang)*0.4f);
	coll=Box();
	time=4.0f;
}
#include "blast.h"
void actTNT::upd()
{
	fvct dx=v/tps;
	if(!entitymove(fvct(dx.x,0,0),coll)) v.x=0;
	if(!entitymove(fvct(0,dx.y,0),coll)) v.y=0;
	if(!entitymove(fvct(0,0,dx.z),coll)) v.z=0;
	v+=fvct(0,-9.8f,0)/tps;
	time-=1.0f/tps;
	v*=0.95f;
	if(time<=0)
	{
		Blast(realpos(),4.0f,wrld,1);
		exist=0;
	}
}

void actTNT::render()
{
	BlockDrawUtil::draw(Block(block::TNT,0,p),0x3f);
}

actTNT::~actTNT()
{}

AmmoM1943::AmmoM1943(fvct ps,fvct v_)
{
	float w=1.0f/4,h=1.0f/4;
	VOID* pVoid;
	Vtx3::Vertex vv[][3]=
	{
		{{0,0,0,0,1},{0,h,0,0,0},{w,h,0,1,0}},
		{{0,0,0,0,1},{w,h,0,1,0},{w,0,0,1,1}},
	};
	d3ddev->CreateVertexBuffer(sizeof(vv),0,Vtx3::FVF,D3DPOOL_DEFAULT,&vb,nullptr);
	vb->Lock(0,0,(void**)&pVoid,0);
	memcpy(pVoid,vv,sizeof(vv));
	vb->Unlock();
	p=ps;v=v_;
	D3DXCreateTextureFromFile(d3ddev,L"bmps/oth/AmmoM1943.bmp",&tex);
}

void AmmoM1943::upd()
{
	if(!entitymove(v/tps)) exist=0,Blast(realpos(),2.0f,wrld,0);
	if(v.y<-1000) exist=0;
}

void AmmoM1943::render()
{
	unsigned vl=15*16;
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
	d3ddev->SetTexture(0,tex);
	d3ddev->SetStreamSource(0,vb,0,sizeof(Vtx3::Vertex));
	d3ddev->SetTransform(D3DTS_WORLD,&matw);
	d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST,0,2u);
	d3ddev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
}

AmmoM1943::~AmmoM1943()
{
	if(vb!=nullptr) vb->Release();
	if(tex!=nullptr) tex->Release();
}

Grenade::Grenade(fvct ps,fvct v_)
{
	float w=1.0f/2,h=1.0f/2;
	VOID* pVoid;
	Vtx3::Vertex vv[][3]=
	{
		{{0,0,0,0,1},{0,h,0,0,0},{w,h,0,1,0}},
		{{0,0,0,0,1},{w,h,0,1,0},{w,0,0,1,1}},
	};
	d3ddev->CreateVertexBuffer(sizeof(vv),0,Vtx3::FVF,D3DPOOL_DEFAULT,&vb,nullptr);
	vb->Lock(0,0,(void**)&pVoid,0);
	memcpy(pVoid,vv,sizeof(vv));
	vb->Unlock();
	p=ps;v=v_;
	D3DXCreateTextureFromFile(d3ddev,L"bmps/items/grenade.bmp",&tex);
}

void Grenade::upd()
{
	v+=fvct(0,-9.8f,0)/tps;
	if(!entitymove(v/tps)) exist=0,Blast(realpos(),3.4f,wrld,0);
	if(v.y<-1000) exist=0;
}

void Grenade::render()
{
	unsigned vl=15*16;
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
	d3ddev->SetTexture(0,tex);
	d3ddev->SetStreamSource(0,vb,0,sizeof(Vtx3::Vertex));
	d3ddev->SetTransform(D3DTS_WORLD,&matw);
	d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST,0,2u);
	d3ddev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
}

Grenade::~Grenade()
{
	if(vb!=nullptr) vb->Release();
	if(tex!=nullptr) tex->Release();
}

GuidedMissile::GuidedMissile(fvct ps,fvct v_)
{
	//float w=1.0f/2,h=1.0f/2;
	//VOID* pVoid;
	//Vtx3::Vertex vv[][3]=
	//{
	//	{{0,0,0,0,1},{0,h,0,0,0},{w,h,0,1,0}},
	//	{{0,0,0,0,1},{w,h,0,1,0},{w,0,0,1,1}},
	//};
	//d3ddev->CreateVertexBuffer(sizeof(vv),0,Vtx3::FVF,D3DPOOL_DEFAULT,&vb,nullptr);
	//vb->Lock(0,0,(void**)&pVoid,0);
	//memcpy(pVoid,vv,sizeof(vv));
	//vb->Unlock();
	p=ps;v=v_;
	//D3DXCreateTextureFromFile(d3ddev,L"bmps/items/GuidedMissile.bmp",&tex);
	assert(pmissile==nullptr);
	pmissile=this;
}

void GuidedMissile::upd()
{
	wrld.update();
	//v+=fvct(0,-9.8f,0)/tps;
	fvct tmp=(tspl.lkt-tspl.cmp)/len(tspl.lkt-tspl.cmp);
	v=tmp*200/tps;
	if(!entitymove(v/tps)) exist=0,Blast(realpos(),3.4f,wrld,0);
	if(v.y<-1000) exist=0;
	tspl.cmp=p;
}

void GuidedMissile::render()
{
	//unsigned vl=15*16;
	//d3ddev->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
	//d3ddev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	//d3ddev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	//d3ddev->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,0);
	//d3ddev->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_TFACTOR);
	//d3ddev->SetRenderState(D3DRS_TEXTUREFACTOR,D3DCOLOR_ARGB(255,vl,vl,vl));
	//d3ddev->SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);
	//d3ddev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_MODULATE);
	//fmat matt,matr,matw;
	//D3DXMatrixTranslation(&matt,p.x,p.y,p.z);
	//fmat r1,r2;
	//fvct up(1.0f,0.0f,0.0f),wup(0.0f,1.0f,0.0f),lk=p-tspl.cmp,c;
	//lk/=len(lk);
	//D3DXMatrixRotationY(&r1,atan2f(lk.x,lk.z));
	//up.x=r1(0,0),up.y=r1(1,0),up.z=r1(2,0);
	//float ax=atan2f(lk.y,sqrt(lk.x*lk.x+lk.z*lk.z));
	//D3DXVec3Cross(&c,&wup,&up); if(c.z<0) ax=-ax;
	//D3DXMatrixRotationAxis(&r2,&up,ax);
	//D3DXMatrixMultiply(&matr,&r2,&r1);
	//D3DXMatrixMultiply(&matw,&matr,&matt);
	//d3ddev->SetTexture(0,tex);
	//d3ddev->SetStreamSource(0,vb,0,sizeof(Vtx3::Vertex));
	//d3ddev->SetTransform(D3DTS_WORLD,&matw);
	//d3ddev->DrawPrimitive(D3DPT_TRIANGLELIST,0,2u);
	//d3ddev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
}

GuidedMissile::~GuidedMissile()
{
	//if(vb!=nullptr) vb->Release();
	//if(tex!=nullptr) tex->Release();
	pmissile=nullptr;
}

GuidedMissile* pmissile=nullptr;