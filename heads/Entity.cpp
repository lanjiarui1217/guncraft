#pragma warning(push)
#pragma warning(disable:4005)

#include "Entity.h"
#include <cmath>
#include "block.h"
#include "render.h"
#include "World.h"
#include "_math.h"
#pragma warning(pop)

bool Entity::entitymove(fvct move,Box coll,float d)
{
	if(len((p+move)-p)<eps) return true;
	Ray r(p,p+move,d,len(move)/d);
	for(;~r;++r) if((!(std::isnan(((fvct)(r)).x)||std::isnan(((fvct)(r)).y)||std::isnan(((fvct)(r)).z))&&(!wrld.bxbl(coll+r)))) p=r; else break;
	return !(~r);
}
bool Entity::entitymove(fvct move,float d)
{
	if(len((p+move)-p)<eps) return true;
	Ray r(p,p+move,d,len(move)/d);
	for(;~r;++r) if((!(std::isnan(((fvct)(r)).x)||std::isnan(((fvct)(r)).y)||std::isnan(((fvct)(r)).z))&&(!wrld.bxbl(r)))) p=r; else break;
	return !(~r);
}


// 单色阶段：实体画成小立方体
static void drawentcube(fvct p,float s,RGB c)
{
	float x1=p.x-s,x2=p.x+s,y1=p.y-s,y2=p.y+s,z1=p.z-s,z2=p.z+s;
	const fvct f[6][4]={
		{{x1,y1,z1},{x1,y1,z2},{x1,y2,z2},{x1,y2,z1}},
		{{x2,y1,z1},{x2,y2,z1},{x2,y2,z2},{x2,y1,z2}},
		{{x1,y1,z1},{x2,y1,z1},{x2,y1,z2},{x1,y1,z2}},
		{{x1,y2,z1},{x1,y2,z2},{x2,y2,z2},{x2,y2,z1}},
		{{x1,y1,z1},{x1,y2,z1},{x2,y2,z1},{x2,y1,z1}},
		{{x1,y1,z2},{x2,y1,z2},{x2,y2,z2},{x1,y2,z2}},
	};
	for(int i(0);i<6;++i)
	{
		rendertriangle(f[i][0],f[i][1],f[i][2],c,c,c);
		rendertriangle(f[i][0],f[i][2],f[i][3],c,c,c);
	}
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
	p=((Player*)pp)->cmp;
	v=((Player*)pp)->v+(((Player*)pp)->lkt-((Player*)pp)->cmp)/len(((Player*)pp)->lkt-((Player*)pp)->cmp)*sqrt(-30*2*g);
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
	drawentcube(p,0.1f,{30,180,60});
}

enderpearl::~enderpearl()
{}


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
	if(tp==item::rawiron) drawentcube(p,0.12f,{200,120,60});
	else drawentcube(p,0.15f,{255,255,255});
}

drop::~drop()
{}

actTNT::actTNT(fvct ps)
{
	p=ps;
	float ang=rand()*2.0f*3.14159265358979323846f/RAND_MAX;
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
	p=ps;v=v_;
}

void AmmoM1943::upd()
{
	if(!entitymove(v/tps)) exist=0,Blast(realpos(),2.0f,wrld,0);
	if(v.y<-1000) exist=0;
}

void AmmoM1943::render()
{
	drawentcube(p,0.06f,{240,220,120});
}

AmmoM1943::~AmmoM1943()
{}

Grenade::Grenade(fvct ps,fvct v_)
{
	p=ps;v=v_;
}

void Grenade::upd()
{
	v+=fvct(0,-9.8f,0)/tps;
	if(!entitymove(v/tps)) exist=0,Blast(realpos(),3.4f,wrld,0);
	if(v.y<-1000) exist=0;
}

void Grenade::render()
{
	drawentcube(p,0.08f,{60,90,60});
}

Grenade::~Grenade()
{}

GuidedMissile::GuidedMissile(fvct ps,fvct v_)
{
	p=ps;v=v_;
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
	drawentcube(p,0.15f,{200,200,200});
}

GuidedMissile::~GuidedMissile()
{
	pmissile=nullptr;
}

GuidedMissile* pmissile=nullptr;