#pragma once

#include <string>
#include <iostream>

#include "environment.h"
#include "parser.h"

class TyphlosionShell {
private:
	std::string buffer;
	typh_env env;
public:
	TyphlosionShell(typh_env env) : buffer(), env(env) {}
	
	void start() {
		Lexer l;
		while(true) {
			std::cout << "> ";
			std::getline(std::cin, buffer);
			
			TokenReader& r = l.getTokens(buffer);
			std::cout << r << std::endl;

			if(r.current()->is(TT_Word, "quit")) break;

			delete &r;
		}
	}
};

