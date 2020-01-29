// File: i2c_aio.cpp
// Description: analog input and analog output with PCF8591
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2020 Yan Naing Aye

#include <stdio.h>
#include <cstdint>
#include <string>
#include "ce_i2c.h"
using namespace std;

typedef struct {
	uint8_t feature;
	uint8_t ao;
	uint8_t ai[4];	
} IOAnalog;

uint8_t ao_last;

void PCF8591Out(uint8_t  chipno, uint8_t value)
{
	char d[] = { 0,0,0,0};
	int i2caddr = 0x48 + (chipno & 0x07); // i2c address
	int i2cbus = 1;
	CE_I2C chip1(i2cbus, i2caddr);

	d[0] = 0x44; // first byte in i2c wr is the control reg
	// | reserved | AO Enable | mode sel 1 | mode sel 0 |
	//    | reserved | auto increase | channel no 1 | channel no 0 |
	// |   0      |   1       |  0             0        |
	//    |  0       |     1         |     0            0          |
	// auto increase needs AO enable to keep internal osc running
	// mode = 00 for 4 single ended ai
	// start reading from channel 0 and then auto increase
	// second byte to write to Ao
	d[1] = value;
	chip1.Write(d, 2);	// send new data
	ao_last = value;

	chip1.Close();
	string str = "Chip " + to_string(chipno) + ", Ao: " + to_string(value);
	printf("%s\n",str.c_str());
}

IOAnalog PCF8591In(uint8_t  chipno)
{
	IOAnalog value;
	char d[] = { 0,0,0,0,0,0 };
	int i2caddr = 0x48 + (chipno & 0x07); // i2c address
	int i2cbus = 1;
	CE_I2C chip1(i2cbus, i2caddr);

	d[0] = 0x44; // first byte in i2c wr is the control reg
	// | reserved | AO Enable | mode sel 1 | mode sel 0 |
	//    | reserved | auto increase | channel no 1 | channel no 0 |
	// |   0      |   1       |  0             0        |
	//    |  0       |     1         |     0            0          |
	// auto increase needs AO enable to keep internal osc running
	// mode = 00 for 4 single ended ai
	// start reading from channel 0 and then auto increase
	chip1.Write(d, 1);	// set control reg to start at channel 0
	chip1.Read(d, 5); // read 5 byte reg value
	chip1.Close();

	// d[0] is the last read value before this read
	value.ai[0] = d[1];
	value.ai[1] = d[2];
	value.ai[2] = d[3];
	value.ai[3] = d[4];
	value.ao = ao_last;

	string str = "Chip " + to_string(chipno) + ", Values: " 
	    + to_string(value.ai[0]) + ", " + to_string(value.ai[1]) + ", " 
	    + to_string(value.ai[2]) + ", " + to_string(value.ai[3]);
	printf("%s\n",str.c_str());
	return value;
}


int main()
{
	uint8_t v = 0;
	float x=0,dx=0,steps=10.0;
	dx = 3.3 / steps;
    for(int i=0; i < steps; i++)
    {		
        // d = v / 3.3 * 256 = v * 77.576 (for 8 bits, 3.3 V full scale)
        x += dx;
        v = uint8_t (x * 256.0 / 3.3);
        printf("x = %f v = %d \n", x,v);
        PCF8591Out(0,v);
        usleep(1000000);
        PCF8591In(0);
        usleep(2000000);
    }
    return 0;
}
