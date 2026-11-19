#include "Player.h"
#include "World.h"
#include "block.h"
#include "keyboard.h"
#include <cmath>
#define D3DXToRadian(a) ((float)((a)*3.14159265358979323846/180.0))
#define D3DX_PI 3.14159265358979323846f
static inline bool GetKeyState(int k)
{
	// k>=32 && k<=126 是 ASCII，不能直接当 KEY_* 码用
	if(k<32||k>126) return keyboard.keys[k];
	int low=k;
	switch(k)
	{
	case'W': case'w': low=KEY_W;break;
	case'S': case's': low=KEY_S;break;
	case'A': case'a': low=KEY_A;break;
	case'D': case'd': low=KEY_D;break;
	case'F': case'f': low=KEY_F;break;
	case'Q': case'q': low=KEY_Q;break;
	case'E': case'e': low=KEY_E;break;
	case'R': case'r': low=KEY_R;break;
	case' ': low=KEY_SPACE;break;
	case'\x09': low=KEY_TAB;break;
	case'\x10': low=KEY_LEFTSHIFT;break;
	default:
		if(k>='0'&&k<='9') { low=k-'0'+KEY_1; break; }
		return false;
	}
	return keyboard.keys[low];
}

std::vector<Player> vp;
ull thisind;

Player::Player()
{
	lstenderpearltime=0;
	lstshoottime=0;
	lstgrenadetime=0;
	chooseind=0;
	for(ull i(0);i<10;++i) itemlist[i].tp=ITEMNUM,itemlist[i].num=0;
	exist=1;v=fvct(0,0,0);
	coll=Box(fvct(-1.0f/4,0,-1.0f/4),fvct(1.0f/4,31.0f/16,1.0f/4));
	memset(angle,0,sizeof(angle));
	angle[2]=90;
}

Player::Player(fvct pos)
{
	lstenderpearltime=0;
	lstshoottime=0;
	lstgrenadetime=0;
	p=pos;
	chooseind=0;
	for(ull i(0);i<10;++i) itemlist[i].tp=ITEMNUM,itemlist[i].num=0;
	itemlist[0].tp=item::glowstone;
	itemlist[0].num=10000;
	itemlist[1].tp=item::stone;
	itemlist[1].num=10000;
	itemlist[2].tp=item::falcula;
	itemlist[2].num=10000;
	itemlist[3].tp=item::ender_pearl;
	itemlist[3].num=10000;
	itemlist[4].tp=item::woodplank;
	itemlist[4].num=10000;
	itemlist[5].tp=item::glass;
	itemlist[5].num=10000;
	itemlist[6].tp=item::TNT;
	itemlist[6].num=10000;
	itemlist[7].tp=item::AK47;
	itemlist[7].num=1;
	itemlist[8].tp=item::grenade;
	itemlist[8].num=10000;
	itemlist[9].tp=item::AK47;
	itemlist[9].num=10000;
	exist=1;v=fvct(0,0,0);
	coll=Box(fvct(-1.0f/4,0,-1.0f/4),fvct(1.0f/4,31.0f/16,1.0f/4));
	memset(angle,0,sizeof(angle));
	angle[2]=90;
	Chunk& c=*wrld.getchunk(vct3<ll>((ll)pos.x,0,(ll)pos.z));
	p.x+=0.5f,p.z+=0.5f;p.y=0.0f;
	vct3<ull> uv=getposChunk(vct3<ll>((ll)pos.x,0,(ll)pos.z));
	ull i=uv.x,k=uv.z;
	for(ull j(WorldHeight-1);j>=2;--j)
	{
		if(c.b[i][j][k].tp==block::air&&c.b[i][j-1][k].tp==block::air&&c.b[i][j-2][k].tp!=block::air)
		{
			p.y=(float)(j-1);
			break;
		}
	}
	if(p.y<0.1f) p.y=(float)WorldHeight;
}

Player::~Player()
{}

void Player::upd() {}

