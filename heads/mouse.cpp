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
        {
            fprintf(stderr,"[evdev] cannot open %s: %s\n",path,strerror(errno));
            continue;
        }

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
void pollmouse()
{
    mouse.dx=0;
    mouse.dy=0;
    mouse.wheel=0;

    if(mousefd<0)
        return;

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