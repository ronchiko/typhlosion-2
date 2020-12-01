
#include "types.h"
#include "environment.h"
#include "error.h"

#define FLOAT(v) static_cast<typh_float>(v)
#define GETF(v) *reinterpret_cast<float*>(v->data())

unsigned int TyphlosionType::typeIdCounter = 0;

TyphlosionFloat::TyphlosionFloat() {
	TyphlosionEnv::float_type = this;
}

TyphFunc_1A(TyphlosionFloat::add) {
	if(env->findt("float") == b->type()) return env->make_float(GETF(a) + GETF(b));
	return env->make_err("No adding operation found.");
}

TyphFunc_1A(TyphlosionFloat::sub) {
	if(env->findt("float") == b->type()) return env->make_float(GETF(a) - GETF(b));
	return env->make_err("No subtraction operation found.");
}

TyphComp_1A(TyphlosionFloat::eql) {
	if(env->findt("float") == b->type()) return GETF(a) == GETF(b);
	return a == b;
}

TyphFunc_CA(TyphlosionFloat::mkn, typh_instance_array args) {
	if(args.size() == 1) return env->make_float(*reinterpret_cast<float*>(args[0]->data()));
	if(args.size() == 0) return env->make_float(0);
	return env->make_err("No such float constructor");
}	

void TyphlosionFloat::log(std::ostream& stream, typh_instance i) const { stream << *reinterpret_cast<float*>(i->data()); }
