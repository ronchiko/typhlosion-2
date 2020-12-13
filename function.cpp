
#include "macros/types"
#include "function.h"
#include "environment.h"

#include <sstream>

#define TYPE_CONST TyphlosionFunction

#define ANY nullptr

typedef TyphlosionFunction::FunctionHandle func_handle;

struct FunctionSignature {
	int generics;
	typh_type* params;	// where 0 is the return type
	int count;
	typh_func_c call_func;

	FunctionSignature(int generics, int count, typh_func_c fnc) : generics(generics), params(
			new typh_type[count+1]), count(count), call_func(fnc) {
		for(int i = 0; i <= count; i++) params[i] = ANY;
	}

	bool matches(typh_instance_array args, typh_generic_array gargs) {
		if (count != args->size() || generics != gargs->size()) return false;

		for(int i = 0; i < count; i++)
			if(params[i + 1] != ANY && args->operator[](i)->type() != params[i + 1]) return false;
		return true;
	}

	bool equals(FunctionSignature* other){
		if(other->generics != generics || count != other->count) return false;

		for(int i = 0; i < count; i++)
			if(params[i] != other->params[i]) return false;
		return true;
	}
};

typh_instance TYPE_CONST::FunctionHandle::try_call(typh_env e, typh_instance a, typh_instance_array args, 
		typh_generic_array gargs){
	for(auto& pair : override_callers){
		if(pair->matches(args, gargs)) return pair->call_func(e, a, args, gargs);
	}
	if(general_caller)
		return general_caller(e,a, args, gargs);
	return e->make_err("No matching override for function '%n'", a);
}

void TYPE_CONST::FunctionHandle::add_override(typh_func_c fnc, typh_generic_array args, int generics) {
	FunctionSignature *sig = new FunctionSignature(generics, args ? args->size() : 0, fnc);
	for(int i = 0; i < args->size(); i++) sig->params[i] = args->operator[](i);
	override_callers.push_back(sig);
}

typh_func TyphlosionEnv::func_type = nullptr;

TYPE_CONST::TYPE_CONST() {
	TyphlosionEnv::func_type = this;
}

TYPE_CONST::FunctionHandle::~FunctionHandle() {
	std::cout << "Deletes" << std::endl;
	for(auto& caller : override_callers){
		delete caller;
	}
}

LDef {
	stream << "function<?>(...)";
}

FDef1A(cmp) {
	return env->make_int(a - b);
}

TyphFunc_CA(TYPE_CONST::mkn, typh_instance_array) { return env->make_err("Lambda functions aren't supported yet"); }
TyphFunc_CA(TYPE_CONST::cll, typh_instance_array args, typh_generic_array gargs) {
	auto& handle = *reinterpret_cast<TYPE_CONST::FunctionHandle*>(a->data());
	return handle.try_call(env, a, args, gargs);
}

typh_instance makeGeneralHandle(typh_func_c fnc){
	typh_instance handle = make(TyphlosionEnv::func_type, new TYPE_CONST::FunctionHandle());
	reinterpret_cast<TYPE_CONST::FunctionHandle*>(handle->data())->general_caller = fnc;
	return handle;
}
