#include "types.h"
#include "macros/types"

#define TYPE_CONST TyphlosionInt

#define float_type TyphlosionEnv::float_type

typh_int TyphlosionEnv::int_type = nullptr;

typh_instance cast_to_float(typh_env env, typh_instance a){
	return env->make_float((float)GETI(a));
}

typh_instance cast_to_bool(typh_env env, typh_instance a) {
	return env->make_bool(GETI(a) != 0);
}

TYPE_CONST::TYPE_CONST() {
	TyphlosionEnv::int_type = this;
	casts["float"] = &cast_to_float;
	casts["bool"] = &cast_to_bool;

	addm("self", [](typh_env env, typh_instance h) { return env->make_int(1);});
}

TyphFunc_CA(TYPE_CONST::mkn, typh_instance_array uargs) {
	TyphlosionInstanceArray& args = *uargs;
	if(args.cross(TyphlosionEnv::int_type)) return env->make_int(GETI(args[0]));
	else if(args.cross(float_type)) return env->make_int((int)GETF(args[0]));
	else if(args.cross()) return env->make_int(0);
	return env->make_err("Undecalred constructor");
}
TyphFunc_CA(TYPE_CONST::cll, typh_instance_array, typh_generic_array) {
	return env->make_err("'int' type is not callable");
}

FDef1A(add) {
	if(b->is(this)) return env->make_int(GETI(a) + GETI(b));
	else if(b->is(float_type)) return env->make_float((float)GETI(a) + GETF(b));
	OpError1A(add);
}

FDef1A(sub) {
	if(b->is(this)) return env->make_int(GETI(a) - GETI(b));
	else if(b->is(float_type)) return env->make_float((float)GETI(a) - GETF(b));
	OpError1A(sub);
}

FDef1A(mul) {
	if(b->is(this)) return env->make_int(GETI(a) * GETI(b));
	else if(b->is(float_type)) return env->make_float((float)GETI(a) * GETF(b));
	OpError1A(mul);
}

FDef1A(div) {
	if(b->is(this)) {
		int bi = GETI(b);
		if(bi == 0) return env->make_err("Division by zero");
		return env->make_int(GETI(a) / bi);
	}
	else if(b->is(float_type))  {
		float bf = GETF(b);
		if(bf == 0) return env->make_err("Division by zero");
		return env->make_float((float)GETI(a) / bf);
	}
	OpError1A(div);
}

FDef1A(mod) {
	if(b->is(this)) return env->make_int(GETI(a) % GETI(b));
	OpError1A(mod);
}

FDef0A(inv) {
	return env->make_int(-GETI(a));
}

FDef0A(inc) { int& x = GETI(a); x++; return a; }
FDef0A(dec) { int& x = GETI(a); x--; return a; }

NoOp1A(get)

FDef1A(an_) { if(b->is(this)) return env->make_int(GETI(a) & GETI(b)); OpError1A(and);  }
FDef1A(xr_) { if(b->is(this)) return env->make_int(GETI(a) ^ GETI(b)); OpError1A(xor);  }
FDef1A(or_) { if(b->is(this)) return env->make_int(GETI(a) | GETI(a)); OpError1A(or);   }
FDef1A(lsh) { if(b->is(this)) return env->make_int(GETI(a) << GETI(b)); OpError1A(lsh); }
FDef1A(rsh) { if(b->is(this)) return env->make_int(GETI(a) >> GETI(b)); OpError1A(rsh); }

FDef0A(nt_) { return env->make_int(~GETI(a)); }

FDef1A(cmp) {
	if(b->is(this)) return env->make_int((int)(GETI(a) - GETF(b)));
	OpError1A(comapre);
}

LDef {
	stream << GETI(a);
}
