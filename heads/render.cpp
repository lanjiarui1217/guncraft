#include "render.h"
#include <termios.h>

static termios oldterm;
static bool term_saved=false;

static void enableInput()
{
	if(tcgetattr(STDIN_FILENO,&oldterm)==-1)
	{
		perror("tcgetattr");
		return;
	}

	term_saved=true;

	termios t=oldterm;

	cfmakeraw(&t);

	/*
	 * 不阻塞 read()
	 */
	t.c_cc[VMIN]=0;
	t.c_cc[VTIME]=0;

	if(tcsetattr(
		STDIN_FILENO,
		TCSANOW,
		&t
	)==-1)
	{
		perror("tcsetattr");
	}
}
static void disableInput()
{
	if(term_saved)
	{
		tcsetattr(
			STDIN_FILENO,
			TCSANOW,
			&oldterm
		);

		term_saved=false;
	}
}


RGB screen[screenmaxh][screenmaxw];
ull screenw,screenh;
Camera camera;
RenderEnv::RenderEnv()
{
	enableInput();

	fputs(
		"\033[?25l"
		"\033[?7l",
		stdout
	);

	fflush(stdout);

	winsize w{};

	if(ioctl(STDOUT_FILENO,TIOCGWINSZ,&w)==0)
	{
		screenh=std::min(
			(ull)w.ws_row*2,
			screenmaxh
		);

		screenw=std::min(
			(ull)w.ws_col,
			screenmaxw
		);
	}
	else
	{
		screenh=screenmaxh;
		screenw=screenmaxw;
	}
}

RenderEnv::~RenderEnv()
{
	/*
	 * 先关闭鼠标报告
	 */
	fputs(
		"\033[0m"
		"\033[?7h"
		"\033[?25h",
		stdout
	);

	fflush(stdout);

	/*
	 * 恢复 shell 的输入模式
	 */
	disableInput();

	system("clear");
}


static double zbuffer[screenmaxh][screenmaxw];

void clearscreen(RGB c={0,0,0})
{
	for(ull y=0;y<screenh;++y)
		for(ull x=0;x<screenw;++x)
			zbuffer[y][x]=camera.farClip,
			screen[y][x]=c;
}

/* AIGC start */
// 相机基向量（内部辅助）：返回 forward/right/up
static bool cameraBasis(const Camera& cm,fvct& f,fvct& r,fvct& u)
{
	f=cm.target-cm.pos;
	double fl=len(f);
	if(fl<eps) return false;
	f/=fl;
	r=fvct(f.y*cm.up.z-f.z*cm.up.y,
			f.z*cm.up.x-f.x*cm.up.z,
			f.x*cm.up.y-f.y*cm.up.x);
	double rl=len(r);
	if(rl<eps) return false;
	r/=rl;
	u=fvct(r.y*f.z-r.z*f.y,
			r.z*f.x-r.x*f.z,
			r.x*f.y-r.y*f.x);
	return true;
}

fvct pia(const Camera& cm,fvct p)
{
	fvct f=cm.target-cm.pos;

	double fl=len(f);
	if(fl<eps)
		return fvct(NAN,NAN,NAN);

	f/=fl;

	// right = forward × up
	fvct r(
		f.y*cm.up.z-f.z*cm.up.y,
		f.z*cm.up.x-f.x*cm.up.z,
		f.x*cm.up.y-f.y*cm.up.x
	);

	double rl=len(r);
	if(rl<eps)
		return fvct(NAN,NAN,NAN);

	r/=rl;

	// 真正的 up
	fvct u(
		r.y*f.z-r.z*f.y,
		r.z*f.x-r.x*f.z,
		r.x*f.y-r.y*f.x
	);

	fvct d=p-cm.pos;

	double x=d.x*r.x+
		d.y*r.y+
		d.z*r.z;

	double y=d.x*u.x+
		d.y*u.y+
		d.z*u.z;

	double z=d.x*f.x+
		d.y*f.y+
		d.z*f.z;

	if(z<cm.nearClip||z>cm.farClip)
		return fvct(NAN,NAN,NAN);
	// 防止 z 过小导致除以接近 0，屏幕坐标爆炸/残影
	if(z<0.05) z=0.05;

	double aspect=
		(double)screenw/(double)screenh;

	double rad=
		cm.fov*3.14159265358979323846/180.0;

	double q=1.0/tan(rad*0.5);

	double nx=(x*q/aspect)/z;
	double ny=(y*q)/z;

	double sx=
		(nx+1.0)*0.5*(double)screenw;

	double sy=
		(1.0-ny)*0.5*(double)screenh;

	return fvct(sx,sy,z);
}

