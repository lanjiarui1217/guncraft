#include "World.h"
#include "block.h"
#include "particle.h"

Chunk* World::getchunk(vct3<ll> v)
{
	if(v.y<0||v.y>=WorldHeight) return nullptr;
	v=getChunkpos(v);
	auto it=mp.find(std::make_pair(v.x,v.z));
	if(it!=mp.end()) return it->second;
	newchunk(v.x,v.z);
	return mp[std::make_pair(v.x,v.z)];
}
void World::_vison(vct3<ll> v)
{
	if(v.y<0||v.y>=WorldHeight) return;
	vct3<ll> v2=getChunkpos(v);
	auto it=mp.find(std::make_pair(v2.x,v2.z));
	if(it==mp.end()) return ;
	Chunk& c=*(*it).second;
	vct3<ull> uv=getposChunk(v);
	ull i=uv[1],j=uv[2],k=uv[3];

	if(j&&c.b[i][j][k].tp==block::sand&&c.b[i][j-1][k].tp==block::air) ve.push_back(new fallsand(fvct(v))),wrld.setbl(v,block::air);
	else if((!j)&&c.b[i][j][k].tp==block::sand) ve.push_back(new fallsand(fvct(v))),wrld.setbl(v,block::air);
	auto oldvis=c.b[i][j][k].vis;
	auto& vis=c.b[i][j][k].vis;vis=0x3f;
	if(i>0&&occl[c.b[i-1][j][k].tp][ZY2]) vis&=~(1<<ZY1);
	if(k>0&&occl[c.b[i][j][k-1].tp][XY2]) vis&=~(1<<XY1);
	if(j>0&&occl[c.b[i][j-1][k].tp][XZ2]) vis&=~(1<<XZ1);
	if(i<Chunksize-1&&occl[c.b[i+1][j][k].tp][ZY1]) vis&=~(1<<ZY2);
	if(k<Chunksize-1&&occl[c.b[i][j][k+1].tp][XY1]) vis&=~(1<<XY2);
	if(j<WorldHeight-1&&occl[c.b[i][j+1][k].tp][XZ1]) vis&=~(1<<XZ2);

	if(i==0&&occl[_getbl(v-vct3<ll>(1,0,0)).tp][ZY2]) vis&=~(1<<ZY1);
	if(k==0&&occl[_getbl(v-vct3<ll>(0,0,1)).tp][XY2]) vis&=~(1<<XY1);
	if(i+1==Chunksize&&occl[_getbl(v+vct3<ll>(1,0,0)).tp][ZY1]) vis&=~(1<<ZY2);
	if(k+1==Chunksize&&occl[_getbl(v+vct3<ll>(0,0,1)).tp][XY1]) vis&=~(1<<XY2);

	//if(c.b[i][j][k].tp==block::glass)
	//{
	//	if(i>0&&c.b[i-1][j][k].tp==block::glass) vis&=~(1<<ZY1);
	//	if(k>0&&c.b[i][j][k-1].tp==block::glass) vis&=~(1<<XY1);
	//	if(j>0&&c.b[i][j-1][k].tp==block::glass) vis&=~(1<<XZ1);
	//	if(i<Chunksize-1&&c.b[i+1][j][k].tp==block::glass) vis&=~(1<<ZY2);
	//	if(k<Chunksize-1&&c.b[i][j][k+1].tp==block::glass) vis&=~(1<<XY2);
	//	if(j<WorldHeight-1&&c.b[i][j+1][k].tp==block::glass) vis&=~(1<<XZ2);

	//	if(i==0&&_getbl(v-vct3<ll>(1,0,0)).tp==block::glass) vis&=~(1<<ZY1);
	//	if(k==0&&_getbl(v-vct3<ll>(0,0,1)).tp==block::glass) vis&=~(1<<XY1);
	//	if(i+1==Chunksize&&_getbl(v+vct3<ll>(1,0,0)).tp==block::glass) vis&=~(1<<ZY2);
	//	if(k+1==Chunksize&&_getbl(v+vct3<ll>(0,0,1)).tp==block::glass) vis&=~(1<<XY2);

	//}

	if(c.b[i][j][k].tp==block::air) c.rd.erase(vct3<ull>(i,j,k));
	else if(vis) c.rd.insert(vct3<ull>(i,j,k));
	else if((!vis)&&oldvis) c.rd.erase(vct3<ull>(i,j,k));
}


