#include "mouse.h"

#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <ctype.h>
#include <stdint.h>

Mouse mouse;

static int mousefd=-1;
static bool grabbed=false;
static bool desynced=false;

static bool hasbit(
    const unsigned long* bits,
    int code
)
{
    return bits[code/(sizeof(unsigned long)*8)] &
        (1UL<<(code%(sizeof(unsigned long)*8)));
}

static bool ismouse(int fd)
{
    unsigned long evbits[
        (EV_MAX+sizeof(unsigned long)*8)/
        (sizeof(unsigned long)*8)
    ]{};

    unsigned long relbits[
        (REL_MAX+sizeof(unsigned long)*8)/
        (sizeof(unsigned long)*8)
    ]{};

    unsigned long keybits[
        (KEY_MAX+sizeof(unsigned long)*8)/
        (sizeof(unsigned long)*8)
    ]{};

    if(ioctl(fd,EVIOCGBIT(0,sizeof(evbits)),evbits)<0)
        return false;

    if(!hasbit(evbits,EV_REL))
        return false;

    if(!hasbit(evbits,EV_KEY))
        return false;

    if(ioctl(fd,EVIOCGBIT(EV_REL,sizeof(relbits)),relbits)<0)
        return false;

    if(ioctl(fd,EVIOCGBIT(EV_KEY,sizeof(keybits)),keybits)<0)
        return false;

    return
        hasbit(relbits,REL_X)&&
        hasbit(relbits,REL_Y)&&
        hasbit(keybits,BTN_LEFT);
}

bool initmouse(bool grab)
{
    closemouse();

    DIR* dir=opendir("/dev/input");

    if(!dir)
    {
        perror("opendir /dev/input");
        return false;
    }

    dirent* ent;

    while((ent=readdir(dir)))
    {
        if(strncmp(ent->d_name,"event",5)!=0)
            continue;

        char path[256];

        snprintf(
            path,sizeof(path),
            "/dev/input/%s",
            ent->d_name
        );

        int fd=open(
            path,
            O_RDONLY|O_NONBLOCK|O_CLOEXEC
        );

        if(fd<0)
            continue;

        if(!ismouse(fd))
        {
            close(fd);
            continue;
        }

        if(grab)
        {
            if(ioctl(fd,EVIOCGRAB,1)<0)
            {
                close(fd);
                continue;
            }
        }

        mousefd=fd;
        grabbed=grab;

        char name[256]{};

        ioctl(fd,EVIOCGNAME(sizeof(name)),name);

        fprintf(
            stderr,
            "[mouse] connected: %s (%s)\n",
            name,path
        );
		usleep(222);
        closedir(dir);
        return true;
    }

    closedir(dir);

    fprintf(stderr,"[mouse] no available mouse found\n");

    return false;
}
// stdin 方向的 SGR 1006 鼠标序列解析（evdev 不可用时使用）
// 序列形如 ESC[<Cb;Cx;Cy(M|m)。1003 模式在鼠标移动时持续上报，
// 用相邻坐标之差计算相对位移。
static std::string sgrbuf;
static int lastx=-1,lasty=-1;

