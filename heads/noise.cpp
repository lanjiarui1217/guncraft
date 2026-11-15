#include "noise.h"
int noisedefseed;
/*
//初始化全局顶点
fvct2 noise::glbvtx[4]={};
fvct noise::glbvtx3D[3]={};

//初始化全局偏移
fvct2 noise::glboffset=fvct2(0,0);
fvct noise::glboffset3D=fvct(0,0,0);

//初始化全局种子
int noise::glbseed=0x3e3e7f2b;
*/
noise::noise(int seed)
{
	glbseed=seed;
	glbvtx[0]=glbvtx[1]=glbvtx[2]=glbvtx[3]=glboffset=fvct2();
	glbvtx3D[0]=glbvtx3D[1]=glbvtx3D[2]=glbvtx3D[3]=glboffset3D=fvct();
	ofst2=fvct2((unsigned short)(seed^27421),(unsigned short)(seed^-313689));
	ofst3=fvct((unsigned short)(seed^-177193),(unsigned short)(seed^458422),(unsigned short)(seed^-1854789));

}
int noise::hash11(int position)
{
	const unsigned BIT_NOISE1=0x85297A5D;
	const unsigned BIT_NOISE2=0x68E31DA4;
	const unsigned BIT_NOISE3=0x1B56C4E9;
	unsigned mangled=position;
	mangled*=BIT_NOISE1;
	mangled^=(mangled>>8);
	mangled+=BIT_NOISE2;
	mangled^=(mangled<<8);
	mangled*=BIT_NOISE3;
	mangled^=(mangled>>8);
	mangled*=glbseed;
	mangled^=(mangled<<8);
	return mangled%1024;
}
#define hash11glbseed hash11
//int noise::hash11glbseed(int position)
//{
//	const unsigned BIT_NOISE1=0x85297A4D;
//	const unsigned BIT_NOISE2=0x68E31DA4;
//	const unsigned BIT_NOISE3=0x1B56C4E9;
//	unsigned mangled=position^glbseed;
//	mangled*=BIT_NOISE1;
//	mangled^=(mangled>>8);
//	mangled+=BIT_NOISE2;
//	mangled^=(mangled<<8);
//	mangled*=BIT_NOISE3;
//	mangled^=(mangled>>8);
//	return mangled%1024;
//}

fvct2 noise::hash22(fvct2 position2D)
{
	fvct2 v(
		(float)(hash11(0x651A6BE3*(int)position2D.x-(int)position2D.y)%1024),
		(float)(hash11((int)position2D.x*(int)position2D.y+0x218AE247)%1024)
	);
	v/=1024.0f;
	return v;
}

int noise::hash21(fvct2 position2D)
{
	return hash11(0x651A6BE1*(int)position2D.x+(int)position2D.y)%1024;
}

fvct noise::hash33(fvct position)
{
	fvct v(
		(float)(hash11((int)position.x^0x651A6BE3+(int)position.y^0x218A6147-(int)position.z^0x118A5191)%1024),
		(float)(hash11((int)position.x^0x118A5191-(int)position.y^0x218AE247+(int)position.z^0x2B8AE147)%1024),
		(float)(hash11((int)position.x^0x21613122-(int)position.y^0x118A5191-(int)position.z^0x218AE247)%1024)
	);
	v/=1024.0f;
	return v;
}

int noise::hash31(fvct position3D)
{
	return hash11((int)(position3D.x*0x651A6BE6-position3D.y*0xCB251062+position3D.z));
}

void noise::setseed(int seed)
{
	glbseed=seed;
}

int noise::randint(fvct2 position)
{
	position=position*1024;
	return hash11glbseed(0x651A6BE1*(int)position.x+(int)position.y)%1024;
}

float noise::rand(fvct2 position)
{
	position=position*1024;
	return hash11glbseed(0x651A6BE1*(int)position.x+(int)position.y)%1024/1024.0f;
}

float noise::grad(fvct2 vtx,fvct2 position2D)
{
	return vtx.x*position2D.x+vtx.y*position2D.y;
}

float noise::grad_f(fvct2 vtx,fvct2 p)
{
	switch((0x651A6BE1*(int)vtx.x+(int)vtx.y)%4)
	{
	case 1: return  p.x+p.y;  //代表梯度向量(1,1)
	case 2: return -p.x+p.y;  //代表梯度向量(-1,1)
	case 3: return  p.x-p.y;  //代表梯度向量(1,-1)
	case 4: return -p.x-p.y;  //代表梯度向量(-1,-1)
	default: return 0; // never happens
	}
}


