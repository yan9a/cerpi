#include <stdio.h>
#include <unistd.h>
#include "ce_stepper.h"
using namespace std;
int main()
{
    CE_STEPPER stepper_motor(CE_HALFSTEP,17,18,23,24);// full step drive, using GPIO 17,18,23,24  
    printf("Forward.\n");
	stepper_motor.Step(2048,2500); //turn 2048 steps forward with 2500 us period for each step
	printf("Reverse.\n");
	stepper_motor.Step(-2048,2500);//turn 2048 steps backward with 2500 us period for each step
    return 0;
}
