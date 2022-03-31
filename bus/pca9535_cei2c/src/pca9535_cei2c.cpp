// File: pca9535_i2c.cpp
// Description: Using io expander PCA9535
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2022 Yan Naing Aye

#include <stdio.h>
#include <cstdint>
#include <string>
#include "ce/ceI2C.h"
#include "ce/cePCA9535.h"
using namespace std;
using namespace ce;
uint8_t chipAddr = 0x20;
uint8_t i2cBus = 1;	
uint8_t p0 = 0x00;
uint8_t p1 = 0xFF;
bool r = true;

int main()
{
	ce::cePCA9535* _chip;
	_chip = new cePCA9535(i2cBus,chipAddr);
	r = r && _chip->Init( // initialize chip 0
		0x00, 0x00, // output p0 all 0, and p1 all 0
		0x00, 0x00, // all active high , no polarity inversion
		0x00, 0xFF); // configure p0 as output, p1 as input

    while(1)
    {		
		p0++; // change port pins
		r = _chip->SetReg(p0,PCA9535_OUTPUT_P0_ADDR); // set output 	
		r = _chip->GetReg(p1,PCA9535_INPUT_P1_ADDR); // get input
		printf("P0 wr = %02X, P1 rd = %02X",p0,p1);
		printf("\n");
		usleep(1000000);
	}
    return 0;
}
