#pragma once

#include <string>
#include "types.h"
#include "environment.h"

class TyphlosionString : public TyphlosionType  {
public:
	TyphlosionString();
	
	typh_instance mkn(std::string str) { return make<std::string>(this, new std::string(str)); }
	
	OvrdFunc0(inv) OvrdFunc1(mod) OvrdFunc1(div) OvrdFunc1(sub)
	OvrdFunc0(nt_) OvrdFunc1(an_) OvrdFunc1(or_) OvrdFunc1(xr_)
	OvrdFunc1(lsh) OvrdFunc1(rsh)
	
	OvrdFunc0(inc) OvrdFunc0(dec) OvrdFunc1(get)

	TyphFunc_1A(cmp) override;
	TyphFunc_1A(add) override;
	TyphFunc_1A(mul) override;

	LogFunc override;
	TyphFunc_CA(mkn, typh_instance_array) override;
	TyphFunc_CA(cll, typh_instance_array, typh_generic_array) override { return env->make_err("'string' is not callable"); }

};
