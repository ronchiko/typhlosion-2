
#include "error.h"
#include "environment.h"

#define err TyphlosionError

#define thrwerr(msg) { return env->make_err(msg); }

err::err() {
	TyphlosionEnv::error_type = this;
}

TyphFunc_1A(err::add) thrwerr("Cannot add error types")
TyphFunc_1A(err::sub) thrwerr("Cannot sub error types")
TyphComp_1A(err::eql) thrwerr("Unimplemented")

void err::log(std::ostream& stream, typh_instance inst) const {
	stream << "\033[1;31m";

	error_data* data = reinterpret_cast<error_data*>(inst->data());
	stream << "Error Thrown! " << data->msg;

	stream << "\033[0m";
}

TyphFunc_CA(err::mkn, typh_instance_array args) {
	if(args.size() != 0) return env->make_err("No such constructor");
	return env->make_err("an error was thrown.");
}
