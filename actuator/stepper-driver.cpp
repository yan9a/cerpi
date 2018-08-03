#include <stdio.h>
#include <unistd.h>
#include "ce_io.h"
using namespace std;

CE_IO step_pin(17, OUTPUT);//using GPIO 17 for step 
CE_IO dir_pin(18, OUTPUT);//using GPIO 18 for dir

// to move n steps forward with period t
// positive n -> forward, negative n -> backward
void Step(int n, int t)
{
	int d = 0; //direction
	if (n<0) { n *= -1; d = 1; }
	t >>= 1; // divided by 2
	dir_pin.Write(d ? LOW : HIGH);
	for (int i = 0; i < n; i++) {
		step_pin.Write(HIGH);
		usleep(t);
		step_pin.Write(LOW);
		usleep(t);
	}
}

int main()
{	
    printf("Forward.\n");
	Step(2000,2000); //turn 200 steps forward with 5000 us period for each step
	printf("Reverse.\n");
	Step(-2000,2000);//turn 2048 steps backward with 5000 us period for each step
    return 0;
}
