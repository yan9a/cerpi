#!/bin/bash
#Build and run
echo "Building"
g++ wxcvsimple.cpp util.h `wx-config --cxxflags --libs std` `pkg-config --cflags --libs opencv` -o wxcvsimple
echo "Running"
gksudo ./wxcvsimple


