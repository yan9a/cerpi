#!/bin/bash
#Build and run
echo "Building"

#if RaspiCam
g++ surveillance.cpp `pkg-config --cflags --libs opencv` -std=c++11 -o surveillance -I/usr/local/include/ -lraspicam -lraspicam_cv

#if USB webcam
#g++ surveillance.cpp `pkg-config --cflags --libs opencv` -std=c++11 -o surveillance

echo "Running"
gksudo ./surveillance


