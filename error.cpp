
#include "error.h"
#include "environment.h"
#include "console.h"
#include "macros/types"

#define err TyphlosionError

#define ErrorOp { if(b->is(this)) return combine(a, b); return a; }

#define TYPE_CONST err

err::err() {
	TyphlosionEnv::error_type = this;
}

TyphFunc_1A(err::add) ErrorOp
TyphFunc_1A(err::sub) ErrorOp
TyphFunc_1A(err::mul) ErrorOp
TyphFunc_1A(err::div) ErrorOp
TyphFunc_0A(err::inv) { return a; }
TyphFunc_1A(err::mod) ErrorOp

TyphFunc_1A(err::an_) ErrorOp
TyphFunc_1A(err::xr_) ErrorOp
TyphFunc_1A(err::or_) ErrorOp
TyphFunc_1A(err::lsh) ErrorOp
TyphFunc_1A(err::rsh) ErrorOp
TyphFunc_0A(err::nt_) { return a; }

TyphFunc_1A(err::get) ErrorOp
TyphFunc_0A(err::inc) { return a; }
TyphFunc_0A(err::dec) { return a; }

TyphFunc_1A(err::cmp) {
	return b == a ? env->make_int(0) : env->make_int(1);
}

TyphFunc_CA(err::cll, typh_instance_array, typh_generic_array) {
	return env->make_err("'error' is not callable");
}

LDef {
	stream << color(CF_Bold, FG_Red);

	error_data* data = reinterpret_cast<error_data*>(a->data());
	stream << "Unhandled error: " << data->msg;

	stream << CONSOLE_RESET;
}

TyphFunc_CA(err::mkn, typh_instance_array args) {
	if(args->size() != 0) return env->make_err("No such constructor");
	return env->make_err("an error was thrown.");
}
