#pragma once

enum ColorCode : short int {
	FG_Black = 30,
	FG_Red = 31,
	FG_Green = 32,
	FG_Yellow = 33,
	FG_Blue = 34,
	FG_Magenta = 35,
	FG_Cyan = 36,
	FG_White = 37,

	BG_Black = 40,
	BG_Red = 41,
	BG_Green = 42,
	BG_Yellow = 43,
	BG_Blue = 44,
	BG_Magenta = 45,
	BG_Cyan = 46,
	BG_White = 47,

	CF_Invert = 7,
	CF_Reset = 0,
	CF_BoldBright = 1,
	CF_Underline = 4,
	CF_NoBoldBright = 21,
	CF_NoUderline = 24,
	CF_NoInvert = 27	
};

std::string color(ColorCode f);
std::string color(ColorCode f, ColorCode s);
std::string color(ColorCode first, ColorCode second, ColorCode third);
const  std::string CONSOLE_RESET = "\033[0m";
