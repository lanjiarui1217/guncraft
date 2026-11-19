#include "heads/4.h"
#include "heads/framework.h"
// #include <basetsd.h>
// #include <minwindef.h>
// #include <windef.h>
// #include <libloaderapi.h>
// #include <winnt.h>
// #include <WinUser.h>
#include "resource.h"
#include "heads/direct3d.h"
// #include "heads/windowhelper.h"
#include "heads/block.h"
#include "heads/World.h"
#include "heads/Entity.h"
#include "heads/Player.h"
#include "heads/Generate.h"
#include "heads/particle.h"
#include "heads/item.h"
#include "heads/mouse.h"
#include "heads/keyboard.h"
#include "heads/render.h"
#include <sys/ioctl.h>
#include <unistd.h>
float fps,tps;
const ull MAX_LOADSTRING=100;
// HWND hwnd;
// HINSTANCE hinst;
bool ESC=1,gameover=0;
// TCHAR WndTitle[MAX_LOADSTRING];
// TCHAR WndName[MAX_LOADSTRING];

// LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
// INT_PTR CALLBACK About(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);

static ull drawnum=0,ticknum=0;

// void upd_tick(HWND,UINT,UINT_PTR,DWORD)
void upd_tick()
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
	if(keyboard.capslock()) tspl.updatepos(5.612f,1.0f/tps);
	else tspl.updatepos(4.317f,1.0f/tps);
	// 3.7
}

std::pair<ull,ull> getsize()
{
	winsize w;
	if(ioctl(STDOUT_FILENO,TIOCGWINSZ,&w)==0) return {w.ws_row,w.ws_col};
	else perror("ioctl");
}

static volatile std::sig_atomic_t g_stop=0;
extern "C" void onSigInt(int) { g_stop=1; }

#include "heads/noise.h"
#include <ctime>
#include "heads/onfile.h"

int paint()
{
	if(!BlockIniterDeler::down()) return 0;

	// 5f8bc2
	RGB c={nowskylight*0x5f*255/194/16,nowskylight*0x8b*255/194/16,nowskylight*0xc2/194*255/16};
	clearscreen(c);
	// if(!D3D9IniterDeler::down()) return 0;
	// if(d3ddev==nullptr) return 0;
	// PAINTSTRUCT ps;
	// HDC hdc=BeginPaint(hwnd,&ps);
	// ++drawnum;

	// RECT rc;
	// GetClientRect(hwnd,&rc);
	// long w=rc.right,h=rc.bottom;
	// draw-code here!
	// d3ddev->Clear(0,nullptr,D3DCLEAR_TARGET,c,1.0f,0);
	// d3ddev->Clear(0,nullptr,D3DCLEAR_ZBUFFER,c,1.0f,0);
	fvct rnd(0,0,0);
	float ln=1;
	fvct cmp=tspl.cmp+rnd;
	fvct lkt=tspl.lkt+rnd/ln*(ln-15);
	//		float Znear=0.01f,Zfar=1000.0f;// less?
	float tmpfar=std::max(fabs(cmp.y),fabs(WorldHeight*1.0f-cmp.y));
	camera.farClip=sqrt(tmpfar*tmpfar+Rendersize*Rendersize*1.0f)*1.1f+1.0f;

	// d3ddev->SetTransform(D3DTS_VIEW,&mtv);
	// d3ddev->SetTransform(D3DTS_PROJECTION,&mtp1);

	wrld.render();
	Ray r(tspl.cmp,tspl.lkt,1.0f/24,96);
	for(;~r;++r) if(wrld.bxbl(r)) break;
	if(~r)
	{
		vct3<ll> point(floor(fvct(r)));
		fvct pointf=point;
		BlockDrawUtil::drawline(coll[wrld.getbl(point).tp]+pointf);
	}
	// d3ddev->SetTransform(D3DTS_PROJECTION,&mtp2);
	for(Entity* e:ve) e->render();
	for(Particle& ptc:vptc) ptc.render();

	// d3ddev->SetFVF(Vtx2::FVF);
	// char s[1000];
	// sprintf(s,"x=%.1f y=%.1f z=%.1f\nfps=%d,tps=%d\nangle=%f\nlight_(x,y,z)=%u\n",tspl.p.x,tspl.p.y,tspl.p.z,
	// 	(int)fps,(int)tps,tspl.angle[2],glight(floor(tspl.p),ZY2));
	// Drawtxt(0,0,8,16,s,strlen(s));

	// SetTexture(texmidcrs,IDB_GRAY);
	float x=screenw/2.0f,y=screenh/2.0f;
	float dif1=10;
	RGB gray({0x80,0x80,0x80});
	// d3ddev->SetTexture(0,texmidcrs);
	renderline0(fvct((x-dif1),y,0),fvct((x+dif1),(y),0),gray,gray);
	renderline0(fvct((x),(y-dif1),0),fvct((x),(y+dif1),0),gray,gray);
	// DrawTexture((x-dif1),(y-dif2),(x+dif1),(y+dif2),0,0,1,1);
	// DrawTexture((x-dif2),(y-dif1),(x+dif2),(y+dif1),0,0,1,1);
	// sfrls(texmidcrs);

	ItemDrawUtil::draw();

	// d3ddev->EndScene();
	// d3ddev->Present(nullptr,nullptr,nullptr,nullptr);
	// EndPaint(hwnd,&ps);
}