static void sgrMouseParse(const char* data,size_t len)
{
    sgrbuf.append(data,len);
    size_t pos=0;
    while(true)
    {
        // 找 ESC[< 起点
        size_t start=sgrbuf.find("\033[<",pos);
        if(start==std::string::npos){ sgrbuf.clear(); return; }
        if(start>0) sgrbuf.erase(0,start);
        // 找结束符 M 或 m
        size_t end=std::string::npos;
        for(size_t i=3;i<sgrbuf.size();++i)
        {
            char c=sgrbuf[i];
            if(c=='M'||c=='m'){ end=i; break; }
            // 中间只允许数字和分号；遇到别的字节说明不是 SGR 鼠标序列，丢掉这个 ESC
            if(!isdigit((unsigned char)c)&&c!=';'){ sgrbuf.erase(0,1); pos=0; break; }
        }
        if(end==std::string::npos) return; // 不完整，等下次
        std::string body=sgrbuf.substr(3,end-3);
        char fin=sgrbuf[end];
        sgrbuf.erase(0,end+1);
        int cb=-1,cx=0,cy=0;
        if(sscanf(body.c_str(),"%d;%d;%d",&cb,&cx,&cy)!=3) continue;
        bool press=(fin=='M');
        int btn=cb&0x03;
        bool motion=(cb&32)!=0;
        int wheel=(cb&64);
        if(wheel)
        {
            if(press) mouse.wheel+=(wheel==64)?1:-1; // 64=上滚 65=下滚
            continue;
        }
        if(motion)
        {
            if(lastx>=0){ mouse.dx+=cx-lastx; mouse.dy+=cy-lasty; }
            lastx=cx; lasty=cy;
            continue;
        }
        switch(btn)
        {
        case 0: mouse.left=press; break;
        case 1: mouse.middle=press; break;
        case 2: mouse.right=press; break;
        }
    }
}

void pollmouse()
{
    mouse.dx=0;
    mouse.dy=0;
    mouse.wheel=0;

    if(mousefd<0)
    {
        // evdev 不可用 → 读 stdin 的 SGR 鼠标序列（render.cpp 已开启 1000/1003/1006）
        char buf[4096];
        ssize_t n=read(STDIN_FILENO,buf,sizeof(buf));
        if(n>0) sgrMouseParse(buf,(size_t)n);
        return;
    }

    pollfd pfd{};
    pfd.fd=mousefd;
    pfd.events=POLLIN;

    int ready=poll(&pfd,1,0);

    if(ready<0)
    {
        if(errno==EINTR)
            return;

        closemouse();
        return;
    }

    if(ready==0)
        return;

    if(pfd.revents&(POLLERR|POLLHUP|POLLNVAL))
    {
        closemouse();
        return;
    }

    input_event ev;

    while(true)
    {
        ssize_t n=read(mousefd,&ev,sizeof(ev));

        if(n<0)
        {
            if(errno==EAGAIN)
                break;

            if(errno==EINTR)
                continue;

            closemouse();
            return;
        }

        if(n==0)
            break;

        if(n!=sizeof(ev))
            break;

        if(ev.type==EV_SYN)
        {
            if(ev.code==SYN_DROPPED)
                desynced=true;

            if(ev.code==SYN_REPORT && desynced)
            {
                unsigned long keys[
                    (KEY_MAX+sizeof(unsigned long)*8)/
                    (sizeof(unsigned long)*8)
                ]{};

                if(ioctl(mousefd,EVIOCGKEY(sizeof(keys)),keys)==0)
                {
                    mouse.left=hasbit(keys,BTN_LEFT);
                    mouse.right=hasbit(keys,BTN_RIGHT);
                    mouse.middle=hasbit(keys,BTN_MIDDLE);
                }

                desynced=false;
            }

            continue;
        }

        if(desynced)
            continue;

        if(ev.type==EV_REL)
        {
            switch(ev.code)
            {
            case REL_X: mouse.dx+=ev.value; break;
            case REL_Y: mouse.dy+=ev.value; break;
            case REL_WHEEL: mouse.wheel+=ev.value; break;
            }
        }

        if(ev.type==EV_KEY)
        {
            bool down=ev.value!=0;

            switch(ev.code)
            {
            case BTN_LEFT: mouse.left=down; break;
            case BTN_RIGHT: mouse.right=down; break;
            case BTN_MIDDLE: mouse.middle=down; break;
            }
        }
    }
}
void closemouse()
{
    if(mousefd>=0)
    {
        if(grabbed)
            ioctl(mousefd,EVIOCGRAB,0);

        close(mousefd);
    }

    mousefd=-1;
    grabbed=false;
}

MouseEnv::MouseEnv() { initmouse(false); }
MouseEnv::~MouseEnv() { closemouse(); }