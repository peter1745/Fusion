#!/bin/bash -xe

# Install required apt packages
cat LinuxPackages.txt | while read package || [[ -n $package ]]
do
	sudo apt install $package -y
done

