// File: i2cao.cpp
// Description: analog output with MCP4725
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

#include<stdio.h>
#include"ce_i2c.h"
using namespace std;
int main()
{
    char d[2]={0,0};
    CE_I2C mcp4725(1,0x60);//use i2c-1, address 0x60


    //Write DAC reg using fast mode
    //1st byte  = |c2|c1|PD1|PD0|D11|D10|D9|D8|
    //Default =   |0 |0 |0  |0  |x  |x  |x |x |
    //c2 c1 = 00 => Fast mode
    //PD1 PD0 = 00 => Power down select (00 : normal mode)
    //D11 D10 D9 D8 = Four MSb of 12 bit DAC value
	//
	//2nd byte  = |D7|D6|D5|D4|D3|D2|D1|D0|
	// D7 - D0 = LSB of 12 bit DAC value
    
	int v = 0;
	float x=0,dx=0,steps=10.0;
	dx = 3.3 / steps;
    for(int i=0;i<steps;i++)
    {		
        // d = v / 3.3 * 4095 = v * 1241 (for 12 bits, 3.3 V full scale)
		x += dx;
		v = int(x * 4095.0 / 3.3);
		printf("x = %f v = %d \n", x,v);
		d[0] = (v >> 8) & 0x0F;// 4 MSb
		d[1] = v & 0xFF; // LSB
		mcp4725.Write(d,2);
        usleep(3000000);
    }
    return 0;
}
