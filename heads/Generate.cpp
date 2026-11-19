#include "Generate.h"
#include "noise.h"
#include "Player.h"
#include "World.h"
#include <mutex>
extern std::mutex mutex_gen;
std::map<vct3<ll>,std::vector<std::pair<vct3<ll>,blocktype>>> genleaf;
void Generate(Chunk& c,WorldType tp)
{
	static noise hnse(noisedefseed^19260817),tnse(noisedefseed^71926081),hmnse(noisedefseed^17192608),cnse(noisedefseed^81719260);
	noise nse;
	(void)tp;
	unsigned char height[Chunksize][Chunksize];
	[&]()->void // generate height
	{
		int cyssz[]={4,16,64};
		float wght[]={0.12f,0.22f,0.66f};
		float mxhigh[]={waterl,waterl,waterl};
		float ht[Chunksize][Chunksize];
		fvct2 chkps((float)c.x,(float)c.z);
		for(ull i=0;i<Chunksize;++i)
		{
			for(ull j=0;j<Chunksize;++j)
			{
				ht[i][j]=0;
			}
		}
		for(ull d(0);d<3;++d)
		{
			hnse.prehandleSimplexNoise(chkps,cyssz[d]);
			for(ull i=0;i<Chunksize;++i)
			{
				for(ull j=0;j<Chunksize;++j)
				{
					fvct2 pf(float(i)/Chunksize/cyssz[d],float(j)/Chunksize/cyssz[d]);
					ht[i][j]+=(hnse.simplexNoise(pf)+1)*mxhigh[d]*wght[d];
				}
			}
		}
		for(ull i=0;i<Chunksize;++i)
		{
			for(ull j=0;j<Chunksize;++j)
			{
				height[i][j]=(unsigned char)(ht[i][j]<0?0:(ht[i][j]>255?255:floor(ht[i][j])));
			}
		}
	}();
	[&]()->void // generate temperature and humidity
	{
		ull cyssz=16,t=3;
		for(ull T(t);T--;cyssz*=2)
		{
			tnse.prehandleSimplexNoise(fvct2((float)c.x,(float)c.z),(int)cyssz);
			hmnse.prehandleSimplexNoise(fvct2((float)c.x,(float)c.z),(int)cyssz);
			for(ull i(0);i<Chunksize;++i)
			{
				for(ull j(0);j<Chunksize;++j)
				{
					c.temp[i][j]=c.humi[i][j]=0;
				}
			}
			for(ull i(0);i<Chunksize;++i)
			{
				for(ull j(0);j<Chunksize;++j)
				{
					fvct2 pf(float(i)/Chunksize/cyssz,float(j)/Chunksize/cyssz);
					float vl1=tnse.simplexNoise(pf)+(tnse.rand(pf)-0.5f)*0.05f;
					if(vl1<-1.0f) vl1=-1.0f;
					if(vl1>1.0f) vl1=1.0f;
					c.temp[i][j]+=vl1/t;
					float vl2=fabs(hmnse.simplexNoise(pf))+hmnse.rand(pf)*0.05f;
					if(vl2<0) vl2=0;
					if(vl2>1.0f) vl2=1.0f;
					c.humi[i][j]+=vl2/t;
				}
			}
		}
	}();
	[&]()->void // generate biome
	{
		for(ull i(0);i<Chunksize;++i)
		{
			for(ull j(0);j<Chunksize;++j)
			{
				fvct2 pf((float)(ll)(i+Chunksize*c.x),(float)(ll)(j+Chunksize*c.z));
				float tem=c.temp[i][j],hum=c.humi[i][j];
				unsigned char h=height[i][j];
				if(h>waterl) tem-=(h-waterl)*0.02f;
				biome& bm=c.biom[i][j];
				if(tem<-0.2f) bm=biometype::snow;
				else if(tem<0.2f&&hum<0.02f) bm=biometype::stone;
				else if(tem<0.2f&&hum<0.1f) bm=biometype::dry;
				else if(tem<0.2f||hum>0.7f) bm=biometype::green;
				else bm=biometype::desert; // tem>=0.2f && hum<=0.7f
			}
		}
	}();
	memset(c.b,0,sizeof(c.b));
	for(ull cx(0);cx<Chunksize;++cx) for(ull cz(0);cz<Chunksize;++cz)
	{
		for(unsigned char y(0);y<=height[cx][cz];++y)
		{
			blocktype& b=(c.b)[cx][y][cz].tp;
			if(y==0)
			{
				b=block::bedrock;
				continue;
			}
			if(y+20<height[cx][cz])
			{
				b=block::stone;
				continue;
			}
			ull del=height[cx][cz]-y;
			if(del>=3) b=block::stone;
			else if(del>=1) b=block::soil;
			else
			{
				switch(c.biom[cx][cz])
				{
				case biometype::snow: b=block::snowsoil; break;
				case biometype::green: b=block::grasssoil; break;
				case biometype::dry: b=block::soil; break;
				case biometype::stone: b=block::stone; break;
				case biometype::desert: b=block::sand; break;
				}
			}
			if(rand()%255>=250&&del) b=block::soil;
			fvct2 pf((float)(ll)(c.x*Chunksize+cx),(float)(ll)(c.z*Chunksize+cz));
			if(del==0&&c.biom[cx][cz]==biometype::stone&&cnse.hash21(pf)<10) b=block::ironore;
			if(cnse.hash31(fvct(pf.x,y,pf.y))<6) b=block::ironore;
		}
	}
	[&]()->void // generate cave
	{
		ull m=2;
		cnse.prehandleSimplexNoise(fvct((float)c.x,0.0f,(float)c.z),(int)m);
		for(ull i(0);i<Chunksize;++i)
		{
			for(ull k(0);k<=Chunksize;++k)
			{
				unsigned char mx=height[i][k];
				for(ull j(height[i][k]);j+10>=height[i][k];--j)
				{
					fvct pf((float)(i)/Chunksize/m,(float)(j)/Chunksize/m,(float)(k)/Chunksize/m);
					if(cnse.simplexNoise(pf)+(height[i][k]-j)*0.03f>0.5f)
					{
						c.b[i][j][k].tp=block::air;
					}
					else mx=std::max(mx,(unsigned char)j);
				}
				height[i][k]=mx;
			}
		}
	};
	//auto tmp=++c.height[15][15];
	//for(ull i(0);i<Chunksize;++i)
	//	for(ull k(0);k<Chunksize;++k)
	//		c.b[i][255][k].tp=block::bedrock,c.height[i][k]=255;
	//if(c.x%2==0||c.z%2==0) return;
	//c.b[15][tmp][15].tp=block::glowstone;
}
void GeneratePlant(Chunk& c,WorldType tp)
{
	static noise nse(noisedefseed^20111217);
	static bool first=1;
	if(first)
	{
		first=0;
		nse.setseed(rand()|(rand()<<15));
	}
	auto gentree=[&](ull i,ull j,ull cyssz)->bool
	{
		auto genleaves=[&](ull x,ull y,ull h,ull rad,blocktype leafid)
		{
			static bool lvtemp[4][5][5]={
			{   {0,0,0,0,0},
				{0,0,1,0,0},
				{0,1,1,1,0},
				{0,0,1,0,0},
				{0,0,0,0,0}},

			{   {0,0,0,0,0},
				{0,1,1,1,0},
				{0,1,1,1,0},
				{0,1,1,1,0},
				{0,0,0,0,0}},

			{   {0,1,1,1,0},
				{1,1,1,1,1},
				{1,1,1,1,1},
				{1,1,1,1,1},
				{0,1,1,1,0}},

			{   {1,1,1,1,1},
				{1,1,1,1,1},
				{1,1,1,1,1},
				{1,1,1,1,1},
				{1,1,1,1,1}}
			};
			for(ull i(0);i<5;++i)
			{
				for(ull j(0);j<5;++j)
				{
					if(!lvtemp[rad][i][j]) continue;
					ll dx=x+i-2,dy=y+j-2;ll gx=dx+c.x*Chunksize,gy=dy+c.z*Chunksize;
					blocktype b=wrld._getbl(vct3<ll>(gx,h,gy)).tp;
					if(b==BLOCKNUM) genleaf[getChunkpos(vct3<ll>(gx,h,gy))].emplace_back(vct3<ll>(gx,h,gy),leafid);
					if(b==BLOCKNUM||b!=block::air) continue;
					wrld._setbl(vct3<ll>(gx,h,gy),leafid);
				}
			}
		};
		
		unsigned char h=255;
		while(h&&c.b[i][h][j].tp==block::air) --h;
		if(h<=waterl-1) return 0;
		fvct2 pf(float(i)/Chunksize/cyssz,float(j)/Chunksize/cyssz);
		float tem=c.temp[i][j],hum=c.humi[i][j];
		if(h>waterl) tem-=(h-waterl)*0.02f;
		float prob=(nse.simplexNoise(pf)+1.0f)/2.0f*0.15f-fabs(tem+0.1f)*0.1f+hum*0.15f+nse.rand(-pf)*0.9f;
		if(prob<0.7f) return 0;// 0.985f
		tem+=(nse.rand(fvct2(1,1)+pf)-0.5f)*0.2f;
		// choose wood type by tem
		ull treeh=4+(nse.randint(fvct2(1,1)-pf)%3);
		if(h+treeh>=WorldHeight) return 0;
		//debug("GEN TREE! %lld,%lld;%u,%llu\n",i+c.x*Chunksize,j+c.z*Chunksize,h,treeh);
		for(ull k(0);k<treeh;++k)
		{
			vct3<ll> v(i+Chunksize*c.x,h+1+k,j+Chunksize*c.z);
			blocktype b=c.b[i][h+1+k][j].tp;
			c.b[i][h+1+k][j].tp=block::logwood;
			wrld._updvis(v);
		}
		ull t1=ull(nse.rand(17*pf)*4+1.5f)+(treeh>=4)+1;
		ull t2=ull(nse.rand(11*pf)*4+1.5f)+(treeh>=4)+1;
		ull leafh=treeh+1+t1%3;
		ull initleafh=2+t2%2;
		for(ull k(leafh-1);k>=initleafh;--k)
		{
			float rad=nse.bezier(fvct2(0,0),fvct2(0.33f,(float)t1),fvct2(0.66f,(float)t2),fvct2(1,0),float(k-initleafh)/(leafh-1-initleafh)).y;
			if(rad<0) rad=0.0f; if(rad>=4.0f) rad=3.9f;
			genleaves(i,j,h+1+k,(ull)rad,block::leaf);
		}
		return 1;
	};
	ull cyssz=16;
	nse.prehandleSimplexNoise(fvct2((float)c.x,(float)c.z),(int)cyssz);
	for(ull i(0);i<Chunksize;++i)
	{
		for(ull j(0);j<Chunksize;++j)
		{
			if(c.biom[i][j]==biometype::none||
			   c.biom[i][j]==biometype::desert||
			   c.biom[i][j]==biometype::stone) continue;
			unsigned char h=255;
			while(h&&c.b[i][h][j].tp==block::air) --h;
			blocktype b=c.b[i][h][j].tp;
			if(b!=block::grasssoil&&b!=block::soil&&b!=block::snowsoil) continue;
			if(gentree(i,j,cyssz)) continue;
		}
	}
	//debug("###");
	for(const auto& pr:genleaf[vct3<ll>(c.x,0,c.z)])
	{
		vct3<ll> v=pr.first;
		blocktype leafid=pr.second;
		ull i=v.x-c.x*Chunksize,j=v.y,k=v.z-c.z*Chunksize;
		blocktype b=c.b[i][j][k].tp;
		if(b!=block::air&&b!=block::BLOCKNUM) continue;
		c.b[i][j][k].tp=leafid;
		wrld._updvis(v);
	}
	//debug("@@@\n");
}
#include <queue>
#include "block.h"
//void GenerateLight(Chunk& c,WorldType)
//{
//	// ZY1,ZY2,XZ1,XZ2,XY1,XY2
//	static ull dx[]{~0ull,1,0,0,0,0},dy[]{0,0,~0ull,1,0,0},dz[]{0,0,0,0,~0ull,1};
//	std::deque<std::pair<unsigned,std::pair<vct3<ull>,int>>> q;
//	std::vector<std::pair<vct3<ull>,int>> insv[16];
//	for(ull i(0);i<Chunksize;++i)
//	{
//		for(ull k(0);k<Chunksize;++k)
//		{
//			for(ull j(0);j<WorldHeight;++j)
//			{
//				memset(c.b[i][j][k].light,0,sizeof c.b[i][j][k].light);
//				if(light[c.b[i][j][k].tp])
//				{
//					for(ull x(0);x<6;++x)
//						c.b[i][j][k].light[x]=light[c.b[i][j][k].tp],
//						insv[light[c.b[i][j][k].tp]].emplace_back(vct3<ull>(i,j,k),x);
//				}
//			}
//		}
//	}
//	for(ull i(0);i<Chunksize;++i)
//	{
//		for(ull k(0);k<Chunksize;++k)
//		{
//			c.b[i][0][k].light[XZ1]=15u;
//			q.push_front(std::make_pair(15u,std::make_pair(vct3<ull>(i,0,k),XZ1)));
//			for(unsigned j=0,lgt=15u;j+1<WorldHeight;++j)
//			{
//				if(lgt<=trslt[c.b[i][j][k].tp]) break;
//				lgt-=trslt[c.b[i][j][k].tp];
//				c.b[i][j+1][k].light[XZ1]=lgt;
//				insv[lgt].push_back(std::make_pair(vct3<ull>(i,j+1,k),XZ1));
//			}
//			if((unsigned)(c.height[i][k]+1u)==0u) continue;
//			c.b[i][c.height[i][k]][k].light[XZ2]=15u;
//			q.push_front(std::make_pair(15u,std::make_pair(vct3<ull>(i,c.height[i][k],k),XZ2)));
//			for(unsigned j=c.height[i][k],lgt=15u;j;--j)
//			{
//				if(lgt<=trslt[c.b[i][j][k].tp]) break;
//				lgt-=trslt[c.b[i][j][k].tp];
//				c.b[i][j-1][k].light[XZ2]=lgt;
//				insv[lgt].push_back(std::make_pair(vct3<ull>(i,j-1,k),XZ2));
//			}
//			q.push_front(std::make_pair(15u,std::make_pair(vct3<ull>(i,c.height[i][k]+1,k),XZ2)));
//		}
//	}
//	static ll tmpx[]{0,-1,0,1},tmpz[]{1,0,-1,0};
//	static int face[]{XY1,ZY2,XY2,ZY1};
//	for(ull k(0);k<4;++k)
//	{
//		if(!wrld.mp.count(std::pair<ll,ll>(c.x+tmpx[k],c.z+tmpz[k]))) continue;
//		Chunk& tmp=*wrld.mp[std::pair<ll,ll>(c.x+tmpx[k],c.z+tmpz[k])];
//		for(ull x(0);x<Chunksize;++x)
//		{
//			for(ull y(0);y<WorldHeight;++y)
//			{
//				ull i,j;
//				if(k==0) i=x,j=0;
//				if(k==1) i=Chunksize-1,j=x;
//				if(k==2) i=x,j=Chunksize-1;
//				if(k==3) i=0,j=x;
//				if(tmp.b[i][y][j].light[face[k]]>=2u) insv[tmp.b[i][y][j].light[face[k]]].push_back(std::make_pair(vct3<ull>(i+tmpx[k]*Chunksize,y,j+tmpz[k]*Chunksize),face[k]));
//			}
//		}
//	}
//	unsigned lst=16;
//	while(q.size())
//	{
//		unsigned lgt=q.front().first;
//		auto u=q.front().second.first;
//		auto ufc=q.front().second.second;
//		q.pop_front();
//		if(lgt!=lst)
//		{
//			q.push_front(std::make_pair(lgt,std::make_pair(u,ufc)));
//			for(unsigned k(lst-1);k+10>=lgt+10;--k) for(auto tmp:insv[k]) q.push_front(std::make_pair(k,tmp));
//			lgt=q.front().first;u=q.front().second.first;
//			ufc=q.front().second.second; q.pop_front();
//			lst=lgt;
//		}
//		vct3<ull> v(u.x+dx[ufc],u.y+dy[ufc],u.z+dz[ufc]);
//		int vfc=ufc^1;
//		if(v.x>=Chunksize||v.y>=WorldHeight||v.z>=Chunksize)
//		{
//			if(v.y>=WorldHeight) goto temp;
//			vct3<ll> tmpv(v.x+c.x*Chunksize,v.y,v.z+c.z*Chunksize),
//				tmpcv=getChunkpos(tmpv);
//			vct3<ull> tmpuv=getposChunk(tmpv);
//			if(wrld.mp.find(std::make_pair(tmpcv.x,tmpcv.z))!=wrld.mp.end())
//			{
//				Chunk& vc=(*wrld.mp[std::make_pair(tmpcv.x,tmpcv.z)]);
//				unsigned& vlgt=vc.b[tmpuv.x][tmpuv.y][tmpuv.z]
//					.light[vfc];
//				if(vlgt<lgt) vlgt=lgt;
//			}
//			goto temp;
//		}
//		if(c.b[v.x][v.y][v.z].light[vfc]>=lgt) goto temp;
//		c.b[v.x][v.y][v.z].light[vfc]=lgt;
//		if(lgt>=2u) q.push_front(std::make_pair(lgt,std::make_pair(v,vfc)));
//		temp:;
//		if(u.x>=Chunksize||u.y>=WorldHeight||u.z>=Chunksize) continue;
//		if(lgt<=trslt[c.b[u.x][u.y][u.z].tp]) continue;
//		unsigned tmplgt=lgt-trslt[c.b[u.x][u.y][u.z].tp];
//		for(ull k(0);k<6;++k)
//		{
//			if(c.b[u.x][u.y][u.z].light[k]>=tmplgt) continue;
//			c.b[u.x][u.y][u.z].light[k]=tmplgt;
//			if(tmplgt>=2u) q.push_back(std::make_pair(tmplgt-1u,std::make_pair(u,k)));
//		}
//	}
//}