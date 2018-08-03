//File: wio.c
//Description: simple digital IO using wiringPi
//Reference: http://wiringpi.com/examples/quick2wire-and-wiringpi/install-and-testing/

#include <stdio.h>
#include <wiringPi.h>

#define BTN 0 //wiringPi pin 0 (pin 11 on the header)
#define LED 1 //wiringPi pin 1 (pin 12 on the header)
int main(void)
{
	int p;
	printf("Wiring Pi digital IO.\n");
	if (wiringPiSetup() == -1) {
		printf("Fail to setup WiringPi!\n");
		return 1;
	}
	pinMode(BTN, INPUT);
	pinMode(LED,OUTPUT);

	while(1)
	{
		if (digitalRead(BTN) == HIGH) p = 250;
		else p = 1000;

		digitalWrite(LED,HIGH);  // On
		delay(p);               // mS
		digitalWrite(LED,LOW);   // Off
		delay(p) ;
	}
	return 0 ;
}