int main()
{
    signal(SIGINT,onSigInt);
    signal(SIGTERM,onSigInt);
	RenderEnv renderenvobj;
	MouseEnv mouseenvobj;
	KeyboardEnv keyboardenvobj;

	vp.emplace_back(fvct(0,0,0));// [-20000,20000]
	thisind=vp.size()-1;
	tspl.p.y=200;
	time_t nowtime;
	nowtime=time(NULL);
	tm p;
	p=*localtime(&nowtime);
	
	srand(p.tm_year^p.tm_mon^p.tm_mday^(p.tm_hour*60+p.tm_min*60+p.tm_sec));
	//srand((rand()<<15)|rand());
	noisedefseed=rand()|(rand()<<15)|(rand()<<30);

	BlockIniterDeler initblock;
	ParticleIniterDeler initparticle;
	ItemIniterDeler initItem;
	wrld.update(true);

	static clock_t st;
	st=clock();
	clock_t st2=clock();


	const struct timespec tick { 0,2*1000*1000 };
	while(!g_stop)
	{
		pollmouse();
		pollkeyboard();
		camera.fov=45;
		if(keyboard.keys[KEY_C]) camera.fov=15;
		if(!ESC)
		{
			if(mouse.left) if(!ESC) wrld.pick(tspl);
			if(mouse.right) if(!ESC) wrld.use(tspl);
		}
		if(mouse.dx||mouse.dy)
		{
			movecamera(camera,mouse.dx,mouse.dy*2);
		}
		if(keyboard.keys[KEY_UP]&&nowskylight<15u) ++nowskylight;
		if(keyboard.keys[KEY_DOWN]&&nowskylight) --nowskylight;
		for(ull i(1);i<=9;++i)
		{
			ull keyid=KEY_1+(i-1);
			if(keyboard.keys[keyid])
				tspl.chooseind=i-1,++tspl.itemlist[tspl.chooseind].num;
		}
		ll k=tspl.chooseind;
		k-=mouse.wheel;
		k%=10; k+=10; k%=10;
		tspl.chooseind=k;
		// rendertriangle(fvct(1,0,0),fvct(0,1,0),fvct(0,0,1),{0,0,255},{255,0,0},{0,255,0});
		paint();
		render();
		nanosleep(&tick,nullptr);
	}
	// save();
	// gameover=1;
	// return (int)msg.wParam;
	return 0;
}