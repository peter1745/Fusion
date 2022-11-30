#!/bin/bash -xe
cat LinuxPackages.txt | while read package || [[ -n $package ]]
do
	sudo apt install $package -y
done

