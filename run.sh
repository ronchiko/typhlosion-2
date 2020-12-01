#!/bin/bash

if  g++ main.cpp environment.cpp float.cpp error.cpp -o typhlosion -I headers
then
	if [ -f "signature.sig" ]
	then
		echo "Adding to git"
		git add .
	fi

	./typhlosion
fi
