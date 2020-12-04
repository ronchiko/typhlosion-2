#pragma once

#include "parser.h"
#include <vector>

typedef ParserNode* node_t;
typedef ParseResult* result_t;
typedef Token* token_t;

struct ConstantNode : public ParserNode {
	std::string value;
	typh_type ttps;

	ConstantNode(std::string v, typh_type t) : value(v), ttps(t) {}
	
	typh_instance compute(typh_env env) override {
		switch(typh_type){
			case TyphlosionEnv::float_type: return env->make_float(std::stof(value));
		}
		return env->make(ttps, value);
	}
};

struct DefineNode : public ParserNode {
	std::string name;
	node_t value;

	DefineNode(std::string name, node_t value) : name(name), value(value) {}

	typh_instance compute(typh_env env) override {
		return env->addi(name, value.compute(env));
	}
};

struct BinaryNode : public ParserNode {
	TokenType operation;
	node_t left, right;

	BinaryNode(node_t left, TokenType t, node_t right) : operation(t), left(left), right(right) {}

	typh_instance compute(typh_env env) override {
		switch(operation){
			case TT_Plus: return env->call("add", left->compute(env), right->compute(env));
			case TT_Minus: return env->call("sub", left->compute(env), right->compute(env));
			case TT_Mult: return env->call("mul", left->compute(env), right->compute(env));
			case TT_Div: return env->call("div", left->compute(env), right->compute(env));
			case TT_Mod: return env->call("mod", left->compute(env), right->compute(env));
			case TT_Logic_And: case TT_And: return env->call("and", left->compute(env), right->compute(env));
			case TT_Xor: return env->call("xor", left->compute(env), right->compute(env));
			case TT_Logic_Or: case TT_Or: return env->call("or", left->compute(env), right->compute(env));
		}
		return env->make_err("Illegal operation");
	}
};

struct UnaryNode : public ParserNode {
	TokenType operation;
	node_t node;

	UnaryNode(TokenType t, node_t right) : operation(t), node(right) {}
};

struct VectorNode : public ParserNode {
	std::vector<node_t> children;

	VectorNode() : children() {}
	VectorNode(node_t first) : children() {
		children.push_back(first);
	}

	inline void push(node_t item) { children.push_back(item); }

	typh_instance compute(typh_env env) override {
		for(node_t child : children) child->compute(env);
		return nullptr;
	}
};