float noise::perlinNoise(fvct2 p)
{
	p+=ofst2;
	p=glboffset+p;
	fvct2 w=p;
	//fvct2 w = p * pf * (fvct2(3.0f, 3.0f) - 2.0f * pf);
	fvct2 e00(0,0),e01(0,1.0f),e10(1.0f,0),e11(1.0f,1.0f);
	float tmp=grad(glbvtx[0],p-e00)*(1-w.x)*(1-w.y)+grad(glbvtx[1],p-e10)*w.x*(1-w.y)+grad(glbvtx[2],p-e01)*(1-w.x)*w.y+grad(glbvtx[3],p-e11)*w.x*w.y;
	return tmp<-1.0f?-1.0f:(tmp>1.0f?1.0f:tmp);
}

//TODO
float noise::valueNoise(fvct2 position2D)
{
	position2D+=ofst2;
	fvct2 pi=fvct2(floor(position2D.x),floor(position2D.y));

	fvct2 pf=position2D-pi;
	fvct2 w=(pf.x*pf.x+pf.y*pf.y)*(fvct2(3.0f,3.0f)-2.0f*pf);

	fvct2 vtx[4]={{pi.x,pi.y},{pi.x+1,pi.y},{pi.x,pi.y+1},{pi.x+1,pi.y+1}};

	float tmp=
		(hash21(vtx[0])%17)*(1-w.x)*(1-w.y)+(hash21(vtx[1])%17)*w.x*(1-w.y)+
		(hash21(vtx[2])%17)*(1-w.x)*w.y+(hash21(vtx[3])%17)*w.x*w.y;
	return 0<tmp?(tmp<255?tmp:255):0;
}


float noise::simplexNoise(fvct2 p)
{
	p+=ofst2;
	p=p+glboffset;

	const float K1=0.366025404f; // (sqrt(3)-1)/2;
	const float K2=0.211324865f; // (3-sqrt(3))/6;
	//坐标偏斜
	float s=(p.x+p.y)*K1;
	fvct2 pi=fvct2(floor(p.x+s),floor(p.y+s));
	float t=(pi.x+pi.y)*K2;
	fvct2 pf=p-pi+t*fvct2(1,1);
	fvct2 vtx2offset=(pf.x<pf.y) ? fvct2(0,1) : fvct2(1,0);

	//顶点变换回单行网格空间
	fvct2 dist1=pf;
	fvct2 dist2=pf-vtx2offset+K2*fvct2(1,1);
	fvct2 dist3=pf-fvct2(1,1)+2*K2*fvct2(1,1);

	//计算贡献度取和
	float hx=0.5f-dist1.x*dist1.x-dist1.y*dist1.y;
	float hy=0.5f-dist2.x*dist2.x-dist2.y*dist2.y;
	float hz=0.5f-dist3.x*dist3.x-dist3.y*dist3.y;

	hx*=hx;
	hy*=hy;
	hz*=hz;
	fvct2 t1=hash22(pi),t2=hash22(pi+vtx2offset),t3=hash22(pi+fvct2(1,1));
	float tmp=70*(hx*hx*(dist1.x*t1.x+dist1.y*t1.y)
			+hy*hy*(dist2.x*t2.x+dist2.y*t2.y)
			+hz*hz*(dist3.x*t3.x+dist3.y*t3.y));
	return tmp<-1?-1:(tmp>1?1:tmp);
}

