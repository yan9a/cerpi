// File: ce_i2c.h
// Description: CE_I2C class to use i2c communication
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

// Reference : http://elinux.org/Interfacing_with_I2C_Devices

#ifndef CE_I2C_H
#define CE_I2C_H


#if defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
#ifndef CE_WINDOWS
#define CE_WINDOWS
#endif



#else
#ifndef CE_LINUX
#define CE_LINUX
#endif


#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>


#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
using namespace std;

class CE_I2C
{
    public:
        CE_I2C();
        CE_I2C(int bus_id,int slave_address);
        ~CE_I2C();
        bool Begin(int bus_id,int slave_address);
        bool Write(char* buf,int n);
        bool Read(char* buf,int n);
		void Close();
        template <typename T>
            string ToString(T Number);
    private:
        int fd;
};

#endif // CE_I2C_H
