
#include "types.h"
#include "environment.h"
#include "error.h"

#include "macros/types"

#define flt TyphlosionFloat
#define mkerr(msg) env->make_err(msg, env->namet(a->type()).c_str(), env->namet(b->type()).c_str())

#define isflt env->float_type == b->type()
#define isint b->is(env->int_type)	

typh_instance cast_to_int(typh_env env, typh_instance o){
	return env->make_int((int)GETF(o));
}

TyphlosionFloat::TyphlosionFloat() {
	TyphlosionEnv::float_type = this;

	casts["int"] = &cast_to_int;
}

TyphFunc_1A(TyphlosionFloat::add) {
	if(isflt) return env->make_float(GETF(a) + GETF(b));
	if(isint) return env->make_float(GETF(a) + GETI(b));
	return mkerr("Cannot add '%s' and '%s'");
}
TyphFunc_1A(TyphlosionFloat::sub) {
	if(isflt) return env->make_float(GETF(a) - GETF(b));
	if(isint) return env->make_float(GETF(a) - GETI(b));
	return mkerr("Cannot subtracat '%s' and '%s'");
}
TyphFunc_1A(flt::mul){
	if(isflt) return env->make_float(GETF(a) * GETF(b));
	if(isint) return env->make_float(GETF(a) * GETI(b));
	return mkerr("Cannot multiply '%s' and '%s'");
}
TyphFunc_1A(flt::div){
	if(isflt) return env->make_float(GETF(a) / GETF(b));
	if(isint) return env->make_float(GETF(a) / GETI(b));
	return mkerr("Cannot divide '%s' with '%s'");
}
TyphFunc_1A(flt::mod) {
	//if(isflt) return env->make_float(GETF(a) % GETF(b));
	return mkerr("Cannot mod '%s' with '%s'");
}
TyphFunc_0A(flt::inv) {
	return env->make_float(-GETF(a));
}


TyphFunc_1A(flt::an_) {
	//if(isflt) return env->make_float(GETF(a) & GETF(b));
	return mkerr("Cannot and '%s' and '%s'");
}
TyphFunc_1A(flt::xr_) {
	//if(isflt) return env->make_float(GETF(a) ^ GETF(b));
	return mkerr("Cannot xor '%s' and '%s'");
}
TyphFunc_1A(flt::or_) {
	//if(isflt) return env->make_float(GETF(a) | GETF(b));
	return mkerr("Cannot or '%s' and '%s'");
}
TyphFunc_0A(flt::nt_) {
	//return env->make_float(~GETF(a));
	return env->make_err("Cannot not a 'float' type");
}
TyphFunc_1A(flt::lsh) {
	//if(isflt) return env->make_float(GETF(a) << GETF(b));
	return mkerr("Cannot left shift '%s' and '%s'");
}
TyphFunc_1A(flt::rsh){
	//if(isflt) return env->make_float(GETF(a) >> GETF(b));
	return mkerr("Cannot right shift '%s' and '%s'");
}

TyphFunc_1A(flt::get) {
	return mkerr("Cannot get index from 'float' type");
}

TyphFunc_1A(TyphlosionFloat::cmp) {
	if(b->is(this)) return env->make_float(GETF(a) - GETF(b));
	else if(isint) return env->make_float(GETF(a) - GETF(b));
	return a == b ? env->make_int(0) : env->make_int(1);
}

TyphFunc_0A(flt::inc) {
	float& v = GETF(a);
	v++;
	return a;
}
TyphFunc_0A(flt::dec) {
	float& v = GETF(a);
	v--;
	return a;
}

TyphFunc_CA(TyphlosionFloat::mkn, typh_instance_array args) {
	if(args.cross(TyphlosionEnv::float_type)) return env->make_float(*reinterpret_cast<float*>(args[0]->data()));
	if(args.cross()) return env->make_float(0);
	return env->make_err("No such float constructor");
}	

TyphFunc_CA(TyphlosionFloat::cll, typh_instance_array, typh_generic_array) {
       	return env->make_err("'float' is not callable"); 
}

void TyphlosionFloat::log(std::ostream& stream, typh_instance i) const { 
	float fv = GETF(i);
	stream << fv;
	if(fv - (int)fv == 0) stream << ".0";
}
