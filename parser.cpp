
#include "allocator.h"
#include "parser.h"
#include "tokens.h"

#include "nodes.h"

#define Secure(op) op; if(result->failed) return result
#define CheckEnd if(r.end()) return result->failure("Unexpected end")
#define cwhile(cnd) CheckEnd; while(!r.end() && (cnd))
#define cif(cnd) CheckEnd; if(cnd)
#define cswitch(i) CheckEnd; switch(i)

static TyphAllocator& alloc = *new TyphAllocator();
static result_t expr(TokenReader&);

typedef result_t (*parse_func)(TokenReader&);

static result_t chain_bin(TokenReader& r, parse_func left, parse_func right, l_int tokens) {
	result_t result = new ParseResult(r.current());

	node_t leftmost = Secure(result->register_((*left)(r)));
	
	cwhile(r.current()->matches(tokens)) {
		token_t operation = Secure(r.advance());
			
		node_t rightmost = Secure(result->register_((*right)(r)));
		
		leftmost = alloc.allocate(new BinaryNode(leftmost, operation->type, rightmost));
		
	}
	return result->success(leftmost);
}

static result_t chain_bin(TokenReader& r, parse_func side, l_int tokens){
	return chain_bin(r, side, side, tokens);
}

static result_t atom(TokenReader& r) {
	result_t result = new ParseResult(r.current());
	cif(r.current()->is(TT_LBracket)) {
		Secure(result->advance(r));

		node_t expr_ = Secure(result->register_(expr(r)));
		
		cif(!r.current()->is(TT_RBracket)) return result->failure("Expected ')'");
		Secure(r.advance());
		return result->success(expr_);
	}else cif(r.current()->matches(TT_Number | TT_Int) ||
			r.current()->keyword("False", "True")) {
		token_t token = Secure(r.advance());
		return result->success(alloc.allocate(new ConstantNode(token->value, token->type)));
	}
	return result->failure("Expected a identifier, keyword, constant or '('", r.current());
}

static result_t factor(TokenReader& r){
	result_t result = new ParseResult(r.current());

	cif(r.current()->matches(TT_Plus | TT_Minus)) {
		token_t op = Secure(r.advance());
		node_t fact = Secure(result->register_(factor(r)));
		return result->success(fact);
	}
	node_t a = Secure(result->register_(atom(r)));
	return result->success(a);
}

static result_t term(TokenReader& r){
	return chain_bin(r, &factor, TT_Mult | TT_Div);
}

static result_t arith_expr(TokenReader& r) {
	return chain_bin(r, &term, TT_Plus | TT_Minus);
}

static result_t comp_expr(TokenReader& r){
	result_t result = new ParseResult(r.current());
	node_t left = Secure(result->register_(arith_expr(r)));

	cif(!r.current()->matches(TT_Not_Equals | TT_Logic_Equals | TT_Less | TT_Greater | TT_Less_Equals | TT_Greater_Equals))
		return result->success(left);
	
	node_t cmpn = alloc.allocate(new ComparisonNode(left));

	l_int mask = 0ULL;
	l_int illegal = TT_Logic_Equals | TT_Less | TT_Less_Equals | TT_Greater | TT_Greater_Equals | TT_Not_Equals;
	cswitch(r.current()->type) {
		case TT_Logic_Equals: mask = TT_Logic_Equals; break;
		case TT_Not_Equals: mask = TT_Not_Equals; break;
		case TT_Greater: case TT_Greater_Equals: mask = TT_Greater_Equals | TT_Greater; break;
		case TT_Less: case TT_Less_Equals: mask = TT_Less | TT_Less_Equals; break;
	}
	illegal &= ~mask;
	
	cwhile(r.current()->matches(mask)) {
		token_t t = Secure(r.advance());
	
		// Push to comparison node
		node_t right = Secure(result->register_(arith_expr(r)));
		reinterpret_cast<ComparisonNode*>(cmpn)->push(t->type, right);
	}
	
	// Check if tring to use hybrid chains.
	cif(r.current()->matches(illegal)) return result->failure("Comparison chain must be of the same operator.", r.current());
		
	
	return result->success(cmpn);
}

static result_t logic_expr(TokenReader& r) {
	result_t result = new ParseResult(r.current());

	cif(r.current()->is(TT_Logic_Not)) {
		Secure(r.advance());
		node_t invert = Secure(result->register_(logic_expr(r)));

		invert = alloc.allocate(new UnaryNode(TT_Logic_Not, invert));	
		return result->success(invert);
	}
	node_t chain = Secure(result->register_(chain_bin(r, &comp_expr, TT_Logic_And | TT_Logic_Or)));
	return result->success(chain);
}

static result_t expr(TokenReader& r){
	result_t result = new ParseResult(r.current());
	
	/* Check if defined node */
	cif(r.current()->is(TT_Word)) {
		/* Get the setters values */
		token_t name = Secure(r.advance());
		cif(r.current()->is(TT_Equals)) {
			Secure(r.advance());
			node_t expr_node = Secure(result->register_(logic_expr(r)));
			
			expr_node = alloc.allocate(new DefineNode(name->value, expr_node));
			return result->success(expr_node);
		}
		r.revert();
	}
	node_t node = Secure(result->register_(logic_expr(r)));
	return result->success(node);
}

static result_t complex_expr(TokenReader& r) {
	result_t result = new ParseResult(r.current());
	
	// Empty instruction
	cif(r.current()->is(TT_SemiColon)) {
		Secure(r.advance());
		return nullptr;
	}
	node_t node = Secure(result->register_(expr(r)));
	
	// Chained defenition
	cif(r.current()->is(TT_Comma)){
		node = alloc.allocate(new VectorNode(node));
		cwhile(r.current()->is(TT_Comma)){
			Secure(r.advance());
			node_t vec1 = Secure(result->register_(expr(r)));
			static_cast<VectorNode*>(node)->push(vec1);
		}
	}	

	cif(!r.current()->is(TT_SemiColon)) return result->failure("Expected a ';'");
	Secure(r.advance());
	return result->success(node);
}

result_t Parser::parse(TokenReader& r) {
	alloc.clear();
	// std::cout << "Parsing: " << r << std::endl;
	return complex_expr(r);
}

result_t Parser::parse(std::string& source){
	TokenReader& reader = lexer.getTokens(source);
	alloc.clear();

	return complex_expr(reader);
}
