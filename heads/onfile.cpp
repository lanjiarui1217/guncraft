#include "onfile.h"
static char buf[1000000],* p1=buf,* p2=buf;
static FILE* fin;
#define getchar() (p1==p2&&(p2=(p1=buf)+fread(buf,1,1000000,fin),p1==p2)?EOF:*p1++)
static inline bool isdigit(char ch) { return '0'<=ch&&ch<='9'; }
static inline ll read()
{
	ull x(0),f(1);char ch(getchar());
	while(!isdigit(ch)) { if(ch=='-')f=0; ch=getchar(); }
	while(isdigit(ch)) x=(x<<3)+(x<<1)+(ch^48),ch=getchar();
	return f?x:(~x+1);
}
#include <string>
static std::string nowfn;
// so many bugs!
#include "noise.h"
extern std::map<vct3<ll>,std::vector<std::pair<vct3<ll>,blocktype>>> genleaf;
void read(std::string s)
{
	fin=fopen(s.c_str(),"r");
	p1=p2=buf;
	if(fin==nullptr)
	{
		vp.emplace_back(fvct(0,0,0));
		thisind=0;
		time_t nowtime;
		time(&nowtime);
		tm p;
		localtime_s(&p,&nowtime);
		srand(p.tm_year^p.tm_mon^p.tm_mday^p.tm_hour^p.tm_min^p.tm_sec);
		srand((rand()<<15)|rand());
		noisedefseed=rand()|(rand()<<15)|(rand()<<30);
		return debug("No file!\n");
	}
	nowfn=s;
	if(fgetc(fin)==EOF)
	{
		time_t nowtime;
		time(&nowtime);
		tm p;
		localtime_s(&p,&nowtime);
		srand(p.tm_year^p.tm_mon^p.tm_mday^p.tm_hour^p.tm_min^p.tm_sec);
		srand((rand()<<15)|rand());
		noisedefseed=rand()|(rand()<<15)|(rand()<<30);
		vp.emplace_back(fvct(0,0,0));
		thisind=0;
		return debug("New file!\n");
	}
	ull vpsz,x;
	fscanf(fin,"%d%llu%llu",&noisedefseed,&vpsz,&thisind);
	for(ull i(0);i<vpsz;++i)
	{
		vp.emplace_back();
		Player& p=vp.back();
		fscanf(fin,"%f%f%f%f%f%llu",&p.p.x,&p.p.y,&p.p.z,&p.angle[1],&p.angle[2],&p.chooseind);
		for(ull i(0);i<10;++i) fscanf(fin,"%llu",&x),p.itemlist[i].tp=(itemtype)x,fscanf(fin,"%llu",&x),p.itemlist[i].num=x;
	}
	ull genlfsz;
	fscanf(fin,"%llu",&genlfsz);
	for(ull i(0);i<genlfsz;++i)
	{
		std::pair<vct3<ll>,blocktype> pr;
		fscanf(fin,"%lld%lld%lld%d",&pr.first.x,&pr.first.y,&pr.first.z,&pr.second);
		genleaf[getChunkpos(pr.first)].push_back(pr);
	}
	// entity is too hard
	ull num=read();
	std::vector<ull> tmp;
	std::vector<ull> number;
	number.reserve(2*Chunksize*Chunksize);
	tmp.reserve(2*Chunksize*Chunksize);
	for(ull i(0);i<num;++i)
	{
		ll a=read(),b=read();
		Chunk& c=*(wrld.mp[std::make_pair(a,b)]=new Chunk(a,b));
		ull num2,vl;
		number.resize(0),tmp.resize(0);
		while((num2=read())!=0) vl=read(),number.push_back(num2),tmp.push_back(vl);
		//memset(c.height,-1,sizeof(c.height));
		for(ull i(0),ind(0);i<WorldHeight;++i)
		{
			for(ull j(0);j<Chunksize;++j)
			{
				for(ull k(0);k<Chunksize;++k)
				{
					c.b[j][i][k].tp=tmp[ind];
					//if(c.b[j][i][k].tp!=block::air) c.height[j][k]=i,c.rd.insert(vct3<ll>(j,i,k));
					// No drc! too hard!
					if(number[ind]>1) --number[ind];
					else ++ind;
				}
			}
		}
		c.updvis();
		//wrld.genlight_round(vct3<ll>(c.x*Chunksize+Chunksize/2,0,
		//					c.z*Chunksize+Chunksize/2),
		//					2*Chunksize-Chunksize/2,0);
	}
	fclose(fin);
}
void save()
{
	if(nowfn=="") return;
	fin=fopen(nowfn.c_str(),"w");
	fprintf(fin," %u\n%llu %llu\n",noisedefseed,vp.size(),thisind);
	for(Player& p:vp)
	{
		fprintf(fin,"%f %f %f %f %f %llu ",p.p.x,p.p.y,p.p.z,p.angle[1],p.angle[2],p.chooseind);
		for(ull i(0);i<10;++i) fprintf(fin,"%llu ",(ull)p.itemlist[i].tp),fprintf(fin,"%llu ",p.itemlist[i].num);
	}
	ull genlfsz=0;
	for(auto& pr:genleaf) genlfsz+=pr.second.size();
	fprintf(fin,"\n%llu\n",genlfsz);
	for(auto& pr:genleaf) for(auto& pr2:pr.second) fprintf(fin,"%lld %lld %lld %d\n",pr2.first.x,pr2.first.y,pr2.first.z,(int)pr2.second);
	// entity is too hard
	fprintf(fin,"\n%llu\n",wrld.mp.size());
	std::vector<ull> tmp;
	std::vector<ull> number;
	number.reserve(2*Chunksize*Chunksize);
	tmp.reserve(2*Chunksize*Chunksize);
	for(auto& pr:wrld.mp)
	{
		fprintf(fin,"%lld %lld\n",pr.first.first,pr.first.second);
		Chunk& c=*pr.second;
		std::vector<ull> tmp;
		tmp.reserve(WorldHeight*Chunksize*Chunksize);
		for(ull i(0);i<WorldHeight;++i)
		{
			for(ull j(0);j<Chunksize;++j)
			{
				for(ull k(0);k<Chunksize;++k)
				{
					tmp.push_back((ull)c.b[j][i][k].tp);
				}
			}
		}
		ull num=1;
		for(ull i(1);i<tmp.size();++i)
		{
			if(tmp[i]!=tmp[i-1]) fprintf(fin,"%llu %llu ",num,tmp[i-1]),num=0;
			++num;
		}
		fprintf(fin,"%llu %llu 0\n",num,tmp.back());
	}
	fclose(fin);
}
