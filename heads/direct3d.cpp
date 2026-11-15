#pragma warning(push)
#pragma warning(disable:4005)
#include "direct3d.h"
#include <d3d9.h>
#include <winnt.h>
#include <windef.h>
#include <WinUser.h>
#include <d3d9types.h>
#include "framework.h"
#include "../Resource.h"
#pragma warning(pop)
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
LPDIRECT3D9 d3d=nullptr;
LPDIRECT3DDEVICE9 d3ddev=nullptr;
LPDIRECT3DVERTEXBUFFER9 vtbf=nullptr;
VOID* pvd=nullptr;
extern HWND hwnd;
Texture letters,lettersw,graytex;

Vtx3::Vertex Vtx3::operator()(float x,float y,float z,float u,float v)
{
	static Vertex vt;
	vt.x=x,vt.y=y,vt.z=z,vt.u=u,vt.v=v;
	return vt;
}
Vtx2::Vertex Vtx2::operator()(float x,float y,float u,float v)
{
	static Vertex vt;
	vt.x=x,vt.y=y,vt.z=0,vt.rhw=1,vt.u=u,vt.v=v;
	return vt;
}

Vtx3 vtx3;
Vtx2 vtx2;

ull D3D9IniterDeler::downnum=0;
bool D3D9IniterDeler::down()
{
	return downnum>0;
}
D3D9IniterDeler::D3D9IniterDeler()
{
#ifdef DBG
	MessageBox(hwnd,L"d3d9init",L"init",MB_OK);
#endif
	if(downnum)
	{
		++downnum;
		return;
	}
	assert(hwnd!=nullptr);
	d3d=Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp,sizeof(d3dpp));
	d3dpp.Windowed=true;
	d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow=hwnd;
	d3dpp.BackBufferFormat=D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth=W;
	d3dpp.BackBufferHeight=H;
	d3dpp.EnableAutoDepthStencil=true;
	d3dpp.AutoDepthStencilFormat=D3DFMT_D16;
	d3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hwnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&d3ddev);
	d3ddev->SetRenderState(D3DRS_ALPHATESTENABLE,TRUE);
	d3ddev->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
	d3ddev->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	d3ddev->SetRenderState(D3DRS_LIGHTING,FALSE);
	d3ddev->SetRenderState(D3DRS_ZENABLE,true);
	//d3ddev->SetRenderState(D3DRS_ZENABLE,false);
	d3ddev->SetRenderState(D3DRS_ZWRITEENABLE,true);
	//d3ddev->SetRenderState(D3DRS_ZWRITEENABLE,false);
	d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	d3ddev->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	d3ddev->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	d3ddev->SetRenderState(D3DRS_ALPHAREF,0);
	d3ddev->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);

	d3ddev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
	d3ddev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	d3ddev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	d3ddev->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_MODULATE);
	d3ddev->SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	d3ddev->SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);
	d3ddev->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
	++downnum;
	SetTexture(letters,IDB_SIGN);
	SetTexture(lettersw,IDB_SIGNW);
	SetTexture(graytex,IDB_GRAY);
}
D3D9IniterDeler::~D3D9IniterDeler()
{
	--downnum;
	if(downnum) return;
	sfrls(d3d),sfrls(d3ddev);
	sfrls(letters);
	sfrls(graytex);
}

extern HINSTANCE hinst;
void SetTexture(Texture& t,int id)
{
	if(id==0) return t=nullptr,(void)0;
	const ull max_len=100;
	WCHAR s[max_len];
	LoadStringW(hinst,id,s,max_len);
	D3DXCreateTextureFromFile(d3ddev,s,&t);
}

void SetTexture(Texture& t,int id,D3DCOLOR c)
{
	if(id==0) return t=nullptr,(void)0;
	const ull max_len=100;
	WCHAR s[max_len];
	LoadStringW(hinst,id,s,max_len);
	D3DXCreateTextureFromFileEx(d3ddev,s,D3DX_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,0,
								D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,D3DX_DEFAULT,
								D3DX_DEFAULT,c,nullptr,nullptr,&t);
}

void DrawTexture(float x0,float y0,float x1,float y1,
				 float u0,float v0,float u1,float v1)
{
	RECT rc;
	GetClientRect(hwnd,&rc);
	long w=rc.right,h=rc.bottom;
	x0=x0*W/w,x1=x1*W/w,y0=y0*H/h,y1=y1*H/h;
	if(d3ddev==nullptr) return;
	Vtx2::Vertex vts[][3]=
	{
		{vtx2(x0,y0,u0,v0),vtx2(x1,y0,u1,v0),vtx2(x1,y1,u1,v1)},
		{vtx2(x0,y0,u0,v0),vtx2(x1,y1,u1,v1),vtx2(x0,y1,u0,v1)},
	};
	d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2u,vts,sizeof(Vtx2::Vertex));
}

bool shdren(fvct v)
{
	return true;
}


