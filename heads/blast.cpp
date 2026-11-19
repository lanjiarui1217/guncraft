#include "blast.h"
#include <algorithm>
std::vector<vct3<ll>> v;
void Blast(fvct p,float power,World& w,bool expdrop)
{
	auto work=[&](fvct p,fvct e,float pw,World& w)
	{
		e/=len(e);
		for(float I((1.0f-0.3f+(0.6f*rand()/RAND_MAX))*power);I>=0;p+=e*0.3f)
		{
			if(w.bxbl(p))
			{
				I-=0.3f*boomhard[w.getbl(floor(p)).tp]+0.315f;
				if(I<=0) break;
				v.push_back(floor(p));
			}
			else I-=0.225f;
		}
	};
	v.resize(0);
	for(ll i(-8);i<=8;++i)
	{
		for(ll j(-8);j<=8;++j)
		{
			for(ll k(-8);k<=8;++k)
			{
				if(!(abs(i)==8||abs(j)==8||abs(k)==8)) continue;
				work(p,fvct((float)i,(float)j,(float)k),power,w);
			}
		}
	}
	std::sort(v.begin(),v.end());
	v.erase(std::unique(v.begin(),v.end()),v.end());
	const ll inf=0x7fffffffffffffff;
	ll xmn=inf,zmn=inf,xmx=-inf,zmx=-inf;
	for(const auto& v:v)
	{
		if(v.y<0||v.y>=WorldHeight) continue;
		Chunk& c=*wrld.getchunk(v);
		vct3<ull> uv=getposChunk(v);
		blocktype tmp=c.b[uv.x][uv.y][uv.z].tp;
		if(tmp==block::air) continue;
		c.b[uv.x][uv.y][uv.z].tp=block::air;
		//if(uv.y==c.height[uv.x][uv.z])
		//{
		//	for(;(unsigned char)(c.height[uv.x][uv.z]+1);--c.height[uv.x][uv.z])
		//	{
		//		if(c.b[uv.x][c.height[uv.x][uv.z]][uv.z].tp!=block::air) break;
		//	}
		//}
		wrld.vison(v);
		wrld.vison(v-vct3<ll>(1,0,0));
		wrld.vison(v+vct3<ll>(1,0,0));
		wrld.vison(v-vct3<ll>(0,1,0));
		wrld.vison(v+vct3<ll>(0,1,0));
		wrld.vison(v-vct3<ll>(0,0,1));
		wrld.vison(v+vct3<ll>(0,0,1));
		if(v.y<0||v.y>=WorldHeight) return;
		xmn=std::min(xmn,v.x);
		zmn=std::min(zmn,v.z);
		xmx=std::max(xmx,v.x);
		zmx=std::max(zmx,v.z);
		if(expdrop) ve.push_back(new drop(fvct((float)v.x,(float)v.y,(float)v.z)+fvct(0.5,0.5,0.5),blkdrop[tmp]));
	}
	wrld.genlight_round(vct3<ll>((xmn+xmx)/2,0,(zmn+zmx)/2),16u+(unsigned)std::max((xmx-xmn)/2,(zmx-zmn)/2));
	float sz=2*power;
	for(auto pe:ve)
	{
		float dis=len(pe->realpos()-p);
		if(dis<=sz&&dis>=1e-6)
		{
			float a;
			ull num=0,cnt=0;
			Box b=pe->coll+pe->p;
			float dx=(b.max.x-b.min.x);
			float dy=(b.max.x-b.min.x);
			float dz=(b.max.x-b.min.x);
			for(float x(b.min.x+(1-floor(2*dx+1)/(2*dx+1))/2);x<b.max.x;x+=dx/(2*dx+1))
			{
				for(float y(b.min.y);y<b.max.y;y+=dy/(2*dy+1))
				{
					for(float z(b.min.z+(1-floor(2*dz+1)/(2*dz+1))/2);z<b.max.z;z+=dz/(2*dz+1))
					{
						Ray r(p,fvct(x,y,z),len(p-fvct(x,y,z))/100,100,0);++num;++cnt;
						for(;~r;++r) if(w.bxbl(r)) { --cnt; break; }
					}
				}
			}
			a=cnt*1.0f/num;
			pe->v+=(pe->realpos()-p)/dis*(1-dis/sz)*a;
			fvct d=(pe->realpos()-p)/dis*(1-dis/sz)*a;
		}
	}
	for(auto& pl:vp)
	{
		auto pe=&pl;
		float dis=len(pe->realpos()-p);
		if(dis<=sz&&dis>=1e-6)
		{
			float a;
			ull num=0,cnt=0;
			Box b=pe->coll+pe->p;
			float dx=(b.max.x-b.min.x);
			float dy=(b.max.x-b.min.x);
			float dz=(b.max.x-b.min.x);
			for(float x(b.min.x+(1-floor(2*dx+1)/(2*dx+1))/2);x<b.max.x;x+=dx/(2*dx+1))
			{
				for(float y(b.min.y);y<b.max.y;y+=dy/(2*dy+1))
				{
					for(float z(b.min.z+(1-floor(2*dz+1)/(2*dz+1))/2);z<b.max.z;z+=dz/(2*dz+1))
					{
						Ray r(p,fvct(x,y,z),len(p-fvct(x,y,z))/100,100,0);++num;++cnt;
						for(;~r;++r) if(w.bxbl(r)) { --cnt; break; }
					}
				}
			}
			a=cnt*1.0f/num;
			pe->v+=(pe->realpos()-p)/dis*(1-dis/sz)*a;
			fvct d=(pe->realpos()-p)/dis*(1-dis/sz)*a;
		}
	}
}
