//File: ce_stepper.h
//Description: stepper motor driver
//WebSite: http://cool-emerald.blogspot.com
//MIT License (https://opensource.org/licenses/MIT)
//Copyright (c) 2018 Yan Naing Aye

#ifndef CE_STEPPER_H
#define CE_STEPPER_H

#include <unistd.h>
#include "ce_io.h"
using namespace std;
typedef enum {CE_WAVEDRIVE=0,CE_FULLSTEP=1,CE_HALFSTEP} CE_STEP_MODE;
#define PRINT_MES 0

class CE_STEPPER{
    CE_IO pinA;
    CE_IO pinB;
    CE_IO pinC;
    CE_IO pinD;
	int step_A[8];
	int step_B[8];
	int step_C[8];
	int step_D[8];
    int cs;//current step
    int N;//number of steps for the current mode
public:
    CE_STEPPER();
	CE_STEPPER(CE_STEP_MODE mode,int gpio_no_A,int gpio_no_B,int gpio_no_C,int gpio_no_D);
	~CE_STEPPER();
	void Begin(CE_STEP_MODE mode,int gpio_no_A,int gpio_no_B,int gpio_no_C,int gpio_no_D);
    void Mode(CE_STEP_MODE mode);
    void Step(int n,int t);
};

CE_STEPPER::CE_STEPPER()
{
}

CE_STEPPER::CE_STEPPER(CE_STEP_MODE mode,int gpio_no_A,int gpio_no_B,int gpio_no_C,int gpio_no_D)
{
    Begin(mode,gpio_no_A,gpio_no_B,gpio_no_C,gpio_no_D);
}

CE_STEPPER::~CE_STEPPER()
{

}

void CE_STEPPER::Begin(CE_STEP_MODE mode,int gpio_no_A,int gpio_no_B,int gpio_no_C,int gpio_no_D)
{        
    pinA.Begin(gpio_no_A,OUTPUT);
    pinB.Begin(gpio_no_B,OUTPUT);
    pinC.Begin(gpio_no_C,OUTPUT);
    pinD.Begin(gpio_no_D,OUTPUT);
    cs=0;//start current step at 0
    Mode(mode);
}

void CE_STEPPER::Mode(CE_STEP_MODE mode)
{
    if(mode==CE_WAVEDRIVE) {
        step_A[0]=1;step_A[1]=0;step_A[2]=0;step_A[3]=0;
        step_B[0]=0;step_B[1]=1;step_B[2]=0;step_B[3]=0;
        step_C[0]=0;step_C[1]=0;step_C[2]=1;step_C[3]=0;
        step_D[0]=0;step_D[1]=0;step_D[2]=0;step_D[3]=1;
        N=4;
    }
    else if(mode==CE_FULLSTEP) {
        step_A[0]=1;step_A[1]=1;step_A[2]=0;step_A[3]=0;
        step_B[0]=0;step_B[1]=1;step_B[2]=1;step_B[3]=0;
        step_C[0]=0;step_C[1]=0;step_C[2]=1;step_C[3]=1;
        step_D[0]=1;step_D[1]=0;step_D[2]=0;step_D[3]=1;
        N=4;
    }
    else {
        step_A[0]=1;step_A[1]=1;step_A[2]=1;step_A[3]=0;
        step_B[0]=0;step_B[1]=0;step_B[2]=1;step_B[3]=1;
        step_C[0]=0;step_C[1]=0;step_C[2]=0;step_C[3]=0;
        step_D[0]=1;step_D[1]=0;step_D[2]=0;step_D[3]=0;
    
        step_A[4]=0;step_A[5]=0;step_A[6]=0;step_A[7]=0;
        step_B[4]=1;step_B[5]=0;step_B[6]=0;step_B[7]=0;
        step_C[4]=1;step_C[5]=1;step_C[6]=1;step_C[7]=0;
        step_D[4]=0;step_D[5]=0;step_D[6]=1;step_D[7]=1;
        N=8;
    }
    #if PRINT_MES==1
    for(int i=0;i<N;i++){
        printf("%d : %d %d %d %d\n",i,step_A[i],step_B[i],step_C[i],step_D[i]);
    }
    #endif
}

//----------------------------------------------------------
//Turn stepper motor n steps
//with t microseconds period for each step
//positive n for forward dir and negative n for backward dir
void CE_STEPPER::Step(int n,int t) {
  int CD=1; //count down
  if(n<0) {n*=-1; CD=-1;}
  for(int i=0;i<n;i++) {
    cs=(cs+N+CD)%N;//find step number
    pinA.Write(step_A[cs]?HIGH:LOW);
    pinB.Write(step_B[cs]?HIGH:LOW);
    pinC.Write(step_C[cs]?HIGH:LOW);
    pinD.Write(step_D[cs]?HIGH:LOW);
    #if PRINT_MES==1
    printf("%d > %d : %d %d %d %d\n",i,cs,step_A[cs],step_B[cs],step_C[cs],step_D[cs]);
    #endif
    usleep(t);
  }
}
//----------------------------------------------------------

#endif
