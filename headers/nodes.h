#pragma once

#include "parser.h"
#include <vector>

typedef ParserNode* node_t;
typedef ParseResult* result_t;
typedef Token* token_t;

#define TYPH_TRUE env->bool_type->typh_True
#define TYPH_FALSE env->bool_type->typh_False

#define TYPH_BOOL(cmp) (cmp) ? TYPH_TRUE : TYPH_FALSE

struct ConstantNode : public ParserNode {
	std::string value;
	TokenType type;

	ConstantNode(std::string v, TokenType t) : value(v), type(t) {}
	
	typh_instance compute(typh_env env) override {
		switch(type){
			case TT_Int:	return env->make_int(std::stoi(value));
			case TT_Number: return env->make_float(std::stof(value));
			case TT_Keyword:
				if(value == "True") return env->make_bool(true);
				else if(value == "False") return env->make_bool(false);
				break;
		}
		return env->make_err("Unknown type constant");
	}
};

struct DefineNode : public ParserNode {
public:
	std::string name;
	node_t value;
	std::string force_type;
	bool isConst;

	DefineNode(std::string name, node_t value, std::string t) : name(name), value(value), force_type(t), isConst(false) {}
	
	typh_instance compute(typh_env env) override {
		bool defined =  !env->findi(name)->is(TyphlosionEnv::error_type);
		if(isConst && defined){
			return env->make_err("Cannot mark a declared value as constant");
		}

		typh_instance inst = value->compute(env);

		if(inst->is(env->error_type) && force_type != "error") return inst;

		if(mustCast() && inst->type() != env->findt(force_type)) {
			inst = env->cast(inst, force_type); 
		}
		if(isConst) {
			inst->setFlags(INF_Const);
		}
		return env->addi(name, inst);
	}
private:
	inline bool mustCast() const { return !force_type.empty(); }
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
			case TT_RShift: return env->call("rsh", left->compute(env), right->compute(env));
			case TT_LShift: return env->call("lsh", left->compute(env), right->compute(env));
		}
		return env->make_err("Illegal operation");
	}
};

struct UnaryNode : public ParserNode {
	TokenType operation;
	node_t node;

	UnaryNode(TokenType t, node_t right) : operation(t), node(right) {}

	typh_instance compute(typh_env env) override {
		switch(operation) {
			case TT_Minus: return env->call("inv", node->compute(env));
			case TT_Plus: return node->compute(env);
			case TT_Logic_Not: case TT_Not: return env->call("not", node->compute(env));
			case TT_Increment: return env->call("inc", node->compute(env));
			case TT_Decrement: return env->call("dec", node->compute(env));
		}
		return env->make_err("Illegal unary operation");
	}
};

struct ComparisonNode : public ParserNode {
	std::vector<node_t> nodes;
	std::vector<TokenType> ops;

	inline ComparisonNode(node_t first) : nodes(), ops() { nodes.push_back(first); }
	
	inline float cmp(typh_env env, int i) { return env->cmp(nodes[i]->compute(env), nodes[i+1]->compute(env)); }
	inline void push(TokenType t, node_t n) { nodes.push_back(n); ops.push_back(t); }

	typh_instance compute(typh_env env) override {
		bool final_ = true;
		for(int i = 0; final_ && i < ops.size(); i++){
			switch(ops[i]) {
				case TT_Less: final_ &= cmp(env, i) < 0; break;
				case TT_Greater: final_ &= cmp(env, i) > 0; break;
				case TT_Logic_Equals: final_ &= cmp(env, i) == 0; break;
				case TT_Not_Equals: final_ &= cmp(env, i) != 0; break;
				case TT_Less_Equals: final_ &= cmp(env, i) <= 0; break;
				case TT_Greater_Equals: final_ &= cmp(env, i) >= 0; break;
				default: return env->make_err("Illegal comparison operation");
			}
		}
		return TYPH_BOOL(final_);
	}
};

struct RefrenceNode : public ParserNode {
	std::string name;

	RefrenceNode(std::string& s) : name(s) {}
	
	typh_instance compute(typh_env env) {
		return env->findi(name);
	}
};

struct ProcessNode : public ParserNode {
	TokenType t;
	std::string ref;

	ProcessNode(std::string ref, TokenType t) : ref(ref), t(t) {}

	typh_instance compute(typh_env env) {
		typh_instance var = env->findi(ref);
		if(var == nullptr) return env->make_err("No variable known as '%s'.", ref);
		typh_instance p = env->copy(var);	
		switch(t) {
			case TT_Increment: env->call("inc", var); break;
			case TT_Decrement: env->call("dec", var); break;
			default: return env->make_err("Illegal operation");
		}
		return p;
	}
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
