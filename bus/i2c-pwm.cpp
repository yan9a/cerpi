// File: i2c-pwm.cpp
// Description: PWM output with PCA9685
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

#include<stdio.h>
#include"ce_i2c.h"
using namespace std;
int main()
{
	char d[8] = { 0,0,0,0,0,0,0,0 };
    CE_I2C pca9685(1,0x40);//use i2c-1, address 0x40
    
    //MODE1 register, address 0x00    
    //MODE1   = |Restart|ExtClk|AI |SLEEP|SUB1|SUB2|SUB3|ALLCALL|
    //Default = |0      |0     |0  |1    |0   |0   |0   |1      |
    //Restart = 0 => Restart disabled
    //ExtClk  = 0 => External clock disabled
    //AI      = 1 => Auto increment enabled
    //SLEEP   = 0 => Normal mode (don't sleep)
    //SUB1    = 0 => Do not respond to sub address group 1
    //SUB2    = 0 => Do not respond to sub address group 2
    //SUB3    = 0 => Do not respond to sub address group 3
    //ALLCALL = 1 => Respond to all call address
    
	
	//To sleep (default value for MODE1 Register)	
	d[0] = 0;// address
	d[1] =0x11;
	pca9685.Write(d,2);
	usleep(1000); //wait a while
	
	//Set frequency 50 Hz
	//PRE_SCALE register, address 0xFE
	//N= [25000000/(4096*f)]-1 =121 = 0x79
	d[0]=0xFE;
	d[1]=0x79;
	pca9685.Write(d,2);
	usleep(1000); //wait a while
	
	//Go back to Normal mode
    //MODE1 register, address 0x00   
	d[0] = 0;// address
    d[1] = 0x21;
    pca9685.Write(d,2);
    //Need at least 500us to wake up from sleep
    usleep(1000);
    
    //Output settings
    //MODE2 register, address 0x01    
    //MODE2   = |Reserved |INVRT|OCH|OUTDRV|OUTNE|
    //Default = |0 |0 |0  |0    |0  |1     |00    |
    //Reserved=000=> Reserved 
    //INVRT   = 0 => Output logic state not inverted
    //OCH     = 0 => Output change on stop command instead of ACK
    //OUTDRV  = 1 => push-pull output instead of open-drain
    //OUTNE  = 00 => output 0 when output is disabled (i.e. LEDn = 0 when #OE = 1)
	d[0] = 1;// address
    //Value to set = 0000 0100
    d[1] = 0x04;    
    pca9685.Write(d,2);
    
    //Control PWM
	//LED on = on at 0 of 0-4095 clock cycles
    //LED off = off at 1024 of 0-4095 clock cycles
    d[0] = 0x06; //address of LED0 ON_L is at 0x06
    d[1] = 0x00; //ON_L
    d[2] = 0x00; //ON_H
    d[3] = 0x00; //OFF_L
    d[4] = 0x04; //OFF_H - (Note: overwriting full off bit 4 to 0) 
    //OFF_H = 0x10 means always off, 
    //always off has higher priority than always on
    pca9685.Write(d,5);
    return 0;
}
