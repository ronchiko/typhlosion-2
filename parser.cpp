
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
static result_t getter(TokenReader&);

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

static result_t callBrackets(result_t result, TokenReader& r, CallNode* callable){
	
	return result->success(callable);
}

static result_t id(TokenReader& r){
	result_t result = new ParseResult(r.current());
	cif(!r.current()->is(TT_Word)) return result->failure("Expected a word");
	
	token_t idToken = Secure(result->advance(r));
	chain_node_t refn = alloc.allocate(new RefrenceNode(idToken->value));

	cif(r.current()->matches(TT_LAngularBracket | TT_LBracket | TT_Dot | TT_LSquareBracket)){
		chain_node_t gettern = (chain_node_t)Secure(result->register_(getter(r)));
		refn->setn(gettern);
	}
	return result->success(refn);

}

static result_t getter(TokenReader& r) {
	result_t result = new ParseResult(r.current());
	
	CallNode* callable = nullptr;
	cswitch(r.current()->type){
		case TT_LAngularBracket: {
			Secure(result->advance(r));
		
			callable = alloc.allocate(new CallNode());
		
			token_t token = Secure(result->advance(r));
			callable->push_garg(token->value);

			cwhile(r.current()->is(TT_Comma)) {
				Secure(result->advance(r));

				cif(!r.current()->is(TT_Word)) return result->failure("Expected a word");

				token_t token = Secure(result->advance(r));
				callable->push_garg(token->value);
			}

			cif(!r.current()->is(TT_Greater)) return result->failure("Expected '>'");
			Secure(result->advance(r));
			cif(!r.current()->is(TT_LBracket)) return result->failure("Expected '('");
		}
	 	case TT_LBracket:{
			if(!callable) callable = alloc.allocate(new CallNode());
					 
			Secure(result->advance(r));
			
			/* Call with no arguments */
			if(!r.current()->is(TT_RBracket)) {
				/* Single argument */
				node_t ex = Secure(result->register_(expr(r)));
				callable->push_arg(ex);
	
				/* Many arguments */
				cwhile(r.current()->is(TT_Comma)){
					Secure(result->advance(r));
				
					node_t ex = Secure(result->register_(expr(r)));
					callable->push_arg(ex);
				}
			
				cif(!r.current()->is(TT_RBracket)) return result->failure("Expected ')'");
			}

			Secure(result->advance(r));

			cif(r.current()->matches(TT_LAngularBracket | TT_LBracket | TT_Dot | TT_LSquareBracket)){
				chain_node_t gettern = (chain_node_t)Secure(result->register_(getter(r)));
				callable->setn(gettern);
			}
			return result->success(callable);
		} break;
		case TT_Dot: {
			Secure(result->advance(r));
			token_t wordn = Secure(result->advance(r));
			chain_node_t refn = alloc.allocate(new AccessNode(wordn->value));
			
			cif(r.current()->matches(TT_LAngularBracket | TT_LBracket | TT_Dot | TT_LSquareBracket)){
				chain_node_t gettern = (chain_node_t)Secure(result->register_(getter(r)));
				refn->setn(gettern);
			}
			return result->success(refn);
		} break;
		default:
			return result->failure("Expected '[', '(', '<' or '.'."); 
	}
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
	
	}else cif(r.current()->matches(TT_Word)) /* WORD (INC | DEC)? */{
		node_t token = Secure(result->register_(id(r)));
		/* Check post operators */
		cif(r.current()->matches(TT_Increment | TT_Decrement)) {
			token_t op = Secure(result->advance(r));
			return result->success(alloc.allocate(new ProcessNode(token, op->type)));	
		}
		return result->success(token);
	}else cif(r.current()->matches(TT_Increment | TT_Decrement)) {
		token_t op = Secure(result->advance(r));

		/* Get the modifable operator */
		cif(!r.current()->is(TT_Word)) return result->failure("Expected an identifier");
		node_t ref = Secure(result->register_(id(r)));
		
		return result->success(alloc.allocate(new UnaryNode(op->type, ref)));
	}
	return result->failure("Expected a identifier, keyword, constant or '('", r.current());
}

static result_t factor(TokenReader& r){
	result_t result = new ParseResult(r.current());

	cif(r.current()->matches(TT_Plus | TT_Minus | TT_Not)) {
		token_t op = Secure(r.advance());
		node_t fact = Secure(result->register_(factor(r)));
		
		fact = Secure(alloc.allocate(new UnaryNode(op->type, fact)));

		return result->success(fact);
	}
	node_t a = Secure(result->register_(atom(r)));
	return result->success(a);
}

static result_t term(TokenReader& r){
	return chain_bin(r, &factor, TT_Mult | TT_Div | TT_Mod);
}

static result_t arith_expr(TokenReader& r) {
	return chain_bin(r, &term, TT_Plus | TT_Minus);
}

static result_t bitwise_expr(TokenReader& r) {
	return chain_bin(r, &arith_expr, TT_And | TT_Or | TT_Xor | TT_LShift | TT_RShift);
}

static result_t comp_expr(TokenReader& r){
	result_t result = new ParseResult(r.current());

	cif(r.current()->matches(TT_Logic_Not)){
		Secure(result->advance(r));

		node_t nnode = Secure(result->register_(comp_expr(r)));

		return result->success(new UnaryNode(TT_Logic_Not, nnode));
	}

	node_t left = Secure(result->register_(bitwise_expr(r)));

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
		node_t right = Secure(result->register_(bitwise_expr(r)));
		reinterpret_cast<ComparisonNode*>(cmpn)->push(t->type, right);
	}
	
	// Check if tring to use hybrid chains.
	cif(r.current()->matches(illegal)) return result->failure("Comparison chain must be of the same operator.", r.current());
		
	
	return result->success(cmpn);
}

static result_t logic_expr(TokenReader& r) {
	result_t result = new ParseResult(r.current());

	/*cif(r.current()->is(TT_Logic_Not)) {
		Secure(r.advance());
		node_t invert = Secure(result->register_(logic_expr(r)));

		invert = alloc.allocate(new UnaryNode(TT_Logic_Not, invert));	
		return result->success(invert);
	}*/
	node_t chain = Secure(result->register_(chain_bin(r, &comp_expr, TT_Logic_And | TT_Logic_Or)));
	return result->success(chain);
}

static result_t expr(TokenReader& r){
	result_t result = new ParseResult(r.current());
	bool isconst = false;
	cif(r.current()->keyword("const")){
		isconst = true;
		Secure(result->advance(r));
	}	
	/* Check if defined node */
	cif(r.current()->is(TT_Word)) {
		/* Get the setters values */
		token_t name = Secure(r.advance());
		std::string castto;
		cif(r.current()->is(TT_Colon)) {
			Secure(result->advance(r));
			cif(!r.current()->is(TT_Word) &&
					!r.current()->keyword("int", "float", "bool", "string"))
				return result->failure("Expected type specifier.");
			token_t typet = Secure(result->advance(r));
			castto = typet->value;
		}
		cif(r.current()->is(TT_Equals)) {
			Secure(r.advance());
			node_t expr_node = Secure(result->register_(logic_expr(r)));
				
			expr_node = alloc.allocate(new DefineNode(name->value, expr_node, castto));
			reinterpret_cast<DefineNode*>(expr_node)->isConst = isconst;
			return result->success(expr_node);
		}
		r.revert();
	}

	if(isconst) return result->failure("'const' must be followed by an variable declaration");

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
