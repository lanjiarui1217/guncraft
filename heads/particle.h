#pragma once
#include "framework.h"
struct d3dParticle
{
	static void draw(particle,fvct);
};
class ParticleIniterDeler
{
	static ull downnum;
public:
	static bool down();
	ParticleIniterDeler();
	~ParticleIniterDeler();
};
#include "block.h"
struct Particle
{
	particle tp;
	fvct p,v,a;
	unsigned othinfo;
	Particle(particle tp,fvct p,fvct v,fvct a,unsigned othinfo) : tp(tp),p(p),v(v),a(a),othinfo(othinfo) {}
	void render()
	{
		// 单色阶段暂不画粒子
	}
	void upd() { v+=a/(float)tick,p+=v/(float)tick; }
	bool exist();
};
extern std::vector<Particle> vptc;