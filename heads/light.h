#pragma once
#include "framework.h"
#include "World.h"
#include "_math.h"
extern bool gameover;
struct LightGenerator
{
	std::pair<vct3<ll>,vct3<ll>> upd,qry;
	LightMap* plgt;
	std::vector<unsigned> lmp;
	//std::map<std::pair<vct3<ll>,ull>,unsigned> lmp;
	std::vector<std::vector<Chunk*>> cqry;
	vct3<ll> frm;
	ll radius;
	std::vector<std::pair<vct3<ll>,ull>> insv[16];
	LightGenerator(): upd(),qry(),plgt(nullptr),lmp(),cqry(),frm(),radius(0),insv() {}
	inline const Block gtbl(vct3<ll> v) const
	{
		vct3<ll> tmp=getChunkpos(v);
		Chunk* pc=cqry[tmp.x-qry.first.x][tmp.z-qry.first.z];
		tmp=getposChunk(v);
		return pc->b[tmp.x][tmp.y][tmp.z];
	}
	inline unsigned& gtlt(vct3<ll> v,ull i)
	{
	//	auto it=plgt->find(std::make_pair(v,i));
	//	if(it==plgt->end()) it=plgt->insert(std::make_pair(std::make_pair(v,i),0)).first;
		//if(v.x+radius+1-frm.x<0||v.x+radius+1-frm.x>=2*radius+3) throw "ERRX";
		//if(v.z+radius+1-frm.z<0||v.z+radius+1-frm.z>=2*radius+3) throw "ERRZ";
		//return lmp[std::make_pair(v,i)];
		return lmp[(v.x+radius+1-frm.x)*WorldHeight*(2*radius+3)*6
			+(v.z+radius+1-frm.z)*WorldHeight*6+v.y*6+i];
	}
	inline void copy()
	{
		for(ll i(frm.x-radius);i<=frm.x+radius;++i)
		{
			for(ll k(frm.z-radius);k<=frm.z+radius;++k)
			{
				unsigned* plmp=&gtlt(vct3<ll>(i,0,k),0);
				vct3<ll> b=getChunkpos(vct3<ll>(i,0,k));
				std::pair<ll,ll> ps(b.x,b.z);
				vct3<ull> t=getposChunk(vct3<ll>(i,0,k));
				auto& tgt=(*plgt)[ps];
				for(ull j(0);j<WorldHeight;++j)
					for(ull p(0);p<6;++p) tgt[t.x][j][t.z][p]=*(plmp++);
			}
		}
		//for(ull i(0);i<2*radius+3;++i)
		//{
		//	for(ull j(0);j<2*radius+3;++j)
		//	{
		//		for(ull k(0);k<WorldHeight;++k) delete[] lmp[i][j][k];
		//		delete[] lmp[i][j];
		//	}
		//	delete lmp[i];
		//}
		//delete[] lmp;
		//lmp=nullptr;

	}
	inline void init(vct3<ll> t1,ll t2,LightMap& t3)
	{
		frm=t1;radius=t2;plgt=&t3;
		//lmp.resize(2*radius+3,std::vector<std::vector<std::vector<unsigned>>>(2*radius+3,std::vector<std::vector<unsigned>>(WorldHeight,std::vector<unsigned>(6,0))));
		lmp.resize(0);
		lmp.resize((2*radius+3)*(2*radius+3)*WorldHeight*6,0);
		//lmp=new unsigned[(2*radius+3)*(2*radius+3)*WorldHeight*6];
		for(ull lgt(0);lgt<16;++lgt)
			insv[lgt].resize(0);
		//for(ull lgt(0);lgt<=15u;++lgt)
			//insv[lgt].reserve((2*radius+3)*(2*radius+3)*WorldHeight*6);
		/*lmp=new unsigned*** [2*radius+3];
		for(ull i(0);i<2*radius+3;++i)
		{
			lmp[i]=new unsigned** [2*radius+3];
			for(ull j(0);j<2*radius+3;++j)
			{
				lmp[i][j]=new unsigned* [WorldHeight];
				for(ull k(0);k<WorldHeight;++k)
				{
					lmp[i][j][k]=new unsigned[6];
					for(ull face(0);face<6;++face) lmp[i][j][k][face]=0;
				}
			}
		}*/
		qry=std::make_pair(getChunkpos(vct3<ll>(frm.x-radius-1,0,frm.z-radius-1)),getChunkpos(vct3<ll>(frm.x+radius+1,0,frm.z+radius+1)));
		upd=std::make_pair(getChunkpos(vct3<ll>(frm.x-radius,0,frm.z-radius)),getChunkpos(vct3<ll>(frm.x+radius,0,frm.z+radius)));
		cqry.resize(0);
		cqry.resize(qry.second.x-qry.first.x+1,
					std::vector<Chunk*>(qry.second.z-qry.first.z+1,nullptr));
		for(ll i(qry.first.x);i<=qry.second.x;++i)
		{
			for(ll k(qry.first.z);k<=qry.second.z;++k)
			{
				auto ps=std::make_pair(i,k);
				if(!wrld.mp.count(ps)) wrld.newchunk(i,k);
				cqry[i-qry.first.x][k-qry.first.z]=wrld.mp.find(ps)->second;
			}
		}
	}
	inline void reset()
	{
		//for(ll i(frm.x-radius);i<=frm.x+radius;++i)
		//{
		//	for(ll k(frm.z-radius);k<=frm.z+radius;++k)
		//	{
		//		for(ull j(0);j<WorldHeight;++j)
		//		{
		//			vct3<ll> v(i,j,k);
		//			for(ull p(0);p<6;++p) 
		///*debug("!!@@##$%$^;  %lld %lld %lld %llu  ;",v.x,v.y,v.z,p),*/gtlt(v,p)=0/*,debug("2222222222222!!@@##$%$^\n")*/;
		//		}
		//	}
		//}
	}
	inline void genlight()
	{
		static ll dx[]{-1,1,0,0,0,0},dy[]{0,0,-1,1,0,0},dz[]{0,0,0,0,-1,1};
		static ll tmpx[]{0,-1,0,1},tmpz[]{1,0,-1,0};
		static int face[]{XY1,ZY2,XY2,ZY1};
		for(ull k(0);k<4;++k)
		{
			for(ll x(-radius);x<=radius;++x)
			{
				for(ull y(0);y<WorldHeight;++y)
				{
					ll i,j;
					if(k==0) i=frm.x+x,j=frm.z+radius+1;
					if(k==1) j=frm.z+x,i=frm.x-radius-1;
					if(k==2) i=frm.x+x,j=frm.z-radius-1;
					if(k==3) j=frm.z+x,i=frm.x+radius+1;
					auto u=vct3<ll>(i,y,j);
					vct3<ll> v(u.x+dx[face[k]],u.y,u.z+dz[face[k]]);
					vct3<ll> b=getChunkpos(u);
					std::pair<ll,ll> ps(b.x,b.z);
					vct3<ull> t=getposChunk(u);
					unsigned tmp=0;
					if(plgt->find(ps)!=plgt->end())
					{
						tmp=(*plgt)[ps][t.x][t.y][t.z][face[k]];
					}
					if(gtlt(v,face[k]^1)<tmp) insv[gtlt(v,face[k]^1)=tmp].emplace_back(v,face[k]^1);
				}
			}
		}
		long tmp=0;
		ull cnt=0;
		ll maxx=radius+frm.x,minx=frm.x-radius,maxz=radius+frm.z,minz=frm.z-radius;
		for(ull lgt(15u);lgt>1u;--lgt)
		{
			for(ull id(0);id<insv[lgt].size();++id)
			{
				auto u=insv[lgt][id].first;
				auto ufc=insv[lgt][id].second;
				Block b2=gtbl(u);
				if(gtlt(u,ufc)>lgt) continue;
				++cnt;
				[&](ull lt)->void
				{
					vct3<ll> v(u.x+dx[ufc],u.y+dy[ufc],u.z+dz[ufc]);
					if((u.y+1==WorldHeight&&ufc==XZ2)||(u.y==0&&ufc==XZ1)) return;
					if(v.x>maxx||v.x<minx||v.z>maxz||v.z<minz) return;
					Block b=gtbl(v);
					int vfc=ufc^1;
					if(gtlt(v,vfc)<lt) insv[gtlt(v,vfc)=lt].emplace_back(v,vfc);
				}(lgt);
				if(lgt<=trslt[b2.tp]) continue;
				unsigned tmplgt=lgt-trslt[b2.tp]-1;
				unsigned* tlmp=&gtlt(u,0);
				for(ull k(0);k<6;++k,++tlmp)
				{
					if((*tlmp)<tmplgt)
					{
						(*tlmp)=tmplgt;
						if(tmplgt>=2u) insv[tmplgt].emplace_back(u,k);
					}
				}

			}
		}
	}
	inline virtual void prepare()=0;
	inline void operator()(vct3<ll> t1,ll t2,LightMap& t3)
	{
		debug("Genlight! radius = %lld\n",t2);
		clock_t st=clock();
		init(t1,t2,t3);
		debug("init time: %lf\n",(double)(clock()-st)/CLOCKS_PER_SEC);
		st=clock();
		//reset();
		prepare();
		debug("prepare time: %lf\n",(double)(clock()-st)/CLOCKS_PER_SEC);
		st=clock();
		genlight();
		debug("genlight time: %lf\n",(double)(clock()-st)/CLOCKS_PER_SEC);
		st=clock();
		copy();
		debug("copy time: %lf\n",(double)(clock()-st)/CLOCKS_PER_SEC);
		st=clock();
	}
};
struct SkyLightGenerator :LightGenerator
{
	inline void prepare()
	{
		insv[15].reserve((radius*2+3)*(radius*2+3)*(WorldHeight-waterl)*6);
		for(ll i(frm.x-radius);i<=frm.x+radius;++i)
		{
			for(ll k(frm.z-radius);k<=frm.z+radius;++k)
			{
				Block b1=gtbl(vct3<ll>(i,0,k));
				gtlt(vct3<ll>(i,0,k),XZ1)=15;
				insv[15].emplace_back(vct3<ll>(i,0,k),XZ1);
				Block b2=gtbl(vct3<ll>(i,0,k));
				Block b3=gtbl(vct3<ll>(i,1,k));
				for(unsigned j=0,lgt=15;j+1<WorldHeight;++j)
				{
					if(lgt<=trslt[b2.tp]) break;
					lgt-=trslt[b2.tp];
					gtlt(vct3<ll>(i,j+1,k),XZ1)=lgt;insv[lgt].emplace_back(vct3<ll>(i,j+1,k),XZ1);
					if(b3.tp==block::air) for(ull p(0);p<6;++p) if(p!=XZ1) gtlt(vct3<ll>(i,j+1,k),p)=lgt,insv[lgt].emplace_back(vct3<ll>(i,j+1,k),p);
					b2=b3; b3=gtbl(vct3<ll>(i,j+2,k));
				}
				Block b4=gtbl(vct3<ll>(i,WorldHeight-1,k));
				gtlt(vct3<ll>(i,WorldHeight-1,k),XZ2)=15;
				insv[15].emplace_back(vct3<ll>(i,WorldHeight-1,k),XZ2);
				Block b5=gtbl(vct3<ll>(i,WorldHeight-1,k));
				Block b6=gtbl(vct3<ll>(i,WorldHeight-2,k));
				for(unsigned j=WorldHeight-1,lgt=15;j;--j)
				{
					if(lgt<=trslt[b5.tp]) break;
					lgt-=trslt[b5.tp];
					gtlt(vct3<ll>(i,j-1,k),XZ2)=lgt;insv[lgt].emplace_back(vct3<ll>(i,j-1,k),XZ2);
					if(b6.tp==block::air) for(ull p(0);p<6;++p) if(p!=XZ2) gtlt(vct3<ll>(i,j-1,k),p)=lgt,insv[lgt].emplace_back(vct3<ll>(i,j-1,k),p);
					b5=b6;
					if(j>=2) b6=gtbl(vct3<ll>(i,j-2,k));
				}
			}
		}
	}
};
struct BlkLightGenerator :LightGenerator
{
	inline void prepare()
	{
		for(ll i(frm.x-radius);i<=frm.x+radius;++i)
		{
			for(ll k(frm.z-radius);k<=frm.z+radius;++k)
			{
				for(ull j(0);j<WorldHeight;++j)
				{
					Block b=gtbl(vct3<ll>(i,j,k));
					if(!light[b.tp]) continue;
					for(ull x(0);x<6;++x) gtlt(vct3<ll>(i,j,k),x)=light[b.tp],insv[light[b.tp]].emplace_back(vct3<ll>(i,j,k),x);
				}
			}
		}
	}
};