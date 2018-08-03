#include <stdio.h>
#include <unistd.h>
#include <string>
#include "ce_io.h"
using namespace std;
int main()
{
    CE_IO light(25,INPUT);
    CE_IO relay(4,OUTPUT);

    while(1){
        //print status
        printf("Light sensor output = %d \n",light.Read());

        //produce relay output
        relay.Write(light.Read());
        
        //sleep
        usleep(1000000);
    }
    return 0;
}
