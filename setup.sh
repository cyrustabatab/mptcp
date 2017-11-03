#!/usr/bin/env bash
#Setup everything in one go.

./waf distclean

#Build only specific modules
#CXXFLAGS="-Wall -g -O0 --std=c++11" time ./waf configure --enable-tests --enable-examples --enable-modules=internet,point-to-point,applications

#Using the .ns3rc file
#CXXFLAGS="-Wall -g -O0" time ./waf configure --enable-tests --enable-examples
CXXFLAGS="-Wall -g -O0 --std=c++11" time ./waf configure --enable-tests --enable-examples

./waf clean

time ./waf build

