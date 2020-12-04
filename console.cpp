
#include <sstream>
#include <string>
#include "console.h"
#include <iostream>

std::string color(ColorCode f) {
	std::stringstream ss;
	ss << "\033[" << f << "m";
	return ss.str();
}

std::string color(ColorCode f, ColorCode s){
	std::stringstream ss;
	ss << "\033[" << f << ";" << s << "m";
	return ss.str();
}
std::string color(ColorCode first, ColorCode second, ColorCode third) {
	std::stringstream ss;
	ss << "\033[" << first << ";" << second << ";" << third << "m";
	return ss.str();
}

