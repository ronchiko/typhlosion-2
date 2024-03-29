
#include "types.all"
#include "typh_strings.h"

#include <sstream>

#define TYPE_CONST TyphlosionString

typh_string TyphlosionEnv::string_type = nullptr;

typh_instance str_length(typh_env env, typh_instance a, typh_instance_array args, typh_generic_array gargs) {	
	return env->make_size((GETS(a)).length());
}

struct StringMembers {
	static typh_instance length_func;
};
typh_instance StringMembers::length_func = nullptr;

TYPE_CONST::TYPE_CONST() : TyphlosionType() {
	TyphlosionEnv::string_type = this;
	
	query_func_handle(StringMembers::length_func, &str_length, nullptr, 0);

	addm("length", [](typh_env e, typh_instance a) { return e->copy(StringMembers::length_func); });
}

TyphFunc_1A(TYPE_CONST::mul) {
	if(b->is(TyphlosionEnv::int_type) || b->is(TyphlosionEnv::float_type)) {
		std::string &source = GETS(a);
		std::stringstream ss;
		int rep = b->is(TyphlosionEnv::float_type) ? (int)GETF(b) : GETI(b);
		if(rep < 0) return env->make_err("Cannot multiply a 'string' by a negative number");
		
		for(int i = 0; i < rep; i++) {
			ss << a;
		}

		return env->make_str(ss.str());
	}
	OpError1A(mul);
}

TyphFunc_1A(TYPE_CONST::add) {
	if(b->is(env->error_type)) return b;
	std::stringstream ss;
	ss << GETS(a);
	b->type()->log(ss, b, env);
	return env->make_str(ss.str());
}

TyphFunc_1A(TYPE_CONST::cmp) {
	if(b->is(this)) {
		std::string& as = GETS(a), &bs = GETS(b);

		if(as.length() != bs.length()) return env->make_size(as.length() - bs.length());
		
		for(int i = 0; i < as.length(); i++) {
			if(as[i] != bs[i]) return env->make_int(static_cast<int>(as[i] - bs[i]));
		}
		return env->make_int(0);
	}
	return env->make_size(a - b);
}

TyphFunc_CA(TYPE_CONST::mkn, typh_instance_array args){
	if(args->size() == 0) return env->make_str("");
	return env->make_err("No 'string' constuctor ('%i')", args->size());
}	

LDef {
	stream << GETS(a);
}