void World::_updvis(vct3<ll> v)
{
	_vison(v);
	_vison(v-vct3<ll>(1,0,0));
	_vison(v+vct3<ll>(1,0,0));
	_vison(v-vct3<ll>(0,1,0));
	_vison(v+vct3<ll>(0,1,0));
	_vison(v-vct3<ll>(0,0,1));
	_vison(v+vct3<ll>(0,0,1));
}

Block World::_getbl(vct3<ll> v)
{
	if(v.y<0||v.y>=WorldHeight) return Block(block::air,0,fvct(v));
	vct3<ll> v2=getChunkpos(v);
	auto it=mp.find(std::make_pair(v2.x,v2.z));
	if(it==mp.end()) return Block(block::BLOCKNUM,0,fvct(v));
	Chunk& c=*(*it).second;
	vct3<ull> uv=getposChunk(v);
	return c.b[uv.x][uv.y][uv.z];
}

bool World::_setbl(vct3<ll> v,blocktype b)
{
	if(v.y<0||v.y>=WorldHeight) return false;
	vct3<ll> v2=getChunkpos(v);
	auto it=mp.find(std::make_pair(v2.x,v2.z));
	if(it==mp.end()) return false;
	Chunk& c=*(*it).second;
	vct3<ull> uv=getposChunk(v);
	c.b[uv.x][uv.y][uv.z].tp=b;
	_updvis(v);
	//if(b==block::air)
	//{
	//	if(uv.y==c.height[uv.x][uv.z])
	//	{
	//		for(;(unsigned char)(c.height[uv.x][uv.z]+1);--c.height[uv.x][uv.z])
	//		{
	//			if(c.b[uv.x][c.height[uv.x][uv.z]][uv.z].tp!=block::air) break;
	//		}
	//	}
	//}
	//else c.height[uv.x][uv.z]=max(c.height[uv.x][uv.z],(unsigned)uv.y);
	return true;
}
#include <list>
#include "light.h"
void World::genlight_round(vct3<ll> frm,int radius)
{
	if(gameover) return;
	static SkyLightGenerator slg;slg(frm,radius,skylight);
	static BlkLightGenerator blg;blg(frm,radius,blklight);
}

void World::vison(vct3<ll> v)
{
	if(v.y<0||v.y>=WorldHeight) return;
	Chunk& c=*getchunk(v);
	vct3<ull> uv=getposChunk(v);
	ull i=uv[1],j=uv[2],k=uv[3];

	if(j&&c.b[i][j][k].tp==block::sand&&c.b[i][j-1][k].tp==block::air) ve.push_back(new fallsand(fvct(v.x,v.y,v.z))),wrld.setbl(v,block::air);
	else if((!j)&&c.b[i][j][k].tp==block::sand) ve.push_back(new fallsand(fvct(v.x,v.y,v.z))),wrld.setbl(v,block::air);
	auto oldvis=c.b[i][j][k].vis;
	auto& vis=c.b[i][j][k].vis;vis=0x3f;
	if(i>0&&occl[c.b[i-1][j][k].tp][ZY2]) vis&=~(1<<ZY1);
	if(k>0&&occl[c.b[i][j][k-1].tp][XY2]) vis&=~(1<<XY1);
	if(j>0&&occl[c.b[i][j-1][k].tp][XZ2]) vis&=~(1<<XZ1);
	if(i<Chunksize-1&&occl[c.b[i+1][j][k].tp][ZY1]) vis&=~(1<<ZY2);
	if(k<Chunksize-1&&occl[c.b[i][j][k+1].tp][XY1]) vis&=~(1<<XY2);
	if(j<WorldHeight-1&&occl[c.b[i][j+1][k].tp][XZ1]) vis&=~(1<<XZ2);

	if(i==0&&occl[getbl(v-vct3<ll>(1,0,0)).tp][ZY2]) vis&=~(1<<ZY1);
	if(k==0&&occl[getbl(v-vct3<ll>(0,0,1)).tp][XY2]) vis&=~(1<<XY1);
	if(i+1==Chunksize&&occl[getbl(v+vct3<ll>(1,0,0)).tp][ZY1]) vis&=~(1<<ZY2);
	if(k+1==Chunksize&&occl[getbl(v+vct3<ll>(0,0,1)).tp][XY1]) vis&=~(1<<XY2);
	if(c.b[i][j][k].tp==block::air) c.rd.erase(vct3<ull>(i,j,k));
	else if(vis) c.rd.insert(vct3<ull>(i,j,k));
	else if((!vis)&&oldvis) c.rd.erase(vct3<ull>(i,j,k));
}

