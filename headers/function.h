#pragma once

#include "environment.h"
#include "types.h"
#include "error.h"
#include "macros/types"

#include <vector>

struct FunctionSignature;

class TyphlosionFunction : public TyphlosionType {
public:
	struct FunctionHandle {
		typh_func_c general_caller;

		std::vector<FunctionSignature*> override_callers;

		typh_instance try_call(typh_env, typh_instance, typh_instance_array, typh_generic_array);
		void add_override(typh_func_c, typh_generic_array, int);

		~FunctionHandle();
		FunctionHandle() : general_caller(nullptr), override_callers() {}
		FunctionHandle(typh_func_c fnc, typh_generic_array ga, int gen) : general_caller(nullptr), override_callers() {
			add_override(fnc, ga, gen);
		}
		FunctionHandle(typh_func_c generalCaller) : general_caller(generalCaller), override_callers() {}
	};
	
	TyphlosionFunction ();
	
	OvrdFunc1(sub) OvrdFunc1(add)
	OvrdFunc1(div) OvrdFunc1(mul) OvrdFunc1(mod);
	OvrdFunc0(inv)
	OvrdFunc1(an_)
	OvrdFunc1(or_)
	OvrdFunc1(xr_)
	OvrdFunc1(rsh)
	OvrdFunc1(lsh)
	OvrdFunc0(nt_)
	OvrdFunc1(get)
	OvrdFunc0(inc) OvrdFunc0(dec)

	LogFunc override;
	TyphFunc_CA(mkn, typh_instance_array) override;
	TyphFunc_CA(cll, typh_instance_array, typh_generic_array) override;
	TyphFunc_1A(cmp) override;

	typh_instance mkn(typh_func_c fnc, typh_generic_array inputs, int generics) {
		return make<FunctionHandle>(this, new FunctionHandle(fnc, inputs, generics));
	}
};

typh_instance makeGeneralHandle(typh_func_c);
