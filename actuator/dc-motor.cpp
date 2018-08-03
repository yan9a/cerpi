#include <stdio.h>
#include "ce_pca9685.h"
#include "ce_io.h"
using namespace std;
int main()
{
    CE_PCA9685 dc_motor(1,0x40,100);//i2c-1, address 0x40, frequency 100 Hz
    CE_IO dir(4,OUTPUT);//GPIO 4 as direction control

    printf("Driving DC motor from 0 percent to 100 percent full speed.\n");

    dir.Write(LOW);
    printf("Forward direction.\n");
	for (int i = 0; i <= 100; i+=10) {
		dc_motor.SetDuty(0,i);//channel,percent duty
		printf("Percent FS = %d \n", i);
		usleep(1000000);//wait
	}
    
    dir.Write(HIGH);
    printf("Reverse direction.\n");
	for (int i = 0; i <= 100; i+=10) {
		dc_motor.SetDuty(0,100-i);//channel,percent duty
		printf("Percent FS = %d \n", (100-i));
		usleep(1000000);//wait
	}
    return 0;
}
