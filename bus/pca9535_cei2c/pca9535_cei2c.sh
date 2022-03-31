#!/bin/bash
echo "Current directory $PWD"
SCRIPTDIR=$PWD
PRJNAME="pca9535_cei2c"
opt_sh="r"

if [ $# == 1 ]; then
 opt_sh=$1
else 
 echo "You can input argument:"
 echo " 'c': cmake"
 echo " 'b': build"
 echo " else: run" 
 echo " ..."
 read -p "Input an option: " opt_sh
fi

if [[ "$opt_sh" == "" ]]; then
    opt_sh="b"
fi

echo "Option: $opt_sh"
echo " ."
echo " ."
echo " ."


if [[ "$opt_sh" == "i" ]] || [[ "$opt_sh" == "c" ]]; then
    echo "Preparing cmake file"
    cd $SCRIPTDIR
    if [[ ! -d "./build" ]]; then
        mkdir -p build
    fi
    rm -r ./build/*
    cd build
    cmake -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    ..
    cd ..
    echo " ."
    echo " ."
    echo " ."
fi

if [[ $opt_sh == "i" ]] || [[ $opt_sh == "c" ]] || [[ $opt_sh == "b" ]]; then
    echo "Building ..."
    cd $SCRIPTDIR/build && make # && sudo make install
    if [[ $? == 0 ]]; then
        echo "Build successful"
        echo "Running ..."
        ./$PRJNAME
    else
        echo "Error in compiling"
    fi  
else 
    echo "Running ..."
    ./$PRJNAME
fi