void Player::render() {}
extern float maxatjumph;
void Player::updatepos(float dfv,float dt)
{
	auto onland=[&]()->bool { return wrld.bxbl(Box(fvct(-1.0f/4,-0.007f,-1.0f/4),fvct(1.0f/4,0,1.0f/4))+p); };
	const float g=-16;
	if(GetKeyState(KEY_TAB)) dfv*=14;
	float dx=dfv*dt;
	float rad=D3DXToRadian(angle[1]),sina=sin(rad),cosa=cos(rad);
	fvct to(0,0,0);
	if(GetKeyState('W')) to+=fvct(cosa,0,sina);
	if(GetKeyState('S')) to+=fvct(-cosa,0,-sina);
	if(GetKeyState('A')) to+=fvct(sina,0,-cosa);
	if(GetKeyState('D')) to+=fvct(-sina,0,cosa);
	if(len(to)>=eps)
	{
		to=to/len(to)*dx;
		auto move2=[&](fvct move)->void
		{
			float d=1.0f/512;
			if(len((p+move)-p)<eps) return ;
			Ray r(p,p+move,d,len(move)/d);
			for(;~r;++r)
			{
				if(std::isnan(((fvct)(r)).x)||std::isnan(((fvct)(r)).y)||std::isnan(((fvct)(r)).z)) break;
				if(!wrld.bxbl(coll+r)) p=r;
				else if(onland())
				{
					float jh=-1.0f;
					fvct np=r;
					for(float k(0.0f);k<=maxatjumph;k+=d)
					{
						if(!wrld.bxbl(coll+(np+fvct(0,k,0))))
						{
							jh=k;
							break;
						}
					}
					if(jh<0) break;
					r=Ray(r.frm+fvct(0,jh,0),r.frm+r.e+fvct(0,jh,0),d,len(move)/d,r.now);
					p=r;
				}
				else break;
			}
		};
		//float j1=move2(fvct(to.x,0,0)),j2=move2(fvct(0,0,to.z));
		//v+=fvct(0,sqrt((-max(j1,j2))*2*g)*dt,0);
		move2(fvct(to.x,0,0)),move2(fvct(0,0,to.z));
	}
	//if(GetKeyState(' ')) entitymove(fvct(0,dx,0),coll);
	if((GetKeyState(' '))&&onland()) p+=fvct(0,0.01f,0),v+=fvct(0,sqrt(((GetKeyState(KEY_TAB))?-256:-1.2522)*2*g),0);
	if(keyboard.keys[KEY_LEFTSHIFT]||keyboard.keys[KEY_RIGHTSHIFT]) entitymove(fvct(0,-dx,0),coll);
	//v*=0.9997f;
	//v.y-=9.8f*dt;
	fvct dv(0,0,0);
	dv.y+=g*dt;
	// 鞘翅
	if(GetKeyState('F'))
	{
		fvct vo=v;
		float voh=sqrt(vo.x*vo.x+vo.z*vo.z);
		float alp=-D3DXToRadian(angle[2]),bet=D3DXToRadian(angle[1]-90);
		v=fvct(vo.x,vo.y-0.4*(1-0.75*cos(alp)*cos(alp)),vo.z);
		if(v.y<0)
		{
			v=fvct(v.x+0.1*v.y*cos(alp)*cos(alp)*sin(bet),
				   (1-0.1*cos(alp)*cos(alp))*v.y,
				   v.z-0.1*v.y*cos(alp)*cos(alp)*cos(bet));
		}
		else
		{
			v=fvct(v.x-0.04*voh*sin(alp)*sin(bet),
				   v.y-0.128*voh*sin(alp),
				   v.z+0.04*voh*sin(alp)*cos(bet));
		}
		v=fvct(0.9*v.x-0.1*voh*sin(bet),
			   v.y,
			   0.9*v.z+0.1*voh*cos(bet));

		v=fvct(0.99*v.x,0.98*v.y,0.99*v.z);
	}
	v+=dv;
	fvct dx2=v*dt-dv*dt/2;
	if(!entitymove(fvct(dx2.x,0,0),coll)) v.x=0;
	if(!entitymove(fvct(0,dx2.y,0),coll)) v.y=0;
	if(!entitymove(fvct(0,0,dx2.z),coll)) v.z=0;
	if(len(v)<=eps) v=fvct(0,0,0);
	if(onland()) v.x*=0.9f,v.z*=0.9f;
	float dn=floor(p.y);
	if(v.y<=0.0f&&p.y-dn<0.01f) p.y=dn;
	updatelkt();
}

void Player::updatelkt()
{
	if(angle[1]>180) angle[1]-=360.0f;
	if(angle[1]<-180) angle[1]+=360.0f;
	if(angle[2]>=89.9) angle[2]=89.9f;
	if(angle[2]<=-89.9) angle[2]=-89.9f;
	cmp=p+fvct(0,15.0f/8,0);
	if(pmissile) cmp=pmissile->p;
	lkt.x=cmp.x+15*cos(D3DXToRadian(angle[1]))*cos(D3DXToRadian(angle[2]));
	lkt.z=cmp.z+15*sin(D3DXToRadian(angle[1]))*cos(D3DXToRadian(angle[2]));
	lkt.y=cmp.y+15*sin(D3DXToRadian(angle[2]));
}
