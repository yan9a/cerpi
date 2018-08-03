#!/bin/bash
#Build and run
echo "Building"
g++ wxcv-alpha.cpp util.h `wx-config --cxxflags --libs std` `pkg-config --cflags --libs opencv` -o wxcv-alpha -std=c++11
echo "Running"
gksudo ./wxcv-alpha


