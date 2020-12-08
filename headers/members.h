
#include "types.h"
#include <functional>

class TyphlosionMember : public TyphlosionInstance {
public:
	typh_getter getter;

	TyphlosionMember(typh_getter gtr) : TyphlosionInstance(nullptr, nullptr, 0, nullptr), getter(gtr) {}

	typh_instance getm(typh_env e, typh_instance a) {
		return getter(e, a);
	}
};

typedef TyphlosionMember* typh_member;
