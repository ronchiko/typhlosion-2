#!/bin/bash

if  g++ main.cpp environment.cpp float.cpp error.cpp lexer.cpp tokens.cpp int.cpp types.cpp function.cpp strings.cpp defaults.cpp console.cpp bool.cpp parser.cpp -o typhlosion -I headers
then
	if [ -f "signature.sig" ]
	then
		echo "Adding to git"
		git add .
	fi

	./typhlosion
fi
