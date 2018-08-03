#include<stdio.h>
#include "ce_pca9685.h"
using namespace std;
int main()
{
    CE_PCA9685 servo_motor(1,0x40,50);//i2c-1, address 0x40, frequency 50 Hz
	for (int i = -90; i <= 90; i++) {
		servo_motor.SetAngle(0,i);
		printf("Angle: %d \n",i);
		usleep(100000);
	}
    return 0;
}
