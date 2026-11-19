#include "Chunk.h"
#include "block.h"
#include "Player.h"
#include "World.h"

void Chunk::updvis()
{
	clock_t stc=clock();
	rd.clear();
	rd.reserve(Chunksize*Chunksize*2);
	auto mped=wrld.mp.end();
	auto
		iti0=wrld.mp.find(std::make_pair(x-1,z)),iti1=wrld.mp.find(std::make_pair(x+1,z)),
		itk0=wrld.mp.find(std::make_pair(x,z-1)),itk1=wrld.mp.find(std::make_pair(x,z+1));
	bool i0=iti0!=mped,i1=iti1!=mped,k0=itk0!=mped,k1=itk1!=mped;
	for(ull i(0);i<Chunksize;++i) for(ull k(0);k<Chunksize;++k) for(ull j(0);j<WorldHeight;++j) b[i][j][k].vis=0x3f;
	for(ull i(0);i<Chunksize;++i) for(ull k(0);k<Chunksize;++k) for(ull j(0);j<WorldHeight;++j)
	{
		if(i>0				&&occl[b[i-1][j][k].tp][ZY2]) b[i][j][k].vis&=0x3e;//111110
		if(i+1<Chunksize	&&occl[b[i+1][j][k].tp][ZY1]) b[i][j][k].vis&=0x3d;//111101
		if(j>0				&&occl[b[i][j-1][k].tp][XZ2]) b[i][j][k].vis&=0x3b;//111011
		if(j+1<WorldHeight	&&occl[b[i][j+1][k].tp][XZ1]) b[i][j][k].vis&=0x37;//110111
		if(k>0				&&occl[b[i][j][k-1].tp][XY2]) b[i][j][k].vis&=0x2f;//101111
		if(k+1<Chunksize	&&occl[b[i][j][k+1].tp][XY1]) b[i][j][k].vis&=0x1f;//011111
		if(b[i][j][k].vis&&b[i][j][k].tp!=block::air) rd.insert(vct3<ull>(i,j,k));

		vct3<ll> v((ll)i+x*(ll)Chunksize,j,(ll)k+z*(ll)Chunksize);
		if(i0&&i==0				&&occl[wrld.getbl(v-vct3<ll>(1,0,0)).tp][ZY2]) b[i][j][k].vis&=0x3e;
		if(i1&&i+1==Chunksize	&&occl[wrld.getbl(v+vct3<ll>(1,0,0)).tp][ZY1]) b[i][j][k].vis&=0x3d;
		if(k0&&k==0				&&occl[wrld.getbl(v-vct3<ll>(0,0,1)).tp][XY2]) b[i][j][k].vis&=0x2f;
		if(k1&&k+1==Chunksize	&&occl[wrld.getbl(v+vct3<ll>(0,0,1)).tp][XY1]) b[i][j][k].vis&=0x1f;
		if(i0&&i==0				&&occl[b[i][j][k].tp][ZY1]) iti0->second->b[Chunksize-1][j][k].vis&=0x3d;
		if(i1&&i+1==Chunksize	&&occl[b[i][j][k].tp][ZY2]) iti1->second->b[0][j][k].vis&=0x3e;
		if(k0&&k==0				&&occl[b[i][j][k].tp][XY1]) itk0->second->b[i][j][Chunksize-1].vis&=0x1f;
		if(k1&&k+1==Chunksize	&&occl[b[i][j][k].tp][XY2]) itk1->second->b[i][j][0].vis&=0x2f;
		b[i][j][k].pos=v;

		//
		//for(ull p(0);p<6;++p) b[i][j][k].light[p]=15;
		// 
		
		//if(b[i][j][k].tp!=block::glass) continue;
		//debug("%lld,%lld,%lld\n",v.x,v.y,v.z);
		//if(i>0				&&b[i-1][j][k].tp==block::glass) b[i][j][k].vis&=0x3e;//111110
		//if(i+1<Chunksize	&&b[i+1][j][k].tp==block::glass) b[i][j][k].vis&=0x3d;//111101
		//if(j>0				&&b[i][j-1][k].tp==block::glass) b[i][j][k].vis&=0x3b;//111011
		//if(j+1<WorldHeight	&&b[i][j+1][k].tp==block::glass) b[i][j][k].vis&=0x37;//110111
		//if(k>0				&&b[i][j][k-1].tp==block::glass) b[i][j][k].vis&=0x2f;//101111
		//if(k+1<Chunksize	&&b[i][j][k+1].tp==block::glass) b[i][j][k].vis&=0x1f;//011111

		//if(i0&&i==0				&&wrld.getbl(v-vct3<ll>(1,0,0)).tp==block::glass) b[i][j][k].vis&=0x3e,iti0->second->b[Chunksize-1][j][k].vis&=0x3d;
		//if(i1&&i+1==Chunksize	&&wrld.getbl(v+vct3<ll>(1,0,0)).tp==block::glass) b[i][j][k].vis&=0x3d,iti1->second->b[0][j][k].vis&=0x3e;
		//if(k0&&k==0				&&wrld.getbl(v-vct3<ll>(0,0,1)).tp==block::glass) b[i][j][k].vis&=0x2f,itk0->second->b[i][j][Chunksize-1].vis&=0x1f;
		//if(k1&&k+1==Chunksize	&&wrld.getbl(v+vct3<ll>(0,0,1)).tp==block::glass) b[i][j][k].vis&=0x1f,itk1->second->b[i][j][0].vis&=0x2f;
	}
	debug("Chunk::updvis() %lf\n",(clock()-stc)*1.0/CLOCKS_PER_SEC);
}
void Chunk::render(rndsps dr)
{
	static std::vector<vct3<ull>> del;
	del.resize(0);
	for(const vct3<ull>& v:rd)
	{
		if(len(fvct((float)(ll)(v.x+x*Chunksize),tspl.cmp.y,(float)(ll)(v.z+z*Chunksize))-tspl.cmp)>Rendersize) continue;
		const vct3<ull> ex(1,0,0),ey(0,1,0),ez(0,0,1);
		if(!(shdren(v)||shdren(v+ex)||shdren(v+ey)||shdren(v+ez)||shdren(v+ex+ey)||shdren(v+ex+ez)||shdren(v+ey+ez)||shdren(v+ex+ey+ez))) continue;

		float tx=(float)(ll)(v.x+x*Chunksize),tz=(float)(ll)(v.z+z*Chunksize);
		rndsps tmp=0x3f;
		if(tspl.cmp.x>tx+1) tmp&=0x3e;
		if(tspl.cmp.x<tx) tmp&=0x3d;
		if(tspl.cmp.z>tz+1) tmp&=0x2f;
		if(tspl.cmp.z<tz) tmp&=0x1f;
		ull i=v.x,j=v.y,k=v.z;
		static ull cnt=0;
		if(b[i][j][k].tp==block::air||!b[i][j][k].vis) del.push_back(vct3<ull>(i,j,k));
		else BlockDrawUtil::draw(b[i][j][k],b[i][j][k].vis&dr&tmp);
	}
	for(const auto& v:del) rd.erase(v);
} 