#include "Generate.h"
void World::updvis(vct3<ll> v)
{
	vison(v);
	vison(v-vct3<ll>(1,0,0));
	vison(v+vct3<ll>(1,0,0));
	vison(v-vct3<ll>(0,1,0));
	vison(v+vct3<ll>(0,1,0));
	vison(v-vct3<ll>(0,0,1));
	vison(v+vct3<ll>(0,0,1));
	if(v.y<0||v.y>=WorldHeight) return;
	genlight_round(v,14u);
	//Chunk& c=*getchunk(v);
	//ll x=c.x,z=c.z;
	//for(ull i(0);i<Chunksize;++i)
	//{
	//	for(ull k(0);k<Chunksize;++k)
	//	{
	//		for(ull j(0);j<WorldHeight;++j)
	//		{
	//			memset(c.b[i][j][k].light,0,sizeof c.b[i][j][k].light);
	//		}
	//	}
	//}
	//if(mp.count(std::make_pair(x-1,z))) GenerateLight(*mp[std::make_pair(x-1,z)],worldtype::Overworld);
	//if(mp.count(std::make_pair(x+1,z))) GenerateLight(*mp[std::make_pair(x+1,z)],worldtype::Overworld);
	//if(mp.count(std::make_pair(x,z-1))) GenerateLight(*mp[std::make_pair(x,z-1)],worldtype::Overworld);
	//if(mp.count(std::make_pair(x,z+1))) GenerateLight(*mp[std::make_pair(x,z+1)],worldtype::Overworld);
	//GenerateLight(c,worldtype::Overworld);
}

Block World::getbl(vct3<ll> v)
{
	if(v.y<0||v.y>=WorldHeight) return Block(block::air,0,fvct(v));
	Chunk& c=*getchunk(v);
	vct3<ull> uv=getposChunk(v);
	return c.b[uv.x][uv.y][uv.z];
}

bool World::setbl(vct3<ll> v,blocktype b)
{
	if(v.y<0||v.y>=WorldHeight) return false;
	Chunk& c=*getchunk(v);
	vct3<ull> uv=getposChunk(v);
	c.b[uv.x][uv.y][uv.z].tp=b;
	updvis(v);
	return true;
}

bool World::bxbl(const Box& b)
{
	for(ll x((ll)floor(b.min.x));x<=(ll)ceil(b.max.x);++x)
	{
		for(ll y((ll)floor(b.min.y));y<=(ll)ceil(b.max.y);++y)
		{
			for(ll z((ll)floor(b.min.z));z<=(ll)ceil(b.max.z);++z)
			{
				if((coll[getbl(vct3<ll>(x,y,z)).tp]+
				   vct3<float>((float)x,(float)y,(float)z))&b) return true;
			}
		}
	}
	return false;
}

bool World::bxbl(fvct v)
{
	vct3<ll> vi(floor(v));
	return (coll[getbl(vi).tp]+vi)&v;
}

World::World() : skylight(),blklight()
{}

World::~World()
{
	for(auto&pr:mpthread)
	{
		auto& th=pr.second;
		if(!ltmp[pr.first]) th.wait();
	}
	for(const auto p:mp)
	{
		delete p.second;
	}
}

