// File: ads1115.cpp
// Description: analog input and analog output with ADS1115
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2020 Yan Naing Aye

#include <stdio.h>
#include <cstdint>
#include <string>
#include "ce_i2c.h"
using namespace std;

class ADS1115 {
public:
	ADS1115(uint8_t i2cBusNo, uint8_t i2cAddr);
	// ~ADS1115();
	void UpdateLastConversion();
	void StartConversion(uint8_t channel);
	// int16_t ContConvRead();
	uint8_t GetChannel();
	uint16_t GetConversionValue(uint8_t channel);
	float V(uint8_t channel);
private:
	uint8_t _bus;
	uint8_t _addr;
	uint8_t _channel;
	int16_t _ai[4];	
};

// Get current channel
uint8_t ADS1115::GetChannel()
{
	return this->_channel;
}

// Get conversion value
uint16_t ADS1115::GetConversionValue(uint8_t channel)
{
	channel &= 0x03;
	return this->_ai[channel];
}

// Get voltage
float ADS1115::V(uint8_t channel)
{
	channel &= 0x03;
	// 4.096 / 32767 = 0.00012500
	float v = 0.00012500;
	v *= float(this->_ai[channel]);
    return v;
}

// addr : [0 - 1]
// channel : [0 - 3]
ADS1115::ADS1115(uint8_t i2cBusNo, uint8_t i2cAddr):_bus(i2cBusNo)
{
	this->_channel = 0;
	this->_addr = 0x48 | (i2cAddr & 0x01); // i2c address
}

// Read previous conversion result
void ADS1115::UpdateLastConversion()
{
	char d[] = { 0,0,0,0 };
	uint8_t v[] = {0,0};
	CE_I2C adc(this->_bus,this->_addr);
	// first byte in i2c wr is the address reg
	d[0] = 0x00; // point to conversion register
	adc.Write(d, 1);	
	// read 2 bytes conversion reg value	
	adc.Read(d, 2); 
	adc.Close();
	v[1] = d[0];
	v[0] = d[1];
	this->_ai[this->_channel] = *(int16_t*)(v);	
}

// start conversion of next channel
void ADS1115::StartConversion(uint8_t channel)
{
	char d[] = { 0,0,0,0 };
	CE_I2C adc(this->_bus,this->_addr);
	// Choose channel to read
	// first byte in i2c wr is the address reg
	d[0] = 0x01; // point to control reg
	// followed by 2 bytes (LSB MSB) 16 bit control reg
	// | OS | MUX[2:0] | PGA[2:0] | Mode |
	// | 1  | 1 c1 c0  | 001      | 1    |
	// OS = 1 start single shot conversion
	// MUX[2:0] 100 = A0 to GND, 101 = A1 to GND, 110 = A2 to GND, 111 = A31 to GND
	// PGA[2:0] 001 +/- 4.096 V default
	// MODE = 1 single shot mode - default
	d[1] = 0xC3; // MSB first
	// | DR[2:0] | COMP_MODE | COMP_POL | COMP_LAT | COMP_QUE[1:0] |
	// |   111   |   0       |  0       |    0     |   11          |
	// DR[2:0] = 111 for data rate of 860 SPS
	// COMP_MODE = 0 default - traditional comparator	
	// COMP_POL = 0 default - active low
	// COMP_LAT = 0 default - non latching
	// COMP_QUE[1:0] = 11 default - high impedance ALERT/RDY pin
	d[2] = 0xE3;
	//--------------------------------------------------------------------
	// select next channel
	this->_channel = channel & 0x03; // get adc channel
	d[1] |= (this->_channel << 4);
	//--------------------------------------------------------------------
	adc.Write(d, 3);	// set control reg to start conversion of next channel
	adc.Close();
}

int main()
{
	int16_t v;
	ADS1115 a(1,0);
    while(1)
    {				
		for(uint8_t i=0;i<4;i++){
			a.StartConversion(i);
			usleep(256000);
			a.UpdateLastConversion();
			printf("C%d = %1.3f V, ",i,a.V(i));
		}
		printf("\n");
	}
    return 0;
}
