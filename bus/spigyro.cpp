// File: spigyro.c
// Description: SPI communication with gyroscope
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

// References
// spidev_test.c @ https://raw.githubusercontent.com/raspberrypi/linux/rpi-3.10.y/Documentation/spi/spidev_test.c

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static int fd;
static const char *device = "/dev/spidev0.0";
static uint8_t mode = SPI_MODE_0;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay = 0;
static uint8_t tx[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x00};
static uint8_t rx[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x00};
    
static void Transfer(unsigned int n)
{
	int ret;
	
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = n,
        .speed_hz = speed,
		.delay_usecs = delay,		
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1) pabort("can't send spi message");
}


int main(int argc, char *argv[])
{
	int ret = 0;

    //Init SPI
	fd = open(device, O_RDWR);
	if (fd < 0) pabort("can't open device");

	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1) pabort("can't set spi mode");


	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1) pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1) pabort("can't set max speed hz");


	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

    //Transfer 
    
	//b7 = set (1) for reading
    //b6 = cleared (0) not to auto increase address
    //b5-b0 = register address

    //read the "who am i" register at address 0x0F
    //its value should be 0xD3
    tx[0]=0x8F;//read, not auto 0x0F | 0x80
    Transfer(2);
    for(int i=1;i<2;i++){
        printf("I am 0x%02x",rx[i]);
    }
    printf("\n");
    
    //Configure Gyro
    //CTRL_REG2 = |0 |0 |HPM1|HPM0|HPCF3|HPCF2|HPCF1|HPCF0|
    //Default =   |0 |0 |0   |0   |0    |0    |0    |0    |
    //HPM = 00 => Normal (High Pass filter Mode selection)
    //HPC = 1001 => 0.1 High Pass Filter Cut-off freq configuration
    //write @address 0x21, value = 0x09
    tx[0]=0x21;
    tx[1]=0x09;
    Transfer(2);
    
    //CTRL_REG3 = |I1_Int1|I1_Boot|H_Lactive|PP_OD|I2DRDY|I2_WTM|I2_ORun|I2_Empty|
    //Default =   |0      |0      |0        |0    |0     |0     |0      |0       |
    //Use Default

    //CTRL_REG4 = |BDU|BLE|FS1|FS0| - |ST1|ST0|SIM|
    //Default =   |0  |0  |0  |0  |0  |0  |0  |0  |
    //BDU = 1 => Block Data Update
    //BLE = 0 => Little endian
    //FS = 00 => 250 dps (Full scale selection)
    //ST = 000 => Disable Self test
    //write @ address 0x23, value =0x80
    tx[0]=0x23;
    tx[1]=0x80;
    Transfer(2);

    //CTRL_REG5 = |BOOT|FIFO_EN| - |HPen|INT1_Sel1|INT1_Sel0|Out_Sel1|Out_Sel0|
    //Default =   |0   |0      |0  |0   |0        |0        |0       |0       |
    //BOOT = 0 => Normal mode (Reboot Memory Content)
    //FIFO_EN = 0 => disable FIFO
    //HPen = 0 => disable (High Pass Filter)
    //INT1_Sel = 00 => Non high pass filtered data are used for interrupt generation
    //Out_Sel = 00 => no filtering
    //Use Default

    //CTRL_REG1 = |DR1|DR0|BW1|BW0|PD|Zen|Yen|Xen|
    //Default =   |0  |0  |0  |0  |0 |1  |1  |1  |
    //DR = 11 => ODR 800 Hz (output data rate)
    //BW = 10 => Cut-off 50 (Bandwidth 50 Hz)
    //PD = 1 => Normal
    //Zen = Yen = Xen = 1 => Enable
    //write @ address 0x20, value =0xEF
    tx[0]=0x20;
    tx[1]=0xEF;
    Transfer(2);

    tx[0]=0xE0;//read ctrl registers
    Transfer(5);//send addr 1 byte + read 4 byte
    for(int i=1;i<5;i++){
        printf("Reg = 0x%02x ",rx[i]);
    }
    printf("\n");

    int x,y,z;
    for(int j=0;j<10;j++)
    {
        //read address 0x28, OUT_X_L register
        //set MSB bit for auto address increment
        tx[0]=0xE8;
        Transfer(7);
        x=((int(rx[2])<<8)+rx[1])|(rx[2]&0x80?0xFFFF0000:0x00000000);
        y=((int(rx[4])<<8)+rx[3])|(rx[4]&0x80?0xFFFF0000:0x00000000);
        z=((int(rx[6])<<8)+rx[5])|(rx[6]&0x80?0xFFFF0000:0x00000000);
        printf("x y z = %d %d %d",x,y,z);
        printf("\n");
        usleep(500000);
    }

    //Close
	close(fd);

	return ret;
}
