
#include "macros/types"

#define TYPE_CONST TyphlosionBool

TYPE_CONST::TYPE_CONST() : typh_True(nullptr), typh_False(nullptr) {
	typh_True = make<bool>(this, new bool(true));
	typh_False = make<bool>(this, new bool(false));
	TyphlosionEnv::bool_type = this;
}

NoOp1A(add)
NoOp1A(sub)
NoOp1A(mul)
NoOp1A(div)
NoOp1A(mod)
NoOp1A(lsh)
NoOp1A(rsh)
NoOp1A(get)

NoOp0A(inv)
NoOp0A(inc)
NoOp0A(dec)

TyphFunc_1A(TYPE_CONST::an_) {
	if(b->is(this)) return GETB(a) && GETB(b) ? typh_True : typh_False;
	OpError1A(and);
}
TyphFunc_1A(TYPE_CONST::xr_) {
	if(b->is(this)) return GETB(a) ^ GETB(b) ? typh_True : typh_False;
	OpError1A(xor);
}
TyphFunc_1A(TYPE_CONST::or_) {
	if(b->is(this)) return GETB(a) || GETB(b) ? typh_True : typh_False;
	OpError1A(or);
}
TyphFunc_0A(TYPE_CONST::nt_) {
	return !GETB(a) ? typh_True : typh_False;
}
TyphFunc_1A(TYPE_CONST::cmp) {
	if(b->is(this)) return GETB(a) == GETB(b) ? env->make_int(0) : env->make_int(1);
	OpError1A(eql);
}

TyphFunc_CA(TYPE_CONST::mkn, typh_instance_array uargs) {
	TyphlosionInstanceArray& args = *uargs;
	if(args.cross(TyphlosionEnv::bool_type)) return GETB(args[0]) ?  typh_True : typh_False;
	return env->make_err("No such 'bool' error");
}

TyphFunc_CA(TYPE_CONST::cll, typh_instance_array, typh_generic_array) {
	return env->make_err("'bool' is not callable");
}

LDef {
	stream <<  (GETB(a) ? "True" : "False");
}
