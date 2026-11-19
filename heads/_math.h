#ifndef _MATH_H_LJR
#define _MATH_H_LJR
#include <bits/stdc++.h>
typedef unsigned long long ull;
typedef long long ll;


template<typename T>
struct vct3
{
	typedef vct3<T> vct;
	T x,y,z;
	inline vct3() noexcept=default;
	inline vct3(const T*t) noexcept { x=t[1],y=t[2],z=t[3]; }
	inline vct3(const vct&) noexcept=default;
	template<typename Tp>
	inline vct3(const vct3<Tp>& v) noexcept { x=v[1],y=v[2],z=v[3]; }
	inline vct3(const T& x,const T& y,const T& z) noexcept
		: x(x),y(y),z(z)
	{}
	// inline operator fvct() noexcept { return fvct((float)x,(float)y,(float)z); }
	inline T& operator[](ull ind) noexcept { return ind==1?x:(ind==2?y:z); }
	inline const T& operator[](ull ind) const noexcept { return ind==1?x:(ind==2?y:z); }
	inline vct& operator+=(const vct& b) noexcept { return x+=b.x,y+=b.y,z+=b.z,*this; }
	inline vct& operator-=(const vct& b) noexcept { return x-=b.x,y-=b.y,z-=b.z,*this; }
	template<typename Tp>
	inline vct& operator*=(Tp b) noexcept { return x*=b,y*=b,z*=b,*this; }
	template<typename Tp>
	inline vct& operator/=(Tp b) noexcept { return x/=b,y/=b,z/=b,*this; }
	inline vct operator+() const noexcept { return *this; }
	inline vct operator-() const noexcept { return vct3(-x,-y,-z); }
	inline vct operator+(const vct& b) const noexcept { return vct3(x+b.x,y+b.y,z+b.z); }
	inline vct operator-(const vct& b) const noexcept { return vct3(x-b.x,y-b.y,z-b.z); }
	template<typename Tp>
	inline vct operator*(Tp b) const noexcept { return vct3(x*b,y*b,z*b); }
	template<typename Tp>
	inline vct operator/(Tp b) const noexcept { return vct3(x/b,y/b,z/b); }
	template<typename Tp>
	inline friend vct operator*(const Tp&b,const vct& a) noexcept { return vct3(a.x*b,a.y*b,a.z*b); }
	inline bool operator==(const vct& b) const noexcept { return x==b.x&&y==b.y&&z==b.z; }
	inline bool operator!=(const vct& b) const noexcept { return x!=b.x||y!=b.y||z!=b.z; }

	inline bool operator<(const vct& b) const noexcept
	{
		if(x!=b.x) return x<b.x;
		if(y!=b.y) return y<b.y;
		return z<b.z;
	}
};

template<typename T>
struct vct2
{
	typedef vct2<T> vct;
	T x,y;
	inline vct2() noexcept=default;
	inline vct2(const T& x,const T& y) noexcept : x(x),y(y) {}
	inline T& operator[](ull ind) noexcept { return ind==1?x:y; }
	inline const T& operator[](ull ind) const noexcept { return ind==1?x:y; }
	inline vct& operator+=(const vct& b) noexcept { return x+=b.x,y+=b.y,*this; }
	inline vct& operator-=(const vct& b) noexcept { return x-=b.x,y-=b.y,*this; }
	inline vct& operator*=(T b) noexcept { return x*=b,y*=b,*this; }
	inline vct& operator/=(T b) noexcept { return x/=b,y/=b,*this; }
	inline vct operator+() const noexcept { return *this; }
	inline vct operator-() const noexcept { return vct2(-x,-y); }
	inline vct operator+(const vct& b) const noexcept { return vct2(x+b.x,y+b.y); }
	inline vct operator-(const vct& b) const noexcept { return vct2(x-b.x,y-b.y); }
	inline vct operator*(T b) const noexcept { return vct2(x*b,y*b); }
	inline vct operator/(T b) const noexcept { return vct2(x/b,y/b); }
	inline friend vct operator*(const T&b,const vct& a) noexcept { return vct2(a.x*b,a.y*b); }
	inline bool operator==(const vct& b) const noexcept { return x==b.x&&y==b.y; }
	inline bool operator!=(const vct& b) const noexcept { return x!=b.x||y!=b.y; }
	inline bool operator<(const vct& b) const noexcept
	{
		if(x!=b.x) return x<b.x;
		return y<b.y;
	}
};

typedef vct2<double> fvct2;

typedef vct3<double> fvct;
// typedef D3DXMATRIX fmtx;
constexpr float eps=1e-8f;
inline float len(fvct v) { return sqrtf(v.x*v.x+v.y*v.y+v.z*v.z); }



template<typename T>
static inline T getmaxofbox() { return 1; }

// AABB ��ײ��
struct Box
{
	fvct min,max;
	Box()
	{
		min=fvct(0,0,0);
		max=fvct(1,1,1);
	}
	Box(const fvct& minn,const fvct& maxx) { min=minn,max=maxx; }
	inline operator bool() const
	{
		return min.x<max.x&&min.y<max.y&&min.z<max.z;
	}
	inline bool operator[](int ind) const
	{
		switch(ind)
		{
		case 0: case 1: return fabs((max.y-min.y)-1)<1e-6&&fabs((max.z-min.z)-1)<1e-6;
		case 2: case 3: return fabs((max.x-min.x)-1)<1e-6&&fabs((max.z-min.z)-1)<1e-6;
		case 4: case 5: return fabs((max.x-min.x)-1)<1e-6&&fabs((max.y-min.y)-1)<1e-6;
		}
		return 0;
	}
};

Box operator&(const Box&,const Box&);
bool operator&(const Box&,const fvct&);
Box operator+(const Box&,const fvct&);

struct Ray
{
	fvct frm,e;
	ull stp,now;
	float l;
	Ray(fvct,fvct,float,ull=100,ull=0);
	inline operator fvct() const;
	inline Ray& operator++();
	inline Ray operator+(ull);
	inline bool operator~();
};
inline Ray::operator fvct() const { return frm+e*((float)(now)*l); }
inline Ray& Ray::operator++() { return ++now,* this; }
inline Ray Ray::operator+(ull k) { return Ray(frm,frm+e,l,stp,now+k); }
inline bool Ray::operator~() { return now<=stp; }


struct pllhash
{
	ull operator()(std::pair<ll,ll> c) const;
};
inline ull pllhash::operator()(std::pair<ll,ll> c) const
{
	return (((ull)((unsigned)c.first))<<32)|((unsigned)c.second);
}

template<typename T>
struct vct3hash
{
	ull operator()(vct3<T> c) const;
};
template<typename T>
inline ull vct3hash<T>::operator()(vct3<T> v) const
{
	return (std::hash<ull>()(v.x)^
			(std::hash<ull>()(v.y)<<1)^
			(std::hash<ull>()(v.z)<<2));

}

vct3<ll> floor(const fvct&);
#endif