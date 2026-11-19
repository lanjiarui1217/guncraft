#include "particle.h"
#include "World.h"
#include "Player.h"

ull ParticleIniterDeler::downnum=0;
std::vector<Particle> vptc;

void d3dParticle::draw(particle tp,fvct p)
{
	// 先不画粒子（单色阶段）
	(void)tp; (void)p;
}

bool ParticleIniterDeler::down()
{
	return downnum>0;
}
ParticleIniterDeler::ParticleIniterDeler()
{
	if(downnum)
	{
		++downnum;
		return;
	}
	++downnum;
}
ParticleIniterDeler::~ParticleIniterDeler()
{
	if(downnum) --downnum;
}

bool Particle::exist()
{
	switch(tp)
	{
	case particletype::block:
		return (!wrld.bxbl(p))&&p.y>=-1000.0f;
	case particletype::rain:
		return (!wrld.bxbl(p))&&p.y>=-1000.0f;
	}
	return false;
}
