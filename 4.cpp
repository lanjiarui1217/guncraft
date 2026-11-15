#include "heads/4.h"
#include "heads/framework.h"
#include <basetsd.h>
#include <minwindef.h>
#include <windef.h>
#include <libloaderapi.h>
#include <winnt.h>
#include <WinUser.h>
#include "Resource.h"
#include "heads/direct3d.h"
#include "heads/windowhelper.h"
#include "heads/block.h"
#include "heads/World.h"
#include "heads/Entity.h"
#include "heads/Player.h"
#include "heads/Generate.h"
#include "heads/Particle.h"
#include "heads/item.h"
float fps,tps;
const ull MAX_LOADSTRING=100;
HWND hwnd;
HINSTANCE hinst;
bool ESC=1,gameover=0;
TCHAR WndTitle[MAX_LOADSTRING];
TCHAR WndName[MAX_LOADSTRING];

LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
INT_PTR CALLBACK About(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);

static ull drawnum=0,ticknum=0;

void upd_tick(HWND,UINT,UINT_PTR,DWORD)
{
	static time_t bef=0;
	tps=1.0f*CLOCKS_PER_SEC/(clock()-bef);
	if(tps<eps) tps=eps*2;
	++ticknum;
	bef=clock();
	if(ESC) return;
	for(ull i(0);i<ve.size();)
	{
		ve[i]->upd();
		if(ve[i]->exist) ++i;
		else
		{
			delete ve[i];
			ve.erase(ve.begin()+i);
		}
	}
	for(Particle& ptc:vptc) ptc.upd();
	for(auto it=vptc.begin();it!=vptc.end();)
	{
		if(it->exist()) ++it;
		else it=vptc.erase(it);
	}
	if(GetKeyState(VK_CONTROL)&0x8000) tspl.updatepos(5.612f,1.0f/tps);
	else tspl.updatepos(4.317f,1.0f/tps);
	// 3.7
}

