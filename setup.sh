#!/usr/bin/env bash
#Setup everything in one go.

./waf distclean

#Build only specific modules
#CXXFLAGS="-Wall -g -O0 --std=c++11" time ./waf configure --enable-tests --enable-examples --enable-modules=internet,point-to-point,applications

#Using the .ns3rc file

if [[ `hostname | grep seas\.` ]]; then
    #For seasnet builds, don't use c++11
    echo "Running seasnet build"
    CXXFLAGS="-Wall -g -O0" time ./waf configure --enable-tests --enable-examples
else
    #For Ubuntu builds, use c++11:
    echo "Running non-seasnet build"
    CXXFLAGS="-Wall -g -O0 --std=c++11" time ./waf configure --enable-tests --enable-examples
fi

./waf clean

time ./waf build

