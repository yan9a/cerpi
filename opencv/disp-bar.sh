#!/bin/bash
g++ disp.cpp `pkg-config --cflags --libs opencv` -o disp
gksudo ./disp


