//File: wpwm.cpp
//Description: PWM using WiringPi
//Reference: https://github.com/WiringPi/WiringPi/blob/master/examples/pwm.c

#include<stdio.h> 
#include<wiringPi.h>

int main(void)
{
	int i;
	printf("WiringPi PWM.\n");
	if (wiringPiSetup() == -1) {
		printf("Fail to setup WiringPi!\n");
		return 1;
	}

	pinMode(1,PWM_OUTPUT); // Set PWM LED as PWM output
    
	while(1)
	{
		for (i = 0; i < 1024; i+=125) {
			pwmWrite(1, i);
			delay(500);//ms
		}
	}
	return 0;
}

