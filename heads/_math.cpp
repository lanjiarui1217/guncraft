#pragma warning(push)
#pragma warning(disable:4005)
#include "_math.h"
#include "framework.h"
#include <cmath>
#pragma warning(pop)


Box operator&(const Box& a,const Box& b)
{
	auto minn=[](float a,float b) { return a<b?a:b; };
	auto maxx=[](float a,float b) { return a>b?a:b; };
	return Box(fvct(
				maxx(a.min.x,b.min.x),
				maxx(a.min.y,b.min.y),
				maxx(a.min.z,b.min.z)),
			   fvct(
				minn(a.max.x,b.max.x),
				minn(a.max.y,b.max.y),
				minn(a.max.z,b.max.z))
			   );
}

bool operator&(const Box& a,const fvct& b)
{
	return a.min.x<=b.x&&b.x<=a.max.x&&
		a.min.y<=b.y&&b.y<=a.max.y&&
		a.min.z<=b.z&&b.z<=a.max.z;
}

Box operator+(const Box& a,const fvct& b)
{
	return Box(a.min+b,a.max+b);
}

Ray::Ray(fvct a,fvct b,float ln,ull num,ull n)
{ frm=a,e=(b-a)/len(b-a);l=ln;stp=num,now=n; }



vct3<ll> floor(const fvct& p)
{
	return vct3<ll>((ll)floor(p.x),(ll)floor(p.y),(ll)floor(p.z));
}