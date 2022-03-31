// File: pca9535_i2c.cpp
// Description: Using io expander PCA9535
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2022 Yan Naing Aye

#include <stdio.h>
#include <cstdint>
#include <string>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

bool r = true;

int main()
{
	int file;
	const char *filename="/dev/i2c-1";
	if((file = open(filename, O_RDWR))<0){
		printf("Failed to open the i2c bus\n");
	}

	int addr = 0x20;
	if(ioctl(file, I2C_SLAVE, addr)<0){
		printf("Failed to access the i2c bus\n");
	}

	char buf[2] = {0, 0};
	int i=0;

	// to write a register write 2 bytes - 
	// 1st byte = reg address, 2nd byte = reg value
	// int n = 2;
	// if(write(file,buf,n)!=n){
	// 	printf("Failed to write the i2c bus\n");
	// }

	// to read a register write 1 bytes address and read 1 byte value- 
	// buf[0] = address
	// int n = 1;
	// if(write(file,buf,n)!=n){
	// 	printf("Failed to write the i2c bus\n");
	// }
	// if(read(file,buf,n)!=n){
	// 	printf("Failed to read the i2c bus\n");
	// }
	// buf[0] is reg value

    while(1)
    {		
		// read first 2 register values
		if(read(file,buf,2)!=2){
			printf("Failed to read the i2c bus\n");
		}
		for(int j=0;j<2;j++){
			printf("%02X ",buf[j]);
		}
		
		printf("\n");
		usleep(1000000);
		if(++i>=1){
			break;
		}
	}
	close(file);
    return 0;
}
