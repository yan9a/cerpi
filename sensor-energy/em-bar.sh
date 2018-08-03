#!/bin/bash

echo "Compiling..."
unamestr=`uname -m`
if [[ "$unamestr" == 'x86_64' ]]; then
   echo "on x86_64"
   g++ em.cpp `wx-config --cxxflags --libs std`-o em -std=c++11
   echo "CWD: $PWD"
   gnome-terminal -x sh -c './em'
elif [[ "$unamestr" == 'armv7l' ]]; then
   echo "on armv7l"
   g++ em.cpp `wx-config --cxxflags --libs std`-o em -std=c++11
   echo "CWD: $PWD"
   gksudo ./em
fi
echo "Done."



