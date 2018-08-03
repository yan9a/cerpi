// File: accelerometer-i2c.cpp
// Description: I2C communication with LIS3DH accelerometer
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

#include<stdio.h>
#include"ce_i2c.h"
using namespace std;
int main()
{
  int x,y,z;
  float K = 0.061036; // (4000/65535) milli-g per digit for +/-2g full scale using 16 bit digital output
  char d[8]={0,0,0,0,0,0,0,0};
  CE_I2C a(1,0x19);//i2c-1 , device address 0x19

  //read the "who am i" register at address 0x0F
  //its value should be 0x33
  d[0]=0x0F;
  a.Write(d,1);
  a.Read(d,1);
  printf("I am 0x%02x\n",d[0]);

  d[0]=0x20;//Send address of 'Control register 1' to write configuration
  d[1]=0x97;//Write a value that enables x,y,z accelerometers, ODR 1.344kHz, 
  // High resolution mode so that BW is ODR/9 ~ 150Hz
  a.Write(d,2);

  d[0] = 0x23;//Send address of 'Control register 4' to write configuration
  d[1] = 0x88;//set BDU - block data update, set HR - High resolution mode
  a.Write(d, 2);

  for(int i=0;i<10;i++)
  {
    //read address 0x28, OUT_X_L register
    d[0]=0xA8;//set msb for address auto-increase
    a.Write(d,1);
    a.Read(d,6);
	//printf("0x%02x%02x 0x%02x%02x 0x%02x%02x \n", d[1],d[0],d[3],d[2],d[5],d[4]);
    x=((int(d[1])<<8)+d[0]) | (d[1] & 0x80 ? 0xFFFF0000 : 0x00000000);
    y=((int(d[3])<<8)+d[2]) | (d[3] & 0x80 ? 0xFFFF0000 : 0x00000000);
    z=((int(d[5])<<8)+d[4]) | (d[5] & 0x80 ? 0xFFFF0000 : 0x00000000);
    printf("x = %d > mg = %f , \t ",x,(K*x));
    printf("y = %d > mg = %f , \t ",y,(K*y));
    printf("z = %d > mg = %f ",z,(K*z));
    printf("\n");
    usleep(1000000);
  }
  return 0;
}

