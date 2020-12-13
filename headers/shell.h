#pragma once

#include <string>
#include <iostream>

#include "types.h"
#include "environment.h"
#include "parser.h"

class TyphlosionShell {
private:
	std::string buffer;
	typh_env env;
public:
	TyphlosionShell(typh_env env) : buffer(), env(env) {}
	
	void start(void) {
		Lexer l;
		Parser p;
		while(true) {
			TyphlosionEnv e(env, EF_NoVar);
			std::cout << "> ";
			std::getline(std::cin, buffer);
			
			TokenReader& r = l.getTokens(buffer);

			if(r.current()->is(TT_Word, "quit")) break;

			ParseResult* pr = p.parse(r);
			if(pr->failed)
				std::cout << *pr << std::endl;
			else if(pr->node){
				typh_instance instance = pr->node->compute(&e);
				if(instance != nullptr) { 
					instance->type()->log(std::cout, instance, env);
					std::cout << std::endl;
				}
			}
			delete &r;
		}
	}
};

