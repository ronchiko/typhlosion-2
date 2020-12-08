#pragma once

#include "types.h"
#include <string>

class TyphlosionError : public TyphlosionType {
	struct error_data {
		std::string msg;

		error_data(std::string msg) : msg(msg) {}
	};

public:
	TyphlosionError();
	
	TyphFunc_CA(mkn, typh_instance_array) override;
	typh_instance mkn(std::string str) { return make<error_data>(this, new error_data(str)); }

	virtual void log(std::ostream&, typh_instance) const override;
	
	TyphFunc_1A(add) override;
	TyphFunc_1A(sub) override;
	TyphFunc_1A(mul) override;
	TyphFunc_1A(div) override;
	TyphFunc_1A(mod) override;
	TyphFunc_0A(inv) override;	

	TyphFunc_1A(an_) override;
	TyphFunc_1A(xr_) override;
	TyphFunc_1A(or_) override;
	TyphFunc_1A(lsh) override;
	TyphFunc_1A(rsh) override;
	TyphFunc_0A(nt_) override;

	TyphFunc_0A(inc) override;
	TyphFunc_0A(dec) override;
	TyphFunc_1A(get) override;

	TyphFunc_1A(cmp) override;
	TyphFunc_CA(cll, typh_instance_array, typh_generic_array) override;

	typh_instance combine(typh_instance a, typh_instance b) const {
		error_data *ad = reinterpret_cast<error_data*>(a->data()), *bd = reinterpret_cast<error_data*>(b->data());

		ad->msg = ad->msg + bd->msg;
		return a;
	}
};

typedef TyphlosionError* typh_error;
