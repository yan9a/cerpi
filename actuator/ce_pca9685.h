//File: ce_pca9685.h
//Description: Controlling PCA9685 PWM
//WebSite: http://cool-emerald.blogspot.com
//MIT License (https://opensource.org/licenses/MIT)
//Copyright (c) 2018 Yan Naing Aye

#ifndef CE_PCA9685_H
#define CE_PCA9685_H

#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <math.h>
#include "ce_i2c.h"

using namespace std;

class CE_PCA9685 :public CE_I2C
{
    public:
        CE_PCA9685();
        CE_PCA9685(int bus_id,int slave_address,int frequency);
        ~CE_PCA9685();
        bool SetFrequency(int frequency);
        bool Begin(int bus_id,int slave_address,int frequency);
        bool SetDuty(int channel,float percent);
        bool SetAngle(int channel,float degree);//for servo motor
    private:
        char d[8];
};

//-----------------------------------------------------------------------------

CE_PCA9685::CE_PCA9685():CE_I2C()
{
    
}

CE_PCA9685::CE_PCA9685(int bus_id,int slave_address,int frequency):CE_I2C(bus_id,slave_address)
{
    SetFrequency(frequency);
}

CE_PCA9685::~CE_PCA9685()
{
}


bool CE_PCA9685::Begin(int bus_id,int slave_address,int frequency)
{
    bool r;
    r = CE_I2C::Begin(bus_id,slave_address);
    if(!r){return false;}
    
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
    r=Write(d,2);
    if(!r){return false;}
    
    return SetFrequency(frequency);
}

bool CE_PCA9685::SetFrequency(int frequency)
{
    bool r;
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
	r=Write(d,2);
    if(!r){return false;}
	
	//Set frequency f Hz
	//PRE_SCALE register, address 0xFE
	//N= [25000000/(4096*f)]-1 , e.g for f=50Hz , N = 121 = 0x79
    
    float den=4096*frequency;//denominator
    float Nf=(25000000.0/den) - 1;
	d[0]=0xFE;
	d[1]=char(int(floor(Nf+0.5)) & 0xFF);
	r=Write(d,2);
    if(!r){return false;}
	
	//Go back to Normal mode
    //MODE1 register, address 0x00   
	d[0] = 0;// address
    d[1] = 0x21;
    r=Write(d,2);
    if(!r){return false;}
    //Need at least 500us to wake up from sleep
    usleep(1000);
    return true;
}

bool CE_PCA9685::SetDuty(int channel,float percent)
{
    if(percent < 0) percent = 0;
    else if(percent >100) percent =100;

    if(channel < 0) percent = 0;
    else if(channel >15) channel =15;

    //Control PWM
	//LED on = on at 0 of 0-4095 clock cycles
    //LED off = off at n of 0-4095 clock cycles
    float cp=percent*4095.0/100.0;
    int n=floor(cp+0.5);//LED off
    d[0] = 0x06+(channel*4); //address of LED0 ON_L is at 0x06, 4 bytes per channel
    d[1] = 0x00; //ON_L
    d[2] = 0x00; //ON_H
    d[3] = char(n & 0xFF); //OFF_L
    d[4] = char((n>>8) & 0x0F); //OFF_H - (Note: overwriting full off bit 4 to 0) 
    //OFF_H = 0x10 means always off, 
    //always off has higher priority than always on
    return Write(d,5);
}

bool CE_PCA9685::SetAngle(int channel,float degree) //for servo motor
{
    if(degree<-90) degree=-90;
    else if(degree > 90) degree =90;
    
    //servo -90 to 90 is duty cycle 3% to 12% @ 50 Hz
    float duty=(degree+90.0)/180.0;
    float p = 3.0 + duty*9.0;
    return SetDuty(channel,p);
}
//-----------------------------------------------------------------------------

#endif // CE_PCA9685_H
