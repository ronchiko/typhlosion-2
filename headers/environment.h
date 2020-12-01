#pragma once

#include <string>
#include "allocator.h"
#include "types.h"
#include "error.h"

class TyphlosionEnv {
private:
	TyphAllocator allocator;
	typh_env parent;

	std::unordered_map<std::string, typh_type> typemap;
	std::unordered_map<std::string, typh_instance> instancemap;

public:
	static typh_float float_type;
	static typh_error error_type;

	TyphlosionEnv() : allocator(), parent(nullptr), typemap(), instancemap() {}
	TyphlosionEnv(typh_env p) : allocator(), parent(p), typemap(), instancemap(){}
	
	typh_instance call(std::string, typh_instance, typh_instance);
	typh_instance call(std::string, typh_instance);
	bool call(typh_comp_1a, typh_instance, typh_instance);
	
	/* adds a typh type to this types*/
	bool addt(std::string, typh_type);
	/* finds a typh_type that is defined in this env*/
	typh_type findt(std::string);
	
	/* adds a varaible to this env*/
	bool addi(std::string, typh_instance);
	/* finds a typh_instance if this env */
	typh_instance findi(std::string);

	typh_instance make(typh_type t, typh_instance_array a) { return allocator.allocate(t->mkn(this, nullptr, a)); }
	typh_instance make_float(float f) { return allocator.allocate(float_type->mkn(f)); }
	
	typh_instance make_err(const char* fmt...);
	/* return the upmost parent of this environment */
	typh_env upmost() const;

	std::string namet(typh_type);
	std::string namei(typh_instance);
};

