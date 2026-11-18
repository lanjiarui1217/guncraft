// #pragma once
#ifndef DIRECT3D_H_LJR
#define DIRECT3D_H_LJR
#include "framework.h"
// #include <d3d9.h>
// #include <d3dx9.h>
// #include <windows.h>
// #include <windowsx.h>
// #include <d3d9types.h>
#include "_math.h"
#include <cassert>
typedef IDirect3DTexture9* Texture;
typedef LPDIRECT3DVERTEXBUFFER9 VtxBuf;
typedef D3DXVECTOR3 fvct;
typedef D3DXVECTOR2 fvct2;
typedef D3DXMATRIX fmat;
constexpr long H=480,W=800;

// ��ά��Ⱦ��
struct Vtx3
{
	struct Vertex
	{
		float x,y,z;
		float u,v;
	};
	static constexpr auto FVF=D3DFVF_XYZ|D3DFVF_TEX1;
	inline Vertex operator()(float x,float y,float z,float u,float v);
};
extern Vtx3 vtx3;

// ��ά��Ⱦ��
struct Vtx2
{
	struct Vertex
	{
		float x,y,z,rhw;
		float u,v;
	};
	static constexpr auto FVF=D3DFVF_XYZRHW|D3DFVF_TEX1;
	inline Vertex operator()(float x,float y,float u,float v);
};
extern Vtx2 vtx2;

// Direct3D9 ��Ⱦ����
extern LPDIRECT3D9 d3d;
extern LPDIRECT3DDEVICE9 d3ddev;
extern LPDIRECT3DVERTEXBUFFER9 vtbf;
extern VOID* pvd;

extern Texture letters,lettersw;
extern Texture graytex;

// Direct3D9 ��ʼ��ģ��
// Warning!!! �ڳ�ʼ������󴴽�
class D3D9IniterDeler
{
	static ull downnum;
public:
	static bool down();
	D3D9IniterDeler();
	~D3D9IniterDeler();
};
// ���ڷ����е�λ��
// Surface Position
enum { ZY1,ZY2,XZ1,XZ2,XY1,XY2 };
typedef int surpos;

// ��Ⱦ��ı��
// render surpos
typedef unsigned char rndsps;

// ��λͼ��Դ [2] �д������� [1]
void SetTexture(Texture&,int);

// ��λͼ��Դ [2] �д������� [1]����͸��ɫ [3]
void SetTexture(Texture&,int,D3DCOLOR);

// ������  [([5],[6]):([7],[8])] ���Ƶ���Ļ [([1],[2]):([3],[4])]
// SetTexture!
void DrawTexture(float,float,float,float,float=0,float=0,float=1,float=1);


#include "../resource.h"
template<typename T>
void Drawtxt(float x0,float y0,float w,float h,const T* ss,size_t len,Texture text=letters)
{
	char s[10000];
	for(ull i(0);i<len;++i) s[i]=((char)ss[i])-' ';
	float xn=x0;
	d3ddev->SetTexture(0,text);
	for(ull i(0);i<len;++i)
	{
		if(s[i]+' '=='\n') y0+=h,xn=x0;
		else
		{
			unsigned x,y;
			y=s[i]/16,x=s[i]%16;
			//printf("%c => %u,%u\n",s[i]+' ',x,y);
			DrawTexture(xn,y0,xn+w,y0+h,(4*x+1)/64.0f,(4*y+1)/32.0f,(4*x+4-1)/64.0f,(4*y+4-1)/32.0f);
			xn+=w;
		}
	}
	//	fflush(stdout);
}

template<typename T>
bool shdren(vct3<T> v)
{
	return true;
}

bool shdren(fvct v);
#endif