
#include "tokens.h"
#include "console.h"

std::ostream& operator<<(std::ostream& stream, Token* token) {
	if(token->is(TT_Error)){
		stream << color(FG_Red) << "(Illegal Token:" << token->value << ")" << CONSOLE_RESET;
		return stream;
	}else if(token->is(TT_String)){
		stream << "(" << color(FG_Magenta) << "\"" << token->value << "\"" << CONSOLE_RESET << ")";
		return stream;
	}
	
	stream << "(";
		
	switch(token->type){
		// Value tokens
		case TT_Int: stream << color(FG_Cyan) << "Int" << CONSOLE_RESET; break;
		case TT_Word: stream << color(FG_Cyan) << "Word" << CONSOLE_RESET; break;
		case TT_Number: stream << color(FG_Cyan) << "Number" << CONSOLE_RESET; break;
		case TT_Keyword: stream << color(FG_Yellow) << "Keyword" << CONSOLE_RESET; break;
		// Control
		case TT_Dot: stream << "."; break;
		case TT_Colon: stream << ":"; break;
		case TT_Equals: stream << "="; break;
		case TT_Comma: stream << ","; break;
		case TT_SemiColon: stream << ";"; break;
		case TT_At: stream << "@"; break;
		case TT_Dollar: stream << "$"; break;
		case TT_Hash: stream << "#"; break;
		case TT_Question: stream << "?"; break;
		// Operators
		case TT_Plus: stream << "+"; break; 
		case TT_Increment: stream << "++"; break; 
		case TT_Add_Equals: stream << "+="; break;
		case TT_Minus: stream << "-"; break;
		case TT_Decrement: stream << "--"; break;
		case TT_Sub_Equals: stream << "-="; break;
		case TT_Mult: stream << "*"; break;
		case TT_Mul_Equals: stream << "*="; break;
		case TT_Div: stream << "/"; break;
		case TT_Div_Equals: stream << "/="; break;
		case TT_Mod: stream << "%"; break;
		case TT_Mod_Equals: stream << "%="; break;
		case TT_Xor: stream << "^"; break;
		case TT_Xor_Equals: stream << "^="; break;
		case TT_And: stream << "&"; break;
		case TT_And_Equals: stream << "&="; break;
		case TT_Logic_And: stream << "&&"; break;
		case TT_Or: stream << "|"; break;
		case TT_Or_Equals: stream << "|="; break;
		case TT_Logic_Or: stream << "||"; break;
		case TT_LShift: stream << "<<"; break;
		case TT_RShift: stream << ">>"; break;

		// Logical operator
		case TT_Logic_Equals: stream << "=="; break;
		case TT_Greater: stream << ">"; break;
		case TT_Less: stream << "<"; break;
		case TT_Greater_Equals: stream << ">="; break;
		case TT_Less_Equals: stream << "<="; break;
		case TT_Not: stream << "!"; break;
		case TT_Not_Equals: stream << "!="; break;
		
		// Brackets
		case TT_RBracket: stream << "')'"; break;
		case TT_LBracket: stream << "'('"; break;
		case TT_RSquareBracket: stream << "]"; break;
		case TT_LSquareBracket: stream << "["; break;
		case TT_RCurlyBracket: stream << "}"; break;
		case TT_LCurlyBracket: stream << "{"; break;
	}

	if(!token->value.empty()) stream << ":" << token->value;

	stream << ")";
	return stream;
}


std::ostream& operator<<(std::ostream& s, TokenReader& r) {
	s << "[";
	for(Token* tok : r.tokens){
		s << tok << ",";
	}
	s << "]";
	return s;
}
