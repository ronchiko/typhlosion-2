#pragma once

#include <string>
#include <iostream>

class TyphlosionShell {
private:
	std::string buffer;
public:
	TyphlosionShell() : buffer() {}
	
	void start() {
		while(true) {
			std::cout << "> ";
			std::getline(std::cin, buffer);
			std::cout << buffer << std::endl;
		}	
	}
};

