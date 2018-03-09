//File: ce_io.h
//Description: Controlling GPIO pins
//WebSite: http://cool-emerald.blogspot.sg
//MIT License (https://opensource.org/licenses/MIT)
//Copyright (c) 2017 Yan Naing Aye

#ifndef CE_IO_H
#define CE_IO_H

#include <string>
#include <fstream>
#include <sstream>
#define IOPATH "/sys/class/gpio/"
using namespace std;
typedef enum {INPUT=0,OUTPUT=1} iodir;
typedef enum {LOW=0,HIGH=1} digitalval;

class CE_IO{
	string fpath;
	int gpioNumber;
public:
    CE_IO();
	CE_IO(int gpiono,iodir mode);
	~CE_IO();
	int Begin(int gpiono,iodir mode);
	digitalval Read();
	int Write(digitalval b);
	int Export();
	int SetDirection(iodir mode);
	template <typename T>
        string ToString(T Number);
};

#endif
