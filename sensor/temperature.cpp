#include <stdio.h>
#include <unistd.h>
#include "ce_spi.h"
using namespace std;
int main()
{
    CE_SPI tsensor;
    tsensor.Begin();

	int n;
	float v, t;
    for(int i=0;i<10;i++){
        tsensor.tx[0]=0x01;//Send start bit - 0000 0001
        tsensor.tx[1]=0x80;//read channel 0  - | s/~d | d2 d1 d0 | x x x x |
        // b7= single/~differential = 1 for single ended
        // d2 d1 d0 = 0 for channel 0
        tsensor.tx[2]=0x00; //  don't care = x x x x x x x x
        tsensor.Transfer(3);
		//printf("%02X %02X \n",(unsigned char)tsensor.rx[1], (unsigned char)tsensor.rx[2]);
        n = tsensor.rx[1]<<8 | tsensor.rx[2]; 
        n&=0x03FF;//mask out invalid bits
		v = 3.3*float(n)/ 1023;//using 3.3 Vref
		t = (float(n)*3.3 - 511.5) / 10.23;
        printf("n= %d v=%f t=%f\n",n,v,t);
        usleep(1000000);
    }
    return 0;
}