static inline RGB lerpRGB(const RGB& a,const RGB& b,double t)
{
	t=std::clamp(t,0.0,1.0);

	return RGB{
		(uint8_t)std::clamp(
			a.r+(b.r-a.r)*t,0.0,255.0),
			(uint8_t)std::clamp(
				a.g+(b.g-a.g)*t,0.0,255.0),
			(uint8_t)std::clamp(
				a.b+(b.b-a.b)*t,0.0,255.0)
	};
}

void renderline0(fvct p1,fvct p2,RGB c1,RGB c2)
{
	if(std::isnan(p1.x)||
		std::isnan(p2.x))
		return;

	ll x1=(ll)std::round(p1.x);
	ll y1=(ll)std::round(p1.y);

	ll x2=(ll)std::round(p2.x);
	ll y2=(ll)std::round(p2.y);

	ll dx=std::abs(x2-x1);
	ll dy=std::abs(y2-y1);

	ll sx=x1<x2?1:-1;
	ll sy=y1<y2?1:-1;

	ll err=dx-dy;

	ll steps=std::max(dx,dy);
	ll step=0;

	while(true)
	{
		double t=
			steps==0
			?0.0
			:(double)step/(double)steps;

		// interpolate depth
		double z=
			p1.z+(p2.z-p1.z)*t;

		if(x1>=0&&x1<(ll)screenw&&
			y1>=0&&y1<(ll)screenh)
		{
			if(z<zbuffer[y1][x1])
			{
				zbuffer[y1][x1]=z;

				screen[y1][x1]=
					lerpRGB(c1,c2,t);
			}
		}

		if(x1==x2&&y1==y2)
			break;

		ll e2=err*2;

		if(e2>-dy)
		{
			err-=dy;
			x1+=sx;
		}

		if(e2<dx)
		{
			err+=dx;
			y1+=sy;
		}

		++step;
	}
}
void rendertriangle0(fvct p1,fvct p2,fvct p3,RGB c1,RGB c2,RGB c3)
{
	if(std::isnan(p1.x)||
		std::isnan(p2.x)||
		std::isnan(p3.x))
		return;

	double area=
		(p2.x-p1.x)*(p3.y-p1.y)-
		(p3.x-p1.x)*(p2.y-p1.y);

	if(std::abs(area)<eps)
		return;

	double invArea=1.0/area;

	ll xmin=(ll)std::floor(
		std::min({p1.x,p2.x,p3.x}));

	ll xmax=(ll)std::ceil(
		std::max({p1.x,p2.x,p3.x}));

	ll ymin=(ll)std::floor(
		std::min({p1.y,p2.y,p3.y}));

	ll ymax=(ll)std::ceil(
		std::max({p1.y,p2.y,p3.y}));

	xmin=std::max<ll>(xmin,0);
	xmax=std::min<ll>(xmax,(ll)screenw-1);

	ymin=std::max<ll>(ymin,0);
	ymax=std::min<ll>(ymax,(ll)screenh-1);


	for(ll y=ymin;y<=ymax;++y)
	{
		for(ll x=xmin;x<=xmax;++x)
		{
			double px=(double)x+0.5;
			double py=(double)y+0.5;

			double w1=
				(
					(p2.x-px)*(p3.y-py)-
					(p3.x-px)*(p2.y-py)
					)*invArea;

			double w2=
				(
					(p3.x-px)*(p1.y-py)-
					(p1.x-px)*(p3.y-py)
					)*invArea;

			double w3=1.0-w1-w2;

			// outside triangle
			if(w1<-eps||
				w2<-eps||
				w3<-eps)
				continue;


			/*
			 * Z-buffer
			 *
			 * Smaller z = closer to camera.
			 */
			double z=
				w1*p1.z+
				w2*p2.z+
				w3*p3.z;

			if(z>=zbuffer[y][x])
				continue;


			zbuffer[y][x]=z;


			/*
			 * Vertex color interpolation
			 */
			double r=
				w1*c1.r+
				w2*c2.r+
				w3*c3.r;

			double g=
				w1*c1.g+
				w2*c2.g+
				w3*c3.g;

			double b=
				w1*c1.b+
				w2*c2.b+
				w3*c3.b;


			screen[y][x]=RGB{
				(uint8_t)std::clamp(r,0.0,255.0),
				(uint8_t)std::clamp(g,0.0,255.0),
				(uint8_t)std::clamp(b,0.0,255.0)
			};
		}
	}
}
void renderline(fvct p1,fvct p2,RGB c1,RGB c2) { renderline0(pia(camera,p1),pia(camera,p2),c1,c2); }
// 在相机空间对 z=nearClip 平面做裁剪，避免近平面穿插时整个三角形消失或投影爆炸
static void clipNear(const fvct& p1,const fvct& p2,const fvct& p3,
                     const RGB& c1,const RGB& c2,const RGB& c3,
                     const fvct& f,const fvct& r,const fvct& u,const Camera& cm,
                     fvct out[4],RGB cout[4],int& nout)
{
	fvct pts[3]={p1,p2,p3};
	RGB  cols[3]={c1,c2,c3};
	double zz[3];
	for(int i(0);i<3;++i){ fvct d=pts[i]-cm.pos; zz[i]=d.x*f.x+d.y*f.y+d.z*f.z; }
	double np=cm.nearClip<0.05?0.05:cm.nearClip;
	fvct po[4]; RGB co[4]; int no=0;
	for(int i(0);i<3;++i)
	{
		int j=(i+1)%3;
		bool inI=zz[i]>=np,inJ=zz[j]>=np;
		if(inI){ po[no]=pts[i]; co[no]=cols[i]; ++no; }
		if(inI!=inJ)
		{
			double t=(np-zz[i])/(zz[j]-zz[i]);
			fvct pi(pts[i]+(pts[j]-pts[i])*t);
			RGB ci{(uint8_t)(cols[i].r+(cols[j].r-cols[i].r)*t),
			       (uint8_t)(cols[i].g+(cols[j].g-cols[i].g)*t),
			       (uint8_t)(cols[i].b+(cols[j].b-cols[i].b)*t)};
			po[no]=pi; co[no]=ci; ++no;
		}
	}
	nout=no;
	for(int i(0);i<no;++i){ out[i]=po[i]; cout[i]=co[i]; }
}
void rendertriangle(fvct p1,fvct p2,fvct p3,RGB c1,RGB c2,RGB c3)
{
	fvct f,r,u;
	if(!cameraBasis(camera,f,r,u)) return;
	fvct out[4]; RGB co[4]; int nout=0;
	clipNear(p1,p2,p3,c1,c2,c3,f,r,u,camera,out,co,nout);
	if(nout==3) rendertriangle0(pia(camera,out[0]),pia(camera,out[1]),pia(camera,out[2]),co[0],co[1],co[2]);
	else if(nout==4)
	{
		rendertriangle0(pia(camera,out[0]),pia(camera,out[1]),pia(camera,out[2]),co[0],co[1],co[2]);
		rendertriangle0(pia(camera,out[0]),pia(camera,out[2]),pia(camera,out[3]),co[0],co[2],co[3]);
	}
}

