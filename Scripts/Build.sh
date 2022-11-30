#!/bin/bash -xe
clear

read -p "Do you want to run Fusion Editor after building? Hit Enter if not: " shouldRun

premake5 gmake2 --cc=clang
make -j -k && test -n $shouldRun && ./Run.sh

