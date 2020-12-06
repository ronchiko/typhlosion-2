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
		Parser p;
		while(true) {
			std::cout << "> ";
			std::getline(std::cin, buffer);
			
			TokenReader& r = l.getTokens(buffer);

			if(r.current()->is(TT_Word, "quit")) break;

			ParseResult* pr = p.parse(r);
			if(pr->failed)
				std::cout << *pr << std::endl;
			else {
				typh_instance instance = pr->node->compute(env);
				std::cout << instance << std::endl;
			}
			delete &r;
		}
	}
};