void World::pick(Player&p)
{
	itemtype nwtp=p.itemlist[p.chooseind].tp;
	if(nwtp==item::AK47)
	{
	//	if(clock()-p.lstshoottime<=CLOCKS_PER_SEC*0.3) return;
		fvct v=(tspl.lkt-tspl.cmp)/len(tspl.lkt-tspl.cmp)*710;
		ve.push_back(new AmmoM1943(tspl.cmp,v));
		p.v+=(p.cmp-p.lkt)/len(p.cmp-p.lkt)*0.2f;
		p.lstshoottime=clock();
		return;
	}
	Ray r(p.cmp,p.lkt,1.0f/24,96);
	for(;~r;++r)
	{
		if(bxbl(r)) break;
	}
	static vct3<ll> lst;
	static float tm;
	if(~r)
	{
		vct3<ll> nw(floor((fvct)r));
		if(nw!=lst) tm=0;
		lst=nw;tm+=1/fps;
		//if(tm<=time) return ;
		tm=0;
		blocktype temp=getbl(nw).tp;
		setbl(nw,block::air);
		// particle
		for(ull i(0);i<16;++i)
		{
			vptc.push_back(Particle(particletype::block,
						  fvct(nw)+fvct(rand()*1.0/RAND_MAX,rand()*1.0/RAND_MAX,rand()*1.0/RAND_MAX),
						  fvct(-0.375+rand()*0.75/RAND_MAX,rand()*1.0/RAND_MAX,-0.375+rand()*0.75/RAND_MAX),
						  fvct(0,-9.8,0),(temp<<3)|(rand()%6)));
		}
		ve.push_back(new drop(fvct(nw)+fvct(0.5f,0.5f,0.5f),blkdrop[temp]));
	}
}

#include "Entity.h"
void World::use(Player&p)
{
	if(p.itemlist[p.chooseind].tp==ITEMNUM) return;
	//if(p.itemlist[p.chooseind].tp==item::falcula)
	//{
	//	static fvct st(0,0,0),ed(0,0,0);
	//	static bool used(false);
	//	if(!used)
	//	{
	//		used=1;
	//		st=
	//	}
	//}
	if(p.itemlist[p.chooseind].tp==item::grenade)
	{
		if(clock()-p.lstgrenadetime<=CLOCKS_PER_SEC) return;
		fvct v=(tspl.lkt-tspl.cmp)/len(tspl.lkt-tspl.cmp)*20;
		ve.push_back(new Grenade(tspl.cmp,v));
		p.lstgrenadetime=clock();
		return;
	}
	if(p.itemlist[p.chooseind].tp==item::guidedmissile)
	{
		if(pmissile) return;
		fvct v=(tspl.lkt-tspl.cmp)/len(tspl.lkt-tspl.cmp)*20;
		ve.push_back(new GuidedMissile(tspl.cmp,v));
		return;
	}
	if(p.itemlist[p.chooseind].tp==item::ender_pearl)
	{
		if(clock()-p.lstenderpearltime<=CLOCKS_PER_SEC) return;
		const float g=-12.0f;
		// vt=60 vt=k(vt+g) 60=k*72 k=5/6
		// v=kv+kg
		ve.push_back(new enderpearl((void*)(&p)));
		p.lstenderpearltime=clock();
		return;
	}
	Ray r(p.cmp,p.lkt,1.0f/24,96);
	fvct ps=r;
	for(;~r;++r)
	{
		if(bxbl(r)) break;
		ps=r;
	}
	if(~r)
	{
		static clock_t lst=clock()-CLOCKS_PER_SEC*0.2;
		if(clock()-lst<=CLOCKS_PER_SEC*0.1) return;
		lst=clock();
		vct3<ll> nw(floor(ps));
		blocktype temp=toblock[p.itemlist[p.chooseind].tp];
		if(!((coll[temp]+nw)&(tspl.coll+tspl.p)))
		{
			--p.itemlist[p.chooseind].num;
			if(temp!=block::TNT) setbl(nw,temp);
			else
			{
				ve.push_back(new actTNT(fvct(nw)));
			}
		}
	}
}

#include "Generate.h"

void World::newchunk(ll x,ll z)
{
	Chunk* pc=new Chunk(x,z,[&](Chunk& c) { return Generate(c,worldtype::Overworld); });
	mp[std::make_pair(x,z)]=pc;GeneratePlant(*pc,worldtype::Overworld);
}

#include "Player.h"

bool World::shdupd()
{
	static bool flag=1;
	static fvct lstps;
	constexpr float reupdsz=6;
	if(flag||len(lstps-tspl.cmp)>=2)
	{
		flag=0;
		lstps=tspl.cmp;
		return true;
	}
	return false;
}

