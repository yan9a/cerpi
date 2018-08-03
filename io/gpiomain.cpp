#include <stdio.h>
#include <unistd.h>
#include <string>
#include "ce_io.h"
using namespace std;
int main()
{
    CE_IO Pin7(4,OUTPUT);
    CE_IO Pin22(25,INPUT);

    for(int i=0;i<5;i++){
        printf("i=%d\n",i);

        //Read input
        printf("Input= %d \n",Pin22.Read());

        //Write output
        Pin7.Write(HIGH);
        printf("Output = 1");
        usleep(500000);
        Pin7.Write(LOW);
        printf(" > 0\n");
        usleep(500000);
    }
    return 0;
}
