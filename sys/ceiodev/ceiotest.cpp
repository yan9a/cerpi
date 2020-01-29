/////////////////////////////////////////////////////////////////////////////
// Name:         cedevtest.cpp
// Description:  Testing cedevdrv module 
// Author:       Yan Naing Aye
// Date:         2020 Jan 23
/////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

#define MAX_MESSAGE_LENGTH 256

class cedev
{
    public:
        cedev();
        ~cedev();
        int Open();
        int Write(char* buf,int n);
        int Read(char* buf,int n);
		int Read(char* buf);
		void Close();
		bool IsOpened();
    private:
        int fd;
};

cedev::cedev()
{
	//ctor
}

cedev::~cedev()
{
	//dtor
    Close();
}

int cedev::Open()
{
	const char *filename = "/dev/cedevdrv";
	if ((fd = open(filename, O_RDWR | O_NONBLOCK)) < 0) {
		perror("Failed to open the device\n");		
		abort();
		return errno;
	}	
	return 0;
}

void cedev::Close()
{
	if (IsOpened()) close(fd);
	fd = -1;
}

int cedev::Write(char* buf, int n)
{
	if (!IsOpened()) {
		perror("The device is not opened.\n");	
		return -1;
	}
	int wn = write(fd, buf, n);
	if (wn != n) {
		perror("Failed to write to the device.\n");		
		// abort();
	}	
	return wn;
}

int cedev::Read(char* buf, int n)
{
	if (!IsOpened()) {
		perror("The device is not opened.\n");	
		return -1;
	}
	int rn = read(fd, buf, n);
	if(rn<0){
		perror("Device read error.\n");
	}
	else if (rn != n) {
		// perror("Failed to read from the device.\n");		
		// abort();
	}	
	return rn;
}

int cedev::Read(char* buf)
{
	int rn = -1;
    usleep(1000);
    for(int i = 0; i < 10; i++){        
        rn = Read(buf,MAX_MESSAGE_LENGTH);        
        usleep(1000); 
        if(rn>=0) break;		
    }
	return rn;
}

bool cedev::IsOpened()
{
	if(fd == (-1)) return false;
	else return true;
}

int main()
{
    char rb[MAX_MESSAGE_LENGTH];
    char tb[] = "Hi from userspace!";
    cedev* testd = new cedev();
    testd->Open();
    
    testd->Write(tb,strlen(tb));
    
    // printf("Press enter to read the device\n");
    // getchar();   
    
    int rn = testd->Read(rb);
    printf("Rx %d : ",rn);
    for (int j = 0; j < rn; j++) printf("%c", rb[j]);
    printf("\n");
    
    testd->Close();
    return 0;
}
