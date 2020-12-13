
#include "types.h"
#include "environment.h"

unsigned int TyphlosionType::typeIdCounter = 0;

typh_instance TyphlosionType::cast(typh_env env, typh_instance a, std::string& type_name){
	if(casts.find(type_name) == casts.end()) 
		return env->make_err("Cannot cast from '%t' to '%s'.", a->type(), type_name.c_str());
	return (*casts[type_name])(env, a);
}

typh_instance TyphlosionType::access(typh_env env, typh_instance a, std::string& member){
	if(members.find(member) == members.end())
		return env->make_err("No member named '%s' in '%t'", member.c_str(), a->type());
	_MemberInfo* info = members[member];
	if(info->isStatic()) return info->instance;
	
	typh_member mbr = reinterpret_cast<typh_member>(info->instance);
	
	return mbr->getm(env, a);
}

void TyphlosionType::addm(std::string name, typh_getter getter) {
	if(members.find(name) != members.end()) {
		return;	
	}
	members[name] = new _MemberInfo(new TyphlosionMember(getter), 0);
}

TyphlosionType::_MemberInfo::~_MemberInfo() { delete instance; }
