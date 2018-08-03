#ifndef CE_WATCHDOG_H
#define CE_WATCHDOG_H

//References
//https://github.com/derekmolloy/exploringrpi/tree/master/chp12/watchdog
//https://embeddedfreak.wordpress.com/2010/08/23/howto-use-linux-watchdog/

#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<linux/watchdog.h>
#define WATCHDOG "/dev/watchdog"

#define CE_WD_PRINT 0

class CE_Watchdog {
    int fd;
    int interval;
public:
    CE_Watchdog();
    ~CE_Watchdog();
    int Begin();
    int Pat();
    int Close();
    int GetInterval();
    bool IsLastBootByWatchdog();
};

CE_Watchdog::CE_Watchdog()
{
    fd=-1;//invalid handle
    interval=15;//in seconds
    //Begin();
}

int CE_Watchdog::Begin()
{
    int r=-1;
    if(fd==(-1)){
        //if(getuid()!=0){
           //printf("You must run this program as root.\n");
        //}
        if ((fd = open(WATCHDOG, O_RDWR))<0){
          perror("Error in opening watchdog.\n");
          return r;
        }
        // set the timing interval
        if (ioctl(fd, WDIOC_SETTIMEOUT, &interval)!=0){
          perror("Error in setting watchdog interval.\n");
          return r;
        }
        r=0;
        #if CE_WD_PRINT ==1
        printf("Watchdog begun. \n");
        #endif // CE_WD_PRINT
    }
    return r;
}

CE_Watchdog::~CE_Watchdog()
{
    this->Close();
}

int CE_Watchdog::Pat()
{
    int r=-1;
    if(fd!=(-1)){
        r=ioctl(fd, WDIOC_KEEPALIVE, NULL);
        #if CE_WD_PRINT ==1
        printf("Patting Watchdog. \n");
        #endif // CE_WD_PRINT
    }
    return r;
}

int CE_Watchdog::Close()
{
    int r=-1;
    if(fd!=(-1)){
        r=write(fd, "V", 1);
        r=close(fd);
        fd=-1;
        #if CE_WD_PRINT ==1
        printf("Closing Watchdog. \n");
        #endif // CE_WD_PRINT
    }
    return r;
}

int CE_Watchdog::GetInterval()
{
    int v=0;
    if(fd!=(-1)){
        if (ioctl(fd, WDIOC_GETTIMEOUT, &v) != 0) {
            perror("Error in reading watchdog interval.\n");
            v=0;
        }
    }
    return v;
}

/*
bool CE_Watchdog::IsLastBootByWatchdog()
{
    int v=0;
    if(fd!=(-1)){
        if (ioctl(fd, WDIOC_GETBOOTSTATUS, &v) != 0) {
            perror("Error in reading boot status.\n");
            v=0;
        }
    }
    return (v!=0)?true:false;
}
*/
#endif