//TODO
float noise::simplexNoise(fvct p)
{
	p+=ofst3;
	auto DotProduct=[](fvct a,fvct b) { return a.x*b.x+a.y*b.y+a.z*b.z; };
	p=p+glboffset3D;

	// Skew the input space to determine which simplex cell we're in
	const float F3=1.0f/3.0f;

	float s=(p.x+p.y+p.z)*F3; // Very nice and simple skew factor for 3D
	fvct pi=fvct(floor(p.x+s),floor(p.y+s),floor(p.z+s));

	const float G3=1.0f/6.0f; // Very nice and simple unskew factor, too
	float t=(pi.x+pi.y+pi.z)*G3;

	// Unskew the cell origin back to (x,y,z) space
	fvct p0=fvct(p.x-pi.x+t,p.y-pi.y+t,p.z-pi.z+t);// The x,y,z distances from the cell origin

	// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
	// Determine which simplex we are in.
	fvct pi1;
	fvct pi2;
	// offsets for second corner of simplex in (i,j,k) coords
	if(p0.x>=p0.y)
	{
		if(p0.y>=p0.z)
		{
			pi1=fvct(1,0,0);
			pi2=fvct(1,1,0);
		} // x y z order
		else if(p0.x>=p0.z)
		{
			pi1=fvct(1,0,0);
			pi2=fvct(1,0,1);
		} // x z y order
		else
		{
			pi1=fvct(0,0,1);
			pi2=fvct(1,0,1);
		} // z x y order
	}
	else
	{ // p0.x<p0.y
		if(p0.y<p0.z)
		{
			pi1=fvct(0,0,1);
			pi2=fvct(0,1,1);
		} // z y x order
		else if(p0.x<p0.z)
		{
			pi1=fvct(0,1,0);
			pi2=fvct(0,1,1);
		} // y z x order
		else
		{
			pi1=fvct(0,1,0);
			pi2=fvct(1,1,0);
		} // y x z order
	}

	// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
	// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
	// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
	// c = 1/6.
	fvct p1=p0-pi1+fvct(1,1,1)*(G3);// offsets for second corner in (x,y,z) coords
	fvct p2=p0-pi2+fvct(1,1,1)*(2.0f*G3);// offsets for third corner in (x,y,z) coords
	fvct p3=p0-fvct(1,1,1)*(-1.0f+3.0f*G3);// offsets for last corner in (x,y,z) coords

	// Calculate the contribution from the four corners
	float t0=0.6f-p0.x*p0.x-p0.y*p0.y-p0.z*p0.z;
	float n0,n1,n2,n3; // Noise contributions from the four corners
	if(t0<0) n0=0.0;
	else
	{
		t0*=t0;
		n0=t0*t0*DotProduct(hash33(pi),p0);
	}
	float t1=0.6f-DotProduct(p1,p1);
	if(t1<0) n1=0.0f;
	else
	{
		t1*=t1;
		n1=t1*t1*DotProduct(hash33(fvct(pi.x+pi1.x,pi.y+pi1.y,pi.z+pi1.z)),p1);
	}
	float t2=0.6f-DotProduct(p2,p2);
	if(t2<0) n2=0.0f;
	else
	{
		t2*=t2;
		n2=t2*t2*DotProduct(hash33(fvct(pi.x+pi2.x,pi.y+pi2.y,pi.z+pi2.z)),p2);
	}
	float t3=0.6f-DotProduct(p3,p3);
	if(t3<0) n3=0.0f;
	else
	{
		t3*=t3;
		n3=t3*t3*DotProduct(hash33(fvct(pi.x+1,pi.y+1,pi.z+1)),p3);
	}
	// Add contributions from each corner to get the final noise value.
	// The result is scaled to stay just inside [-1,1]
	return 32.0f*(n0+n1+n2+n3);
}


void noise::prehandlePerlinNoise(fvct2 position2D,int crystalSize)
{
	fvct2 pi=fvct2(floor(position2D.x/crystalSize),floor(position2D.y/crystalSize));
	fvct2 vtx[4]={{pi.x,pi.y},{pi.x+1,pi.y},{pi.x,pi.y+1},{pi.x+1,pi.y+1}};

	for(int i=0;i<4;++i)
		glbvtx[i]=noise::hash22(vtx[i]);

	glboffset=(position2D-pi*(float)crystalSize)/(float)crystalSize;
}

//TODO
void noise::prehandleValueNoise(fvct2 position2D,int crystalSize)
{

}

void noise::prehandleSimplexNoise(fvct2 position2D,int crystalSize)
{
	glboffset=position2D/(float)crystalSize;
}

void noise::prehandleSimplexNoise(fvct position,int crystalSize)
{
	glboffset3D=position/(float)crystalSize;
}

//二阶bezier曲线 t应[0.0f~1.0f]
fvct2 noise::bezier(fvct2 p0,fvct2 p1,fvct2 p2,float t)
{
	return ((1-t)*(1-t)*p0+t*(1-t)*p1+t*t*p2);
}

//三阶bezier曲线 t应[0.0f~1.0f]
fvct2 noise::bezier(fvct2 p0,fvct2 p1,fvct2 p2,fvct2 p3,float t)
{
	return bezier(p0,p1,p2,t)*(1-t)+bezier(p1,p2,p3,t)*t;
}
/*
//三维坐标转一维坐标（压缩坐标）
ull noise::Index(int x,int y,int z)
{
	const int offset=16384;
	ull t=
		ull(x+offset)<<40|
		ull(y+offset)<<20|
		ull(z+offset);
	return t;
}

//一维坐标转三维坐标（解压坐标）
fvct noise::UnIndex(ull index)
{
	const int offset=16384;
	return fvct(
		int(index>>40)-offset,
		int((index>>20)&0xFFFFF)-offset,
		int(index&0xFFFFF)-offset
	);
}
*/