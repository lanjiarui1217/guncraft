
#include "keyboard.h"

#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

Keyboard keyboard;

static int keyboardfd=-1;
static bool desynced=false;

static bool hasbit(const unsigned long* bits,int code)
{
    return bits[code/(sizeof(unsigned long)*8)] &
        (1UL<<(code%(sizeof(unsigned long)*8)));
}

static bool iskeyboard(int fd)
{
    unsigned long evbits[
        (EV_MAX+sizeof(unsigned long)*8)/
        (sizeof(unsigned long)*8)
    ]{};

    unsigned long keybits[
        (KEY_MAX+sizeof(unsigned long)*8)/
        (sizeof(unsigned long)*8)
    ]{};

    if(ioctl(fd,EVIOCGBIT(0,sizeof(evbits)),evbits)<0)
        return false;

    if(!hasbit(evbits,EV_KEY))
        return false;

    if(ioctl(fd,EVIOCGBIT(EV_KEY,sizeof(keybits)),keybits)<0)
        return false;

    // A keyboard should have ordinary keys, not just mouse buttons.
    return
        hasbit(keybits,KEY_A) &&
        hasbit(keybits,KEY_ENTER) &&
        hasbit(keybits,KEY_SPACE);
}

bool initkeyboard()
{
    closekeyboard();

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

        if(!iskeyboard(fd))
        {
            close(fd);
            continue;
        }

        keyboardfd=fd;
        desynced=false;

        memset(&keyboard,0,sizeof(keyboard));

        char name[256]{};

        ioctl(fd,EVIOCGNAME(sizeof(name)),name);

        fprintf(
            stderr,
            "[keyboard] connected: %s (%s)\n",
            name,path
        );

        closedir(dir);
        return true;
    }

    closedir(dir);

    fprintf(stderr,"[keyboard] no available keyboard found\n");

    return false;
}

void pollkeyboard()
{
    if(keyboardfd<0)
        return;

    pollfd pfd{};

    pfd.fd=keyboardfd;
    pfd.events=POLLIN;

    int ready=poll(&pfd,1,0);

    if(ready<0)
    {
        if(errno==EINTR)
            return;

        closekeyboard();
        return;
    }

    if(ready==0)
        return;

    if(pfd.revents&(POLLERR|POLLHUP|POLLNVAL))
    {
        closekeyboard();
        return;
    }

    input_event ev;

    while(true)
    {
        ssize_t n=read(keyboardfd,&ev,sizeof(ev));

        if(n<0)
        {
            if(errno==EAGAIN)
                break;

            if(errno==EINTR)
                continue;

            closekeyboard();
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

                if(ioctl(
                    keyboardfd,
                    EVIOCGKEY(sizeof(keys)),
                    keys
                )==0)
                {
                    for(int i=0;i<=KEY_MAX;i++)
                        keyboard.keys[i]=hasbit(keys,i);
                }

                desynced=false;
            }

            continue;
        }

        if(desynced)
            continue;

        if(ev.type==EV_KEY && ev.code<=KEY_MAX)
        {
            if(ev.value==0)
                keyboard.keys[ev.code]=false;

            if(ev.value==1)
                keyboard.keys[ev.code]=true;

            // ev.value == 2 means key repeat.
            // The key is still pressed.
        }
    }
}

void closekeyboard()
{
    if(keyboardfd>=0) close(keyboardfd);

    keyboardfd=-1;
    desynced=false;

    memset(&keyboard,0,sizeof(keyboard));
}

KeyboardEnv::KeyboardEnv()
{
    initkeyboard();
}

KeyboardEnv::~KeyboardEnv()
{
    closekeyboard();
}
