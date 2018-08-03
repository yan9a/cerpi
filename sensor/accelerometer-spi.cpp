// File: accelerometer-spi.cpp
// Description: SPI communication with LIS3DH accelerometer
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

#include <stdio.h>
#include <stdlib.h>
#include "ce_spi.h"
using namespace std;

int main()
{
  CE_SPI a;
  a.Begin();

  int x,y,z;
  float K=0.061036; // (4000/65535) milli-g per digit for +/-2g full scale using 16 bit digital output

  //read the "who am i" register at address 0x0F
  //Set R/~W bit to read.
  //its value should be 0x33
  a.tx[0] = 0x8F;
  a.tx[1] = 0x00;
  a.Transfer(2);
  printf("I am 0x%02x\n", a.rx[1]);
  
  a.tx[0]=0x20;//Send address of 'Control register 1' to write configuration
  a.tx[1]=0x97;//Write a value that enables x,y,z accelerometers, ODR 1.344kHz, 
  // High resolution mode so that BW is ODR/9 ~ 150Hz
  a.Transfer(2);

  a.tx[0]=0x23;//Send address of 'Control register 4' to write configuration
  a.tx[1]=0x88;//set BDU - block data update, set HR - High resolution mode
  a.Transfer(2);

  for(int j=0;j<10;j++)
  {
    a.tx[0]=0xE8;//Send address of LSB of x. 
    //Set R/~W bit to read. Set M/~S to auto-increase address for multiple rw.
    a.Transfer(7);// 6 bytes to read after the address
    //printf("0x%02x%02x 0x%02x%02x 0x%02x%02x \n", a.rx[1],a.rx[0],a.rx[3],a.rx[2],a.rx[5],a.rx[4]);
    x=((int(a.rx[2])<<8)+a.rx[1]) | (a.rx[2] & 0x80 ? 0xFFFF0000 : 0x00000000);
    y=((int(a.rx[4])<<8)+a.rx[3]) | (a.rx[4] & 0x80 ? 0xFFFF0000 : 0x00000000);
    z=((int(a.rx[6])<<8)+a.rx[5]) | (a.rx[6] & 0x80 ? 0xFFFF0000 : 0x00000000);
    printf("x = %d > mg = %f , \t ",x,(K*x));
    printf("y = %d > mg = %f , \t ",y,(K*y));
    printf("z = %d > mg = %f ",z,(K*z));
    printf("\n");
    usleep(1000000);
  }
	return 0;
}
