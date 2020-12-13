
#include "types.all"
#include "environment.h"

#define iGet(i) operator[](i)

typh_instance cast_func(typh_env env, typh_instance a, typh_instance_array args, typh_generic_array gargs) {
	if(args->size() != 1 || gargs->size() != 1) 
		return env->make_err("Cast call takes 1 generic argument and 1 instance argument");
	
	if(args->iGet(0)->is(env->error_type)) return args->iGet(0);
	
	std::string name = env->namet(gargs->iGet(0));
	return env->cast(args->iGet(0), name);
}

typh_instance test_func(typh_env env, typh_instance a, typh_instance_array args, typh_generic_array gargs) {
	return env->make_int(20);
}

void bindDefaults(typh_env e) {	
	e->addi("cast", makeGeneralHandle(&cast_func));
	e->addi("test", makeGeneralHandle(&test_func));
}
