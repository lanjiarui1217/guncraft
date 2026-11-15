#pragma once
#include "framework.h"
struct d3dParticle
{
	static Texture texture[PARTICLENUM];
	static VtxBuf vertexbuffer[PARTICLENUM];
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
		if(tp!=particletype::block) d3ddev->SetTexture(0,d3dParticle::texture[tp]);
		else d3ddev->SetTexture(0,BlockDrawUtil::texture[othinfo>>3][othinfo&7]);
		d3dParticle::draw(tp,p);
	}
	void upd() { v+=a/(float)tick,p+=v/(float)tick; }
	bool exist();
};
extern std::vector<Particle> vptc;