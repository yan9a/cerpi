#!/bin/bash
#Build and run
echo "Building"
g++ watchdog.cpp -o watchdog
echo "Running"
sudo ./watchdog


