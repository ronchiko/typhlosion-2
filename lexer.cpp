
#include "tokens.h"
#include "parser.h"

#include <iostream>
#include <sstream>

const int KEYWORD_COUNT = 3; 
const std::string* KEYWORDS = new std::string[KEYWORD_COUNT] { "const", "True", "False" };

/* Helper structure for reading a string */
struct CharReader {
	std::string& source;
	int line, index, i, lindex;
	
	CharReader(std::string& source) : source(source), line(0), index(0), i(0), lindex(0) {}

	char advance() {
		lindex = index;
		if(current() == '\n') { line++; index = 0; }
		else index++;

		i++;
		return current();
	}
	char revert() { 
		if(current() == '\n') { line--; index = lindex; }
		else index--;
		i--;
		return current();
	}
	char current() const { return source[i]; }
	bool hasnext() const { return i < source.length(); }
};

/* Helper functions */
inline static bool isdigit(char c) { return '0' <= c && c <= '9'; }
inline static bool isalpha(char c) { return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || c == '_'; }
inline static bool isletter(char c) { return isdigit(c) || isalpha(c); }

/* Checks if a word is a keyword */
inline static bool iskeyword(std::string word) {
	for(int i = 0; i < KEYWORD_COUNT; i++) if(word == KEYWORDS[i]) return true;
	return false; 
}

/* Token makers*/
inline static Token* token(CharReader& r, TokenType t) { return new Token(r.line, r.index, t); }
inline static Token* token(CharReader& r, TokenType t, std::string value) { return new Token(r.line, r.index, t, value); }
inline static Token* error(CharReader& r, char c) { return new Token(r.line, r.index, TT_Error, std::string(1, c)); }
/* Makes diffrent tokens under diffrent conditions */
static Token* if_next_make(CharReader& r, TokenType failure, char expected, TokenType success,
		char expected2 = 0, TokenType success2 = TT_Word){
	r.advance();
	if(r.current() == expected) return token(r, success);
	if(expected2 != 0 && r.current() == expected2) return token(r, success2);
	r.revert();
	return token(r, failure);
}
/* Makes a number token */
static Token* numtok(CharReader& r, bool intOnly = false){
	std::stringstream ss; int dcount = 0;
	int start = r.index;
	while(isdigit(r.current()) || (!intOnly && dcount <= 0 && r.current() == '.')){
		if(r.current() == '.') dcount++;
		ss << r.current();
		r.advance();
	}

	return new Token(r.line, start, dcount >= 1 ? TT_Number : TT_Int, ss.str());
}
/* Makes a word/keyword token */
static Token* wordtok(CharReader& r){
	std::stringstream ss;
	int start = r.index;
	while(isletter(r.current())) { ss << r.current(); r.advance(); }	
	return new Token(r.line, start, iskeyword(ss.str()) ? TT_Keyword : TT_Word, ss.str());
}

TokenReader& Lexer::getTokens(const std::string& source) {
	TokenReader& tokens = *new TokenReader();
	CharReader reader(const_cast<std::string&>(source));
	alloc.clear();

	while(reader.hasnext()){
		switch(reader.current()){
			/* Whitespace */
			case ' ': case '\f': case '\n': case '\r': break;
			
			/* Double conditions */
			case '<': tokens.push(alloc.allocate(
					if_next_make(reader, TT_Less, '=', TT_Less_Equals, '<', TT_LShift)));
				  break;
			case '>': tokens.push(alloc.allocate(
					if_next_make(reader, TT_Greater, '=', TT_Greater_Equals, '>', TT_RShift)));
				  break;
			case '+': tokens.push(alloc.allocate(
					if_next_make(reader, TT_Plus, '+', TT_Increment, '=', TT_Add_Equals))); 
				  break;
			case '-': tokens.push(alloc.allocate(
					if_next_make(reader, TT_Minus, '-', TT_Decrement, '=', TT_Sub_Equals)));
				  break;
			case '&': tokens.push(alloc.allocate(
					if_next_make(reader, TT_And, '&', TT_Logic_And, '=', TT_And_Equals)));
				  break;
			case '|': tokens.push(alloc.allocate(
					if_next_make(reader, TT_Or, '|', TT_Logic_Or, '=', TT_Or_Equals)));
				  break;
			
			/* Single Condition */
			case '^': tokens.push(alloc.allocate(
					if_next_make(reader, TT_Xor, '=', TT_Xor_Equals)));
				  break;
			case '*': tokens.push(alloc.allocate(
					if_next_make(reader, TT_Mult, '=', TT_Mul_Equals)));
				  break;
			case '/': tokens.push(alloc.allocate(
					if_next_make(reader, TT_Div, '=', TT_Div_Equals)));
				  break;
			case '%': tokens.push(alloc.allocate(
					if_next_make(reader, TT_Mod, '=', TT_Mod_Equals)));
				  break;
			case '=': tokens.push(alloc.allocate(
					if_next_make(reader, TT_Equals, '=', TT_Logic_Equals)));
				  break;
			case '!': tokens.push(alloc.allocate(
				  	if_next_make(reader, TT_Logic_Not, '=', TT_Not_Equals)));
				  break;			  
			/* No condition */
			case '(': tokens.push(alloc.allocate(token(reader, TT_LBracket))); break;
			case ')': tokens.push(alloc.allocate(token(reader, TT_RBracket))); break;

			case '[': tokens.push(alloc.allocate(token(reader, TT_LSquareBracket))); break;
			case ']': tokens.push(alloc.allocate(token(reader, TT_RSquareBracket))); break;

			case '{': tokens.push(alloc.allocate(token(reader, TT_LCurlyBracket))); break;
			case '}': tokens.push(alloc.allocate(token(reader, TT_RCurlyBracket))); break;

			case ':': tokens.push(alloc.allocate(token(reader, TT_Colon))); break;
			case ';': tokens.push(alloc.allocate(token(reader, TT_SemiColon))); break;
			case ',': tokens.push(alloc.allocate(token(reader, TT_Comma))); break;
		       	case '?': tokens.push(alloc.allocate(token(reader, TT_Question))); break;
			case '#': tokens.push(alloc.allocate(token(reader, TT_Hash))); break;
			case '@': tokens.push(alloc.allocate(token(reader, TT_At))); break;
			case '$': tokens.push(alloc.allocate(token(reader, TT_Dollar))); break;
			case '~': tokens.push(alloc.allocate(token(reader, TT_Not))); break; 
			/* Advanced condition */
			case '.': {
				char c = reader.advance();

				if(!isdigit(c))
				{ 
					tokens.push(alloc.allocate(token(reader, TT_Dot))); 
					reader.revert();
				}
				else{
					Token* token = numtok(reader, true);
					token->type = TT_Number;
					token->value = "0." + token->value;
					tokens.push(alloc.allocate(token));
					continue;
				}
			} break;
			case '"': {
				std::stringstream ss;
				int start = reader.index, sline = reader.line;
				reader.advance();
				while(reader.current() != '"'){
					ss << reader.current();
					reader.advance();
				}
				reader.advance();
				tokens.push(alloc.allocate(new Token(sline, start, TT_String, ss.str())));
			} continue;
			default:{
				if(isdigit(reader.current())) { 
					tokens.push(alloc.allocate(numtok(reader)));
					continue;
				}else if(isletter(reader.current())){
					tokens.push(alloc.allocate(wordtok(reader)));
					continue;
				}
				else tokens.push(alloc.allocate(error(reader, reader.current())));
			} break;
		}
		reader.advance();
	}

	return tokens;
}
