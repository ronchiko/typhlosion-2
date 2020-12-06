#pragma once

#include <string>
#include "allocator.h"
#include "types.h"
#include "error.h"

const int EF_Temporary = 1 << 0;

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

	TyphlosionEnv() : allocator(), parent(nullptr), flags(), typemap(), instancemap() {}
	TyphlosionEnv(typh_env p) : allocator(), parent(p), flags(), typemap(), instancemap() {}
	TyphlosionEnv(typh_env p, int flags) : allocator(), parent(p), flags(flags), typemap(), instancemap() {}
	
	/* Call functions */
	typh_instance call(std::string, typh_instance, typh_instance);
	typh_instance call(std::string, typh_instance);
	float cmp(typh_instance, typh_instance);
	
	/* adds a typh type to this types*/
	bool addt(std::string, typh_type);
	/* finds a typh_type that is defined in this env*/
	typh_type findt(std::string);
	
	/* adds a varaible to this env*/
	typh_instance addi(std::string, typh_instance);
	/* finds a typh_instance if this env */
	typh_instance findi(std::string);

	inline typh_instance make(typh_type t, typh_instance_array a) { return allocator.allocate(t->mkn(this, nullptr, a)); }
	inline typh_instance make_float(float f) { return allocator.allocate(float_type->mkn(f)); }	
	inline typh_instance make_int(int i) { return make_float(static_cast<float>(i)); }
	typh_instance make_err(const char* fmt...);
	inline typh_instance make_bool(bool b) { return allocator.allocate(b ? bool_type->typh_True : bool_type->typh_False); } 

	/* return the upmost parent of this environment */
	typh_env upmost() const;

	/* Name search functions */
	std::string namet(typh_type);
	std::string namei(typh_instance);
};

