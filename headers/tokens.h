#pragma once

#include <vector>
#include <iostream>

typedef unsigned long long int l_int;

/* All the possible token types */
enum TokenType : l_int {
	// Basic tokens
	TT_Word = 1ULL << 0,
	TT_Keyword = 1ULL << 1,
	TT_String = 1ULL << 2,
	TT_Number = 1ULL << 3,
	TT_Int = 1ULL << 4,
	TT_Colon = 1ULL << 5,
	TT_Dot = 1ULL << 6,
	TT_Comma = 1ULL << 7,
	TT_SemiColon = 1ULL << 8,

	TT_Error = 1ULL << 9,

	// Arithmatic tokens
	TT_Plus = 1ULL << 10,
	TT_Minus = 1ULL << 11,
	TT_Mult = 1ULL << 12,
	TT_Div = 1ULL << 13,
	TT_LBracket = 1ULL << 14,
	TT_RBracket = 1ULL << 15,
	TT_Equals = 1ULL << 16,
	
	// Bitwise logic operators
	TT_And = 1ULL << 17,
	TT_Or = 1ULL << 18,
	TT_Xor = 1ULL << 19,
	
	// Logical tokens
	TT_Logic_Equals = 1ULL << 20,
	TT_Logic_And = 1ULL << 21,
	TT_Logic_Or = 1ULL << 22,
	TT_Greater = 1ULL << 23, 	TT_RAngularBracket = 1ULL << 23,
	TT_Less = 1ULL << 24,		TT_LAngularBracket = 1ULL << 24,
	TT_NotEquals = 1ULL << 25,
	TT_Greater_Equals = 1ULL << 26,
	TT_Less_Equals = 1ULL << 27,

	// Bitwise operators
	TT_Not = 1ULL << 28,
	TT_RShift = 1ULL << 29,
	TT_LShift = 1ULL << 30,

	// Brackets
	TT_LCurlyBracket = 1ULL << 31,
	TT_RCurlyBracket = 1ULL << 32,
	TT_LSquareBracket = 1ULL << 33,
	TT_RSquareBracket = 1ULL << 34,
	
	// Misc
	TT_Mod = 1ULL << 35,
	TT_Dollar = 1ULL << 36,
	TT_Question = 1ULL << 37,
	TT_Hash = 1ULL << 38,
	TT_At = 1ULL << 39,
	
	// Incrementors
	TT_Increment = 1ULL << 40,
	TT_Decrement = 1ULL << 41,
	
	// Modifiers
	TT_And_Equals = 1ULL << 42,
	TT_Or_Equals = 1ULL << 43,
	TT_Xor_Equals = 1ULL << 44,
	TT_Add_Equals = 1ULL << 45,
	TT_Sub_Equals = 1ULL << 46,
	TT_Mul_Equals = 1ULL << 47,
	TT_Div_Equals = 1ULL << 48,
	TT_Mod_Equals = 1ULL << 49,
	TT_Not_Equals = 1ULL << 50,

	TT_Logic_Not = 1ULL << 51
};


/* Represents a token */
struct Token {
	TokenType type;
	std::string value;

	int line, index;
	
	Token(int line, int index, TokenType type, std::string value) : type(type), value(value), line(line), index(index) {}
	Token(int line, int index, TokenType type) : type(type), value(), line(line), index(index) {}
	
	/* Checks the tokens type */
	inline bool is(TokenType type) const { return type == this->type; }
	inline bool is(TokenType type, std::string value) const { return is(type) && this->value == value; }
	/* Checks if the token matches one of the types */
	inline bool matches(l_int types) const { return types & (l_int)type; }
	/* Checks if this token is a specific keyword */
	inline bool keyword(std::string word) const { return is(TT_Keyword) && value == word; }
	template<typename... Ts>
	bool keyword(std::string word, Ts... b) const {
		if(!is(TT_Keyword)) return false;
		return keyword(word) ? true : keyword(b...);
	}
};

/* Used to make chain of tokens, and to read them */
struct TokenReader {
	std::vector<Token*> tokens;
	int index;

	TokenReader() : tokens(), index(0) {}
	
	/* Adds a token to the chain */
	void push(Token* t) { tokens.push_back(t); }
	
	/* Advances to the next token in the chain, and returns the previus token */
	inline Token* advance() {
		Token* prev = current();
		if(index < tokens.size()) index++;
		return prev;
	}
	/* Reverts to the previus token in the chain, return the last token that was pointed to */
	inline Token* revert() {
		Token* prev = current();
		if(index > 0) index--;
		return prev;
	}

	/* Gets the current token pointed to by the reader */ 
	inline Token* current() const { return tokens[index]; }
	/* Is the reader at its end */ 
	inline bool end() const { return index < tokens.size(); }

	friend std::ostream& operator<<(std::ostream&, TokenReader&);
};


std::ostream& operator<<(std::ostream& stream, Token* token);
std::ostream& operator<<(std::ostream& s, TokenReader& r);
