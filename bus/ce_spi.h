// File: ce_spi.h
// Description: SPI communication class
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

// References
// spidev_test.c @ https://raw.githubusercontent.com/raspberrypi/linux/rpi-3.10.y/Documentation/spi/spidev_test.c

#ifndef CESPI_H
#define CESPI_H

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <string>
using namespace std;
#define BUF_SIZE 8

class CE_SPI
{
    public:
        CE_SPI();
        virtual ~CE_SPI();
        bool Begin();
        void Transfer(unsigned int n);
        uint8_t tx[BUF_SIZE];
        uint8_t rx[BUF_SIZE];
    private:
        int fd;
        uint8_t mode;
        uint8_t bits;
        uint32_t speed;
        uint16_t delay;
        string spidev;
};

CE_SPI::CE_SPI()
{
    mode=SPI_MODE_0;
    bits=8;
    speed=500000;//500 kHz
    delay=0;
    spidev="/dev/spidev0.0";
    for(int i=0;i<BUF_SIZE;i++){
        tx[i]=0;
        rx[i]=0;
    }
}

CE_SPI::~CE_SPI()
{
    close(fd);
}

bool CE_SPI::Begin()
{
    fd = open(spidev.c_str(), O_RDWR);
	if (fd < 0){
        perror("Can't open spi device.\n");
        abort();
	}

    if(ioctl(fd, SPI_IOC_WR_MODE, &mode)==-1){
        perror("Can't set SPI mode.\n");
        abort();
    }

    if(ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits)==-1){
        perror("Can't set bits per word.\n");
        abort();
    }

    if(ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed)==-1){
        perror("Can't set max speed hz.\n");
        abort();
    }
	return true;
}

void CE_SPI::Transfer(unsigned int n)
{
    struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = n,
        .speed_hz = speed,
		.delay_usecs = delay,		
		.bits_per_word = bits,
	};

	if(ioctl(fd, SPI_IOC_MESSAGE(1), &tr)<1){
        perror("Can't send SPI message.\n");
	}
}

#endif // CESPI_H