void movecamera(Camera& cm,int dx,int dy)
{
	const double sensitivity=0.003;

	fvct d=cm.target-cm.pos;

	double yaw=atan2(d.z,d.x);

	double horizontal=
		sqrt(
			d.x*d.x+
			d.z*d.z
		);

	double pitch=
		atan2(d.y,horizontal);

	yaw+=dx*sensitivity;
	pitch-=dy*sensitivity;

	pitch=std::clamp(
		pitch,
		-1.55,
		1.55
	);

	double cp=cos(pitch);

	cm.target=cm.pos+fvct(
		cp*cos(yaw),
		sin(pitch),
		cp*sin(yaw)
	);
}

/* AIGC end */


void render(ull h,ull w,RGB in[screenmaxh][screenmaxw])
{
	// ---- 跨帧复用缓冲 ----
	static std::string buf;
	static size_t lastRows=0;

	buf.clear();
	buf.reserve(w*((h+1)/2)*32+64); // 首次分配, 之后不再 malloc

	// 颜色缓存(每帧重置, 避免外部改过终端状态)
	int lastFR=-1,lastFG=-1,lastFB=-1;
	int lastBR=-1,lastBG=-1,lastBB=-1;

	auto appendUInt=[](std::string& s,unsigned v)
	{
		char tmp[3];
		int n=0;
		if(!v)
		{
			s.push_back('0');
			return;
		}
		while(v)
		{
			tmp[n++]=char('0'+v%10);
			v/=10;
		}
		while(n)
			s.push_back(tmp[--n]);
	};

	size_t rows=(h+1)/2;

	buf.append("\033[H",3); // 回左上角

	// 上一帧更高 -> 清掉多出来的旧行
	if(lastRows>rows)
	{
		buf.append("\033[",2);
		appendUInt(buf,(unsigned)(rows+1));
		buf.append(";1H\033[J",6);
	}
	lastRows=rows;

	for(size_t cy=0; cy<rows; ++cy)
	{
		// 行定位(第 0 行已被 \033[H 定位)
		if(cy>0)
		{
			buf.append("\033[",2);
			appendUInt(buf,(unsigned)(cy+1));
			buf.append(";1H",3);
		}

		size_t y0=cy*2;
		const RGB* rowT=in[y0];
		const RGB* rowB=(y0+1<h)?in[y0+1]:in[y0];

		for(size_t x=0; x<w; ++x)
		{
			RGB t=rowT[x];
			RGB b=rowB[x];

			int dr=(int)t.r-(int)b.r;
			int dg=(int)t.g-(int)b.g;
			int db=(int)t.b-(int)b.b;

			if(dr*dr+dg*dg+db*db<=27)
			{
				// 上下近似同色 -> 空格 + 背景色 (省一半字节)
				if((int)t.r!=lastBR||(int)t.g!=lastBG||(int)t.b!=lastBB)
				{
					buf.append("\033[48;2;",7);
					appendUInt(buf,t.r);
					buf.push_back(';');
					appendUInt(buf,t.g);
					buf.push_back(';');
					appendUInt(buf,t.b);
					buf.push_back('m');
					lastBR=t.r;
					lastBG=t.g;
					lastBB=t.b;
				}
				buf.push_back(' ');
			}
			else
			{
				// 前景 = 上半
				if((int)t.r!=lastFR||(int)t.g!=lastFG||(int)t.b!=lastFB)
				{
					buf.append("\033[38;2;",7);
					appendUInt(buf,t.r);
					buf.push_back(';');
					appendUInt(buf,t.g);
					buf.push_back(';');
					appendUInt(buf,t.b);
					buf.push_back('m');
					lastFR=t.r;
					lastFG=t.g;
					lastFB=t.b;
				}
				// 背景 = 下半
				if((int)b.r!=lastBR||(int)b.g!=lastBG||(int)b.b!=lastBB)
				{
					buf.append("\033[48;2;",7);
					appendUInt(buf,b.r);
					buf.push_back(';');
					appendUInt(buf,b.g);
					buf.push_back(';');
					appendUInt(buf,b.b);
					buf.push_back('m');
					lastBR=b.r;
					lastBG=b.g;
					lastBB=b.b;
				}
				buf.append("\xE2\x96\x80",3); // ▀
			}
		}
		// 行末不重置 SGR: 颜色缓存跨行继续命中
	}

	// 一次系统调用写出(处理部分写与 EINTR)
	const char* p=buf.data();
	size_t left=buf.size();
	while(left)
	{
		ssize_t n=::write(STDOUT_FILENO,p,left);
		if(n>0)
		{
			p+=n;
			left-=(size_t)n;
		}
		else if(n<0&&errno==EINTR)
			continue;
		else
			break;
	}
}