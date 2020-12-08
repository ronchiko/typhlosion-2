#pragma once

#include "allocator.h"
#include "tokens.h"
#include "error.h"
#include "environment.h"
#include "console.h"

struct ParserNode{
	virtual typh_instance compute(typh_env) = 0;
};

struct ParseResult {
	int line, index;
	bool failed;
	std::string error;
	ParserNode* node;

	ParseResult(Token* token) : line(token->line), index(token->index), failed(false), error(), node(nullptr) {  }

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
	inline Token* advance(TokenReader& r){
		if(r.end()) {
			failure("Unexepected end of line");
			return nullptr;
		}
		return r.advance();
	}

	ParseResult* failure(std::string err){
		failed = true;
		error = err;
		return this;
	}
	ParseResult* failure(std::string err, Token* t){
		line = t->line;
		index = t->index;
		return failure(err);
	}
	ParseResult* success(ParserNode* node) {
		if(!failed) this->node = node;
		return this;
	}

	friend std::ostream& operator<<(std::ostream& s, const ParseResult& r);
};

inline std::ostream& operator<<(std::ostream& s, const ParseResult& r){
	if(r.failed) s << color(FG_Red, CF_Bold) << "Error at (" << r.line << ", " << r.index << "): " << r.error << CONSOLE_RESET;
	else s << r.node;
	return s;
}

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
	
	ParseResult* parse(TokenReader&);
	ParseResult* parse(std::string&);
};
