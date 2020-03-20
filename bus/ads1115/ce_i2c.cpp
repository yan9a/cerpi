// File: ce_i2c.h
// Description: CE_I2C class to use i2c communication
// WebSite: http://cool-emerald.blogspot.com
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

#include "ce_i2c.h"

template <typename T>
string CE_I2C::ToString(T a)
{
	ostringstream ss;
	ss << a;
	return ss.str();
}

CE_I2C::CE_I2C()
{
	//ctor
}

CE_I2C::CE_I2C(int bus_id, int slave_address)
{
	Begin(bus_id, slave_address);
}

CE_I2C::~CE_I2C()
{
	//dtor
#ifdef CE_LINUX
	close(fd);
#else
#endif
}

bool CE_I2C::Begin(int bus_id, int slave_address)
{
	string filename = "/dev/i2c-";
	filename += ToString(bus_id);
#ifdef CE_LINUX
	if ((fd = open(filename.c_str(), O_RDWR)) < 0) {
		perror("Failed to open the i2c bus\n");
		return false;
	}
	if (ioctl(fd, I2C_SLAVE, slave_address) < 0) {
		perror("Failed to acquire bus access and/or talk to slave.\n");
		return false;
	}
#else
#endif
	return true;
}

bool CE_I2C::Write(char* buf, int n)
{
#ifdef CE_LINUX
	if (write(fd, buf, n) != n) {
		perror("Failed to write to the i2c bus.\n");
		return false;
	}
#else
#endif
	return true;
}

bool CE_I2C::Read(char* buf, int n)
{
#ifdef CE_LINUX
	if (read(fd, buf, n) != n) {
		perror("Failed to read from the i2c bus.\n");
		return false;
	}
#else
#endif
	return true;
}

void CE_I2C::Close()
{
#ifdef CE_LINUX
	close(fd);
#else
#endif
}