void World::update(bool tp)
{
	ll cx=(ll)floor(tspl.cmp.x/(ll)Chunksize),cz=(ll)floor(tspl.cmp.z/(ll)Chunksize);
	ll l;
	l=(Rendersize-1)/(ll)(Chunksize)+1ll;
	for(ll x(cx-l-8);x<=cx+l+8;++x)
	{
		for(ll z(cz-l-8);z<=cz+l+8;++z)
		{
			std::pair<ll,ll> pr(x,z);
			if(!mp.count(pr)) newchunk(x,z);
		}
	}

	ull cnt=0,cnt2=0;
	for(ll x(cx-l);x<=cx+l;++x)
	{
		for(ll z(cz-l);z<=cz+l;++z)
		{
			++cnt2;
			std::pair<ll,ll> pr(x,z);
			if(ltmp[pr]) continue;
			++cnt;
		}
	}
	if(cnt*9<=cnt2)
	{
		for(ll x(cx-l);x<=cx+l;++x)
		{
			for(ll z(cz-l);z<=cz+l;++z)
			{
				std::pair<ll,ll> pr(x,z);
				if(ltmp[pr]) continue;
				if(mpthread.find(pr)!=mpthread.end())
				{
					mpthread[pr].wait();
					continue;
				}
				genlight_round(vct3<ll>(x*Chunksize+Chunksize/2,0,z*Chunksize+Chunksize/2),2*Chunksize-Chunksize/2);
				totmutex.lock();
				ltmp[std::make_pair(x,z)]=1;
				totmutex.unlock();
			}
		}
	}
	else
	{
		genlight_round(floor(tspl.cmp),Rendersize+Chunksize);
		for(ll x(cx-l);x<=cx+l;++x)
		{
			for(ll z(cz-l);z<=cz+l;++z)
			{
				std::pair<ll,ll> pr(x,z);
				if(ltmp[pr]) continue;
				totmutex.lock();
				ltmp[std::make_pair(x,z)]=1;
				totmutex.unlock();
			}
		}
	}
	//for(ll end=(++l)+3ll;l<=end;++l)
	//{
	//	for(ll x(cx-l);x<=cx+l;++x)
	//	{
	//		for(ll z(cz-l);z<=cz+l;++z)
	//		{
	//			std::pair<ll,ll> pr(x,z);
	//			if(ltmp[pr]||mpthread.find(pr)!=mpthread.end()) continue;
	//			mpthread.insert(std::make_pair(pr,std::async(std::launch::async|std::launch::deferred,
	//							[&](ll x,ll z)
	//							{
	//								genlight_round(vct3<ll>(x*Chunksize+Chunksize/2,0,z*Chunksize+Chunksize/2),2*Chunksize-Chunksize/2);
	//								totmutex.lock();
	//								ltmp[std::make_pair(x,z)]=1;
	//								totmutex.unlock();
	//							},x,z)));
	//		}
	//	}
	//}
	//if(!tp) return;
	//--l;
	//for(ll x(cx-l);x<=cx+l;++x)
	//{
	//	for(ll z(cz-l);z<=cz+l;++z)
	//	{
	//		std::pair<ll,ll> pr(x,z);
	//		if(!ltmp[pr]) mpthread[pr].wait();
	//	}
	//}
}

void World::render()
{
	if(shdupd()) update();
	float plx=tspl.cmp.x,plz=tspl.cmp.z;
	float plcx=plx*1.0f/(float)Chunksize-1.0f/2,plcz=plz*1.0f/(float)Chunksize-1.0f/2;
	for(auto& pr:mp)
	{
		rndsps dr=0x3f;
		double x=tspl.cmp.x/Chunksize,z=tspl.cmp.z/Chunksize;
		Chunk& c=*pr.second;
		if(len(fvct(c.x-plcx,0,c.z-plcz))*Chunksize>=Rendersize+Chunksize) continue;
		if(x>c.x+1) dr&=0x3e;
		if(x<c.x) dr&=0x3d;
		if(z>c.z+1) dr&=0x2f;
		if(z<c.z) dr&=0x1f;
		c.render(dr);
	}
	for(auto& pr:trb)
	{
		vct3<ll> ps=pr.second;
		Chunk& c=*getchunk(ps);
		vct3<ull> uv=getposChunk(ps);
		{
			Block b=getbl(ps);
			if(b.tp<block::BLOCKNUM) BlockDrawUtil::draw(b,0x3f);
		}
	}
	trb.clear();
}

World wrld;