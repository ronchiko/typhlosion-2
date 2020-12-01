
#include "environment.h"
#include "types.h"

#include <stdarg.h>
#include <iostream>
#include <sstream>

typh_float TyphlosionEnv::float_type = nullptr;
typh_error TyphlosionEnv::error_type = nullptr;

typedef TyphlosionError TyphErr;

#define CALL_OP1(name) a->type()->name(e, a, b) 
#define CALL_OP0(name) a->type()->name(e, a)

constexpr unsigned int str2int(const char* str, int h = 0){
	return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

typh_instance call_function(std::string f, typh_instance a, typh_instance b, typh_env e){
	switch(str2int(f.c_str())){
		case str2int("add"): return CALL_OP1(add);
		case str2int("sub"): return CALL_OP1(sub);
		case str2int("log"): a->type()->log(std::cout, a);
				     return nullptr;
		default: return nullptr;
	}
}

typh_instance TyphlosionEnv::call(std::string f, typh_instance a, typh_instance b) {
	typh_instance o = call_function(f, a, b, this);
	return o;
}

typh_instance TyphlosionEnv::call(std::string f, typh_instance a){
	typh_instance o = call_function(f, a, nullptr, this);
	return o;
}

bool TyphlosionEnv::call(typh_comp_1a f, typh_instance a, typh_instance b){
	return (a->type()->*f)(this, a, b);
}

/* Gets the upmost parent of a env recusively */
typh_env TyphlosionEnv::upmost() const { return parent == nullptr ? const_cast<typh_env>(this) : parent->upmost(); }

typh_type TyphlosionEnv::findt(std::string name) {
	/* Check if is a primitive type */
	switch(str2int(name.c_str())){
		case str2int("float"): return float_type;
		case str2int("error"): return error_type;
	}
	
	/* search in custom types */
	if(typemap.find(name) == typemap.end()) { 
		if(parent == nullptr) return nullptr; //make_err("No type known as %s.", name);
		return parent->findt(name);
	}
	return typemap[name];
}

typh_instance TyphlosionEnv::findi(std::string name) {
	if(instancemap.find(name) == instancemap.end()) {
		if(parent == nullptr) return make_err("No instance known as %s.", name);
		return parent->findi(name);
	}
	return instancemap[name];
}

typh_instance TyphlosionEnv::make_err(const char* fmt...){
	std::stringstream ss;
	
	va_list args;
	va_start(args, fmt);

	while(*fmt != '\0'){
		switch(*fmt){
			case '%':
				++fmt;
				switch(*fmt){
					case 's':{
						const char* str = va_arg(args, const char*);
						ss << std::string(str);
						 }break;
					case 'i':{
						int inum = va_arg(args, int);
						ss << inum;
						 }break;
					case '%': ss << '%'; break;
				}
				break;
			default: ss << *fmt;
		}
		++fmt;
	}
	va_end(args);
	return allocator.allocate(error_type->mkn(ss.str()));
}

std::string TyphlosionEnv::namet(typh_type t) {

	if(t == float_type) return "float";
	if(t == error_type) return "error";

	for(auto& i : typemap){
		if(i.second == t) return i.first;
	}
	if(parent == nullptr) return std::string();
	return parent->namet(t);
}

std::string TyphlosionEnv::namei(typh_instance t){
	for(auto& i : instancemap){
		if(t == i.second) return i.first;
	}
	if(parent == nullptr) return std::string();
	return parent->namei(t);
}
