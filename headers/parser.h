#pragma once

#include "allocator.h"
#include "tokens.h"
#include "error.h"
#include "environment.h"

struct ParserNode{
	virtual typh_instance compute(typh_env) = 0;
};

struct ParseResult {
	int line, index;
	bool failed;
	std::string error;
	ParserNode* node;

	ParseResult(Token& token) : line(token.line), index(token.index), failed(false), error(), node(nullptr) {  }

	ParserNode* register_(ParseResult* other){
		if(other->failed) {
			failed = true;
			line = other->line;
			index = other->index;
			error = other->error;
		}
		ParserNode* nd = other->node;
		delete other;
		return nd;
	}
	ParseResult* failure(std::string err){
		failed = true;
		error = err;
		return this;
	}
	ParseResult* success(ParserNode* node) {
		if(!failed) this->node = node;
		return this;
	}
};

class Lexer {
private:
	TyphAllocator alloc;
public:
	Lexer() : alloc() {}

	TokenReader& getTokens(const std::string&);
};

class Parser {
private:
	TyphAllocator alloc;
	Lexer lexer;	
public:
	Parser() : alloc(), lexer() {}

	ParseResult* parse(std::string& code);
};