// 主程序的消息处理函数
LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	static constexpr ull TICK_TIMER=0x1400;
	float rad=D3DXToRadian(45);
	if(GetKeyState('C')&0x8000) rad=D3DXToRadian(15);
	// 5f8bc2
	D3DCOLOR c=D3DCOLOR_XRGB(nowskylight*0x5f*255/194/16,nowskylight*0x8b*255/194/16,nowskylight*0xc2/194*255/16);

	static Texture texmidcrs=nullptr;

	switch(msg)
	{
	case WM_CREATE:
	{
		SetTimer(hwnd,TICK_TIMER,(UINT)(1000/tick),&upd_tick);
	}
	return 0;
	case WM_MOUSEMOVE:
	{
		if(ESC) return 0;
		POINT po,p;POINT tmp;
		RECT rc;
		GetWindowRect(hwnd,&rc);GetCursorPos(&tmp);
		po.x=(rc.left+rc.right)/2,po.y=(rc.bottom+rc.top)/2,p.x=tmp.x-po.x,p.y=tmp.y-po.y;
		// 4.0~25.0
		tspl.angle[1]-=p.x/10.0f,tspl.angle[2]-=p.y/10.0f;
		tspl.updatelkt();
		SetCursorPos((int)po.x,(int)po.y);
	}
	return 0;
	case WM_MOUSEWHEEL:
	{
		ll k=tspl.chooseind;
		k-=GET_WHEEL_DELTA_WPARAM(wParam)/120;
		k%=10; k+=10; k%=10;
		tspl.chooseind=k;
	}
	return 0;
	case WM_COMMAND:
	{
		int wmId=LOWORD(wParam);
		switch(wmId)
		{
		case IDM_ABOUT:
			DialogBox(hwnd_hinst_mp[hwnd],MAKEINTRESOURCE(IDD_ABOUTBOX),hwnd,About);
			return 0;
		case IDM_EXIT:
			DestroyWindow(hwnd);
			return 0;
		}
	}
	return 0;
	case WM_PAINT:
	{
		if(!BlockIniterDeler::down()) return 0;
		if(!D3D9IniterDeler::down()) return 0;
		if(d3ddev==nullptr) return 0;
		PAINTSTRUCT ps;
		HDC hdc=BeginPaint(hwnd,&ps);
		++drawnum;

		RECT rc;
		GetClientRect(hwnd,&rc);
		long w=rc.right,h=rc.bottom;
		// draw-code here!
		d3ddev->Clear(0,nullptr,D3DCLEAR_TARGET,c,1.0f,0);
		d3ddev->Clear(0,nullptr,D3DCLEAR_ZBUFFER,c,1.0f,0);
	//	fvct re(rand(),rand(),rand());
	//	fvct re(0,0,0);
	//	fvct cmp=tspl.cmp+re/len(re)*1.0f/16;
	//	fvct lkt=tspl.lkt+re/len(re)*1.0f/8;
		//fvct rnd(rand(),rand(),rand());
		//if(rnd.x==0&&rnd.y==0&&rnd.z==0) rnd.x=1;
		//rnd/=len(rnd);
		//if(tspl.angle[2]>89||tspl.angle[2]<-89) rnd*=0.001f;
		//if(tspl.angle[2]>88||tspl.angle[2]<-88) rnd*=0.0016f;
		//if(tspl.angle[2]>83||tspl.angle[2]<-83) rnd*=0.002f;
		//else rnd*=0.02f;
		//float ln=500.0f;
		//do
		//{
		//	Ray r(tspl.cmp,tspl.lkt,1.0f/24,24*500.0f);
		//	for(;~r;++r) if(wrld.bxbl(r)) break;
		//	if(~r)
		//	{
		//		ln=len((fvct)r-tspl.cmp);
		//	}
		//} while(0);
		fvct rnd(0,0,0);
		float ln=1;
		fvct cmp=tspl.cmp+rnd;
		fvct lkt=tspl.lkt+rnd/ln*(ln-15);
//		float Znear=0.01f,Zfar=1000.0f;// less?
		float tmpfar=max(fabs(cmp.y),fabs(WorldHeight*1.0f-cmp.y));
		float Zfar=sqrt(tmpfar*tmpfar+Rendersize*Rendersize*1.0f)*1.1f+1.0f;
		float Znear=(cmp.y<0.0f?(-cmp.y):(cmp.y>WorldHeight*1.0f?(cmp.y-WorldHeight*1.0f):0.0f))*0.9f-1.0f;
		if(Zfar>1000.0f) Zfar=1000.0f;
		if(Znear<0.07f) Znear=0.03f;
		static fvct up(0.0f,1.0f,0.0f);
		fmtx mtv,mtp1,mtp2;
		D3DXMatrixLookAtLH(&mtv,&cmp,&lkt,&up);
		D3DXMatrixPerspectiveFovLH(&mtp1,rad,rc.right*1.0f/rc.bottom,Znear,Zfar);
		D3DXMatrixPerspectiveFovLH(&mtp2,rad,rc.right*1.0f/rc.bottom,0.03f,1000.0f);

		d3ddev->BeginScene();
		d3ddev->SetFVF(Vtx3::FVF);

		d3ddev->SetTransform(D3DTS_VIEW,&mtv);
		d3ddev->SetTransform(D3DTS_PROJECTION,&mtp1);

		wrld.render();
		Ray r(tspl.cmp,tspl.lkt,1.0f/24,96);
		for(;~r;++r) if(wrld.bxbl(r)) break;
		if(~r)
		{
			vct3<ll> point(floor(fvct(r)));
			fvct pointf=point;
			BlockDrawUtil::drawline(coll[wrld.getbl(point).tp]+pointf);
		}
		d3ddev->SetTransform(D3DTS_PROJECTION,&mtp2);
		for(Entity* e:ve) e->render();
		for(Particle& ptc:vptc) ptc.render();

		d3ddev->SetFVF(Vtx2::FVF);

		char s[1000];
		sprintf(s,"x=%.1f y=%.1f z=%.1f\nfps=%d,tps=%d\nangle=%f\nlight_(x,y,z)=%u\n",tspl.p.x,tspl.p.y,tspl.p.z,
				(int)fps,(int)tps,tspl.angle[2],glight(floor(tspl.p),ZY2));
		Drawtxt(0,0,8,16,s,strlen(s));
		
		SetTexture(texmidcrs,IDB_GRAY);
		float x=w/2.0f,y=h/2.0f;
		float dif1=10,dif2=2;
		d3ddev->SetTexture(0,texmidcrs);
		DrawTexture((x-dif1),(y-dif2),(x+dif1),(y+dif2),0,0,1,1);
		DrawTexture((x-dif2),(y-dif1),(x+dif2),(y+dif1),0,0,1,1);
		sfrls(texmidcrs);

		ItemDrawUtil::draw();

		d3ddev->EndScene();
		d3ddev->Present(nullptr,nullptr,nullptr,nullptr);
		EndPaint(hwnd,&ps);
	}
	return 0;
	case WM_DESTROY:
		KillTimer(hwnd,TICK_TIMER);
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
	{
		if(GetAsyncKeyState(VK_ESCAPE)&0x8000) if(!ESC) ShowCursor(TRUE),ESC=1;
		if(GetKeyState(VK_UP)&0x8000) if(nowskylight<15u) ++nowskylight;
		if(GetKeyState(VK_DOWN)&0x8000) if(nowskylight) --nowskylight;

		for(ull i(1);i<=9;++i)
		{
			if(GetAsyncKeyState((int)i^'0')&0x8000) tspl.chooseind=i-1,++tspl.itemlist[tspl.chooseind].num;
		}
	}
	return 0;
	case WM_LBUTTONDOWN:
	{
		if(ESC)
		{
			ShowCursor(FALSE),ESC=0;
			RECT rc;POINT po;
			GetWindowRect(hwnd,&rc);
			po.x=(rc.left+rc.right)/2,po.y=(rc.bottom+rc.top)/2;
			SetCursorPos((int)po.x,(int)po.y);
		}
	}
	return 0;
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

// “关于”框的消息处理函数。
INT_PTR CALLBACK About(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch(message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if(LOWORD(wParam)==IDOK||LOWORD(wParam)==IDCANCEL)
		{
			EndDialog(hDlg,LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
#include "heads/noise.h"
#include <ctime>
#include "heads/onfile.h"
int APIENTRY wWinMain(HINSTANCE hInstance,
					  HINSTANCE,LPWSTR pcmdline,
					  int nCmdShow)
{
	//ull len=lstrlenW(pcmdline);
	//std::string filename;
	//for(ull i(0);i<len;++i) filename+=(char)pcmdline[i];
	// 

	//read(/*filename*/std::string("tempgcfile2.gcfile"));
	// open when you want to read a file, and you can use the filename from command line
	
	vp.emplace_back(fvct(0,0,0));// [-20000,20000]
	thisind=vp.size()-1;
	tspl.p.y=200;
	time_t nowtime;
	nowtime=time(NULL);
	tm p;
	p=*localtime(&nowtime);
	//MessageBoxA(nullptr,asctime(&p),"Time",MB_OK);
	//debug("%s\n",asctime(&p));
	srand(p.tm_year^p.tm_mon^p.tm_mday^(p.tm_hour*60+p.tm_min*60+p.tm_sec));
	//srand((rand()<<15)|rand());
	noisedefseed=rand()|(rand()<<15)|(rand()<<30);
	// open when you want to open new

	debug("%d\n",noisedefseed);
	hinst=hInstance;
	LoadString(hinst,IDS_APP_TITLE,WndTitle,MAX_LOADSTRING);
	LoadString(hinst,IDC_MY4,WndName,MAX_LOADSTRING);
	MyRegisterClass(hinst,WndName,WndProc);
	if(!InitInstance(hinst,nCmdShow,hwnd,WndName,WndTitle)) return FALSE;
	HACCEL hAccelTable=LoadAccelerators(hinst,MAKEINTRESOURCE(IDC_MY4));
	//初始化
	D3D9IniterDeler initd3d9;
	BlockIniterDeler initblock;
	ParticleIniterDeler initparticle;
	ItemIniterDeler initItem;
	wrld.update(true);
	//wrld.update(true);// testing light-generator in second using
	static clock_t st;
	st=clock();
	clock_t st2=clock();
	MSG msg={};
	while(msg.message!=WM_QUIT)
	{
		while(PeekMessage(&msg,nullptr,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		clock_t del=clock()-st2;st2=clock();
		if(del) fps=1.0f*CLOCKS_PER_SEC/del;
		InvalidateRect(hwnd,nullptr,0);
		if(ESC) continue;
		if(GetAsyncKeyState(VK_LBUTTON)&0x8000) if(!ESC) wrld.pick(tspl);
		if(GetAsyncKeyState(VK_RBUTTON)&0x8000) if(!ESC) wrld.use(tspl);
	}
	fps=drawnum*CLOCKS_PER_SEC*1.0f/(clock()-st);
	tps=ticknum*CLOCKS_PER_SEC*1.0f/(clock()-st);
	if(!ESC) ShowCursor(TRUE),ESC=1;
	debug("\n\nfps=%.3lf,tps=%.3lf\n\n",fps,tps);
	save();
	gameover=1;
	return (int)msg.wParam;
}