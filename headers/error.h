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
	
	TyphComp_1A(eql) override;
};

typedef TyphlosionError* typh_error;
