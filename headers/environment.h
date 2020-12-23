#pragma once

#include <string>
#include <functional>

#include "allocator.h"
#include "types.h"
#include "error.h"

const int EF_NoVar = 1 << 0;

class TyphlosionFunction;
typedef TyphlosionFunction* typh_func;

class TyphlosionString;
typedef TyphlosionString* typh_string;

typedef std::function<typh_instance(typh_env, typh_instance, typh_instance_array, typh_generic_array)> typh_func_c;

class TyphlosionEnv {
private:
	TyphAllocator allocator;
	typh_env parent;
	int flags;

	std::unordered_map<std::string, typh_type> typemap;
	std::unordered_map<std::string, typh_instance> instancemap;

public:
	static typh_float float_type;
	static typh_error error_type;
	static typh_bool bool_type;
	static typh_int int_type;
	static typh_func func_type;
	static typh_string string_type;

	TyphlosionEnv() : allocator(), parent(nullptr), flags(), typemap(), instancemap() {}
	TyphlosionEnv(typh_env p) : allocator(), parent(p), flags(), typemap(), instancemap() {}
	TyphlosionEnv(typh_env p, int flags) : allocator(), parent(p), flags(flags), typemap(), instancemap() {}
	
	/* Call functions */
	typh_instance call(std::string, typh_instance, typh_instance);
	typh_instance call(std::string, typh_instance);
	float cmp(typh_instance, typh_instance);
	typh_instance call(typh_instance, typh_instance_array, typh_generic_array);
	/* adds a typh type to this types*/
	bool addt(std::string, typh_type);
	/* finds a typh_type that is defined in this env*/
	typh_type findt(std::string);
	
	/* adds a varaible to this env*/
	typh_instance addi(std::string, typh_instance);
	/* finds a typh_instance if this env */
	typh_instance findi(std::string);
	
	/* Copies an item */
	inline typh_instance copy(typh_instance a) { return a ? allocator.allocate(a->copy()) : nullptr; }
	inline typh_instance cast(typh_instance a, std::string& type) { 
		return allocator.allocate(a->type()->cast(this, a, type));
       	}
	inline typh_instance access(typh_instance a, std::string member) {
		return allocator.allocate(a->type()->access(this, a, member));
	}
	inline typh_instance make(typh_type t, typh_instance_array a) { return allocator.allocate(t->mkn(this, nullptr, a)); }
	inline typh_instance make_float(float f) { return allocator.allocate(float_type->mkn(f)); }
	inline typh_instance make_int(int i) { return allocator.allocate(int_type->mkn(i)); }
	typh_instance make_err(const char* fmt...);
	inline typh_instance make_bool(bool b) { return allocator.allocate(b ? bool_type->typh_True : bool_type->typh_False); } 
	typh_instance make_str(std::string str);
	/* return the upmost parent of this environment */
	typh_env upmost() const;

	/* Name search functions */
	std::string namet(typh_type);
	std::string namei(typh_instance);
	
	inline typh_instance allocate(typh_instance t) { 	
		return allocator.allocate(t);
       	}

	typh_instance make_handle(typh_func_c fnc, typh_generic_array ga, int gen);
};

typh_env initRootEnvironment();
void bindDefaults(typh_env);

void query_func_handle(typh_instance&, typh_func_c, typh_generic_array ga, int gen);
