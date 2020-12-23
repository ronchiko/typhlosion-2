#!/bin/bash

if g++ main.cpp console.cpp types.cpp environment.cpp tokens.cpp lexer.cpp parser.cpp error.cpp float.cpp int.cpp bool.cpp function.cpp strings.cpp defaults.cpp -o typhlosion -I headers
then
	if [ -f "signature.sig" ]
	then
		echo "Adding to git"
		git add .
	fi

	./typhlosion
fi
