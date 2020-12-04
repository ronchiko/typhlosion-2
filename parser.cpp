
#include "allocator.h"
#include "parser.h"
#include "tokens.h"

#include "nodes.h"

static TyphAllocator& alloc;

typedef result_t (*parse_func)(TokenReader&);

static result_t chain_bin(TokenReader& r, parse_func left, parse_func right, l_int tokens) {
	result_t result = new ParseResult();

	node_t leftmost = result->register_((*left)(r));
	if(result->failed) return result;

	while(r.current().matches(tokens)){
		Token* operation = r.advance();
		
		node_t rightmost = result->register_((*right)(r));
		if(result->failed) return result;

		leftmost = alloc.allocate(new BinaryNode(leftmost, operation->type, rightmost));
	}
	return result->success(leftmost);
}

static result_t chain_bin(TokenReader& r, parse_func side, l_int tokens){
	return chain_bin(r, side, side, tokens)
}

static result_t logic_expr(TokenReader& r) {
	if(r.current() == TT_Logic_Not) {
		r.advance();
		result_t result = new ParseResult();

		node_t invert = result->register_(logic_expr(r));
		if(result->failed) return result;
		
		return result->success(invert);
	}
	return chain_bin(r, comp_expr, TT_Logic_And | TT_Logic_Or)
}

static result_t expr(TokenReader& r){
	result_t result = new ParseResult();
	
	/* Check if defined node */
	if(r.current.is(TT_Word)) {
		/* Get the setters values */
		token_t name = r.advance();
		if(r.current().is(TT_Equals)) {
			node_t expr_node = result->register_(logic_expr(r));
			if(result->failure) return result;
			expr_node = alloc.allocate(new DefineNode(name->value, expr_node));
			return result->success(expr_node);
		}
		r.revert();
	}
	node_t node = result->register_(logic_expr(r));
	return result->success(node);
}

static result_t complex_expr(TokenReader& r) {
	result_t result = new ParseResult();
	
	// Empty instruction
	if(r.current().is(TT_SemiColon)) {
		r.advance();
		return nullptr;
	}

	node_t node = result->register_(expr(r));
	if(result->failed) return result;
	
	// Chained defenition
	if(r.current().is(TT_Comma)){
		node = alloc.allocate(new VectorNode(node));
		while(r.current().is(TT_Comma)){
			r.advance();
			static_cast<VectorNode*>(node.add)(result->register_(expr(r)));
			if(result->failed) return result;
		}
	}	

	if(!r.current().is(TT_SemiColon)) return result->failure("Expected a ';'");
	r.advance();
	return result->success(node);
}

result_t Parser::parse(std::string& source){
	TokenReader& reader = lexer.getTokens(source);
	this->alloc.clear();
	alloc = this->alloc;

	return complex_expr(reader);
}
