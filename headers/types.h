#pragma once

#include <stdarg.h>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <cstring>

class TyphlosionEnv;
typedef TyphlosionEnv* typh_env;

#define TyphFunc_1A(name) typh_instance name(typh_env env, typh_instance a, typh_instance b) const
#define TyphFunc_0A(name) typh_instance name(typh_env env, typh_instance a) const
#define TyphLogFunc void log(std::ostream stream, typh_instance i) const

#define TyphFunc_CA(name, ...) typh_instance name(typh_env env, typh_instance a, __VA_ARGS__) const  
#define LogFunc void log(std::ostream& stream, typh_instance a, typh_env env) const
#define NoDef(v) virtual v = 0;

#define OvrdFunc1(v) TyphFunc_1A(v) override { return env->make_err("Cannot %s '%t' and '%t'", #v, a->type(), b->type()); }
#define OvrdFunc0(v) TyphFunc_0A(v) override { return env->make_err("Cannot %s '%t'", #v, this); }

class TyphlosionInstance;
typedef TyphlosionInstance* typh_instance;

/* Arrays */
struct TyphlosionInstanceArray;
typedef TyphlosionInstanceArray* typh_instance_array;
class TyphlosionTypeArray;
typedef TyphlosionTypeArray* typh_generic_array;

/* Type related */
typedef typh_instance (*cast_op)(typh_env, typh_instance);
class TyphlosionType;
typedef TyphlosionType* typh_type;

typedef std::function<typh_instance(typh_env, typh_instance)> typh_getter;

enum MemberFlags {
	MF_Static = 1 << 1,
	MF_Private = 1 << 0
};

class TyphlosionType {
protected:
	struct _MemberInfo {
		typh_instance instance;
		int flags;

		_MemberInfo(typh_instance ins, int flags) : instance(ins), flags(flags) {}
		~_MemberInfo();
		
		bool isStatic() const { return flags & MF_Static; }
		bool isPrivate() const { return flags & MF_Private; }
	};
	
	/* adds a member to the type */
	void addm(std::string name, typh_getter getter);
private:
	unsigned int typeId;

	static unsigned int typeIdCounter;
	static std::unordered_map<std::string, TyphlosionType*> types;
public:
	std::unordered_map<std::string, cast_op> casts;
	std::unordered_map<std::string, _MemberInfo*> members;

	TyphlosionType() : typeId(typeIdCounter++), casts(), members() {}
	~TyphlosionType() {
		for(auto& i : members) {
			delete &(i.second);
		}
	}
	/* Make new (constructor) */	
	virtual TyphFunc_CA(mkn, typh_instance_array) = 0;

	/* Arithmatic operations */
	NoDef(TyphFunc_1A(add));
	NoDef(TyphFunc_1A(sub));
	NoDef(TyphFunc_1A(mul));
	NoDef(TyphFunc_1A(div));
	NoDef(TyphFunc_1A(mod));
	NoDef(TyphFunc_0A(inv));
	
	/* Bitwise operators */
	NoDef(TyphFunc_1A(an_));
	NoDef(TyphFunc_1A(xr_));
	NoDef(TyphFunc_0A(nt_));
	NoDef(TyphFunc_1A(or_));
	NoDef(TyphFunc_1A(lsh));
	NoDef(TyphFunc_1A(rsh));
	
	/* Other operators */
	NoDef(TyphFunc_0A(inc));
	NoDef(TyphFunc_0A(dec));
	NoDef(TyphFunc_1A(get));
	NoDef(TyphFunc_CA(cll, typh_instance_array, typh_generic_array = nullptr));
	
	/* Logical operation*/
	NoDef(TyphFunc_1A(cmp));
	
	/* Misc functions */
	NoDef(LogFunc);
	
	/* Cast operation */
	typh_instance cast(typh_env, typh_instance, std::string&);
	typh_instance access(typh_env, typh_instance, std::string&);
};
typedef TyphFunc_1A((TyphlosionType::*typh_func_1a));
typedef TyphFunc_0A((TyphlosionType::*typh_func_0a));

enum InstanceFlags : unsigned short int {
	INF_Const = 1 << 0,
	INF_ForceCopy = 1 << 1,
	INF_Malloc = 1 << 2
};

/* An instance has a type & a value */
class TyphlosionInstance {
private:
	typh_type _type;
	void* address;
	size_t size;
	std::function<void()> destructor;
	unsigned short int flags;
	typh_instance _parent;
public:
	TyphlosionInstance(typh_type type, void* data, size_t size, std::function<void()> destructor, 
			unsigned short int flags=0, typh_instance parent = nullptr) : _type(type), address(data), size(size),
       			destructor(destructor), flags(flags), _parent(parent) {}

	~TyphlosionInstance() {
		if(isNull()) return;

		if(flags & INF_Malloc) free(address); 
		else destructor();
	}

	inline void* data() const { return address; }
	inline typh_type type() const { return _type; }
	
	inline bool isConst() const { return flags & INF_Const; }
	inline bool mustCopy() const { return flags & INF_ForceCopy; }
	inline bool isNull() const { return address == nullptr; }

	typh_instance copy(unsigned short int flags=0) {
		void* cpy = std::malloc(size);
		std::memcpy(cpy, address, size);
		std::cout << this << ", Copy parent: " << _parent << std::endl;
		return new TyphlosionInstance(_type, cpy, size, nullptr, flags | INF_Malloc, _parent);
	}

	inline void setp(typh_instance p) { _parent = p; }
	inline typh_instance parent() const { return _parent; } 
	inline bool is(const TyphlosionType* t) const { return _type == t; }
	inline void setFlags(unsigned short int flags) { this->flags |= flags; }
	friend std::ostream& operator<<(std::ostream&, TyphlosionInstance*);
};

inline std::ostream& operator<<(std::ostream& stream, typh_instance i){
	if(i != nullptr)
		i->_type->log(stream, i, nullptr);
	return stream;
}

template<typename T>
typh_instance make(typh_type t, T* d){
	return new TyphlosionInstance(t, (void*)d, sizeof *d, [d]() { delete d; });
}	

class TyphlosionFloat : public TyphlosionType {
public:
	TyphlosionFloat();
	
	TyphFunc_CA(mkn, typh_instance_array) override;
	typh_instance mkn(float v) { return make<float>(this, new float(v)); }

	TyphFunc_1A(add) override;
	TyphFunc_1A(sub) override;
	TyphFunc_1A(mul) override;
	TyphFunc_1A(div) override;
	TyphFunc_1A(mod) override;
	TyphFunc_0A(inv) override;

	TyphFunc_1A(an_) override;
	TyphFunc_1A(xr_) override;
	TyphFunc_1A(or_) override;
	TyphFunc_0A(nt_) override;
	TyphFunc_1A(lsh) override;
	TyphFunc_1A(rsh) override;
	
	TyphFunc_0A(inc) override;
	TyphFunc_0A(dec) override;
	TyphFunc_1A(get) override;

	TyphFunc_1A(cmp) override;
	
	TyphFunc_CA(cll, typh_instance_array, typh_generic_array) override;

	LogFunc override;

	static float cfloat(typh_instance i) { return *reinterpret_cast<float*>(i->data()); }
};

class TyphlosionBool : public TyphlosionType {
public:
	typh_instance typh_True;
	typh_instance typh_False;

	TyphlosionBool();

	TyphFunc_CA(mkn, typh_instance_array) override;

	TyphFunc_1A(add) override;
	TyphFunc_1A(sub) override;
	TyphFunc_1A(mul) override;
	TyphFunc_1A(div) override;
	TyphFunc_1A(mod) override;
	TyphFunc_0A(inv) override;
	
	TyphFunc_0A(nt_) override;
	TyphFunc_1A(xr_) override;
	TyphFunc_1A(an_) override;
	TyphFunc_1A(or_) override;
	TyphFunc_1A(rsh) override;
	TyphFunc_1A(lsh) override;

	TyphFunc_0A(inc) override;
	TyphFunc_0A(dec) override;
	TyphFunc_1A(get) override;
	
	TyphFunc_CA(cll, typh_instance_array, typh_generic_array) override;

	TyphFunc_1A(cmp) override;

	LogFunc override;

	static bool cbool(typh_instance i) { return *reinterpret_cast<bool*>(i->data()); }
};

class TyphlosionInt : public TyphlosionType {
public:
	TyphlosionInt();

	TyphFunc_CA(mkn, typh_instance_array) override;
	typh_instance mkn(int x) { return make<int>(this, new int(x)); }

	TyphFunc_1A(add) override;
	TyphFunc_1A(sub) override;
	TyphFunc_1A(mul) override;
	TyphFunc_1A(div) override;
	TyphFunc_1A(mod) override;
	TyphFunc_0A(inv) override;

	TyphFunc_0A(nt_) override;
	TyphFunc_1A(an_) override;
	TyphFunc_1A(xr_) override;
	TyphFunc_1A(or_) override;
	TyphFunc_1A(rsh) override;
	TyphFunc_1A(lsh) override;

	TyphFunc_0A(inc) override;
	TyphFunc_0A(dec) override;
	TyphFunc_1A(get) override;

	TyphFunc_1A(cmp) override;
	TyphFunc_CA(cll, typh_instance_array, typh_generic_array) override;

	LogFunc override;
};

class TyphlosionTypeArray {
private:
	int _size;
	typh_type* array;

	inline bool matches(int i) const { return i == _size - 1; }
	template<typename... Ts> inline bool matches(int i, const typh_type type, Ts... b){
		if(i >= _size) return false;
		if(array[i] != type) return false;
		return matches(i + 1, b...);
	}

	inline void vput(int i) {}
	template<typename... Ts> void vput(int i, typh_type a, Ts... b){
		if(i >= _size) return;
		array[i] = a;
		vput(i + 1, b...);
	}

	inline int vcount() { return 0; }
	template<typename... Ts> inline int vcount(typh_type a, Ts... b) { return 1 + vcount(b...); }

public:
	TyphlosionTypeArray(int size) : _size(size), array(new typh_type[size]) {}

	template<typename... T> bool cross(T... a) const { return matches(0, a...); }

	typh_type operator[](int i){
		if(i < 0 || i >= _size) return nullptr;
		return array[i];
	}

	inline int size() const { return _size; }
	inline int put(int i, typh_type t) { array[i] = t; }
};

struct TyphlosionInstanceArray
{
private:
	typh_instance* array;
	int _size;

	/* Variadic constructor helper functions */
	int vcount() { return 0; }
	template<typename T, typename... Types>
	int vcount(T a, Types... b){
		return 1 + vcount(b...);
	}

	void vput(int i) {}
	template<typename T, typename... Types>
	void vput(int i, T a, Types... b){
		if(i >= _size) return;
		array[i] = static_cast<typh_instance>(a);
		vput(i+1, b...);
	}

	inline bool matches(int i) const { return i == _size - 1; }
	template<typename... Ts> inline bool matches(int i, const typh_type type, Ts... b) const { 
		if(i >= _size) return false;
		if(!array[i]->is(type)) return false;
		return matches(i + 1, b...);
	}
public:
	TyphlosionInstanceArray(int size) : array(new typh_instance[size]), _size(size) {}
	TyphlosionInstanceArray(unsigned int size) : array(new typh_instance[size]), _size(size) {}
	template<typename... Types>
	TyphlosionInstanceArray(Types... b) : array(nullptr), _size(0) {
		_size = vcount(b...);
		array = new typh_instance[_size];
		vput(0, b...);
	}
	~TyphlosionInstanceArray() { delete[] array; }

	typh_instance operator[](int i) { return array[i]; }
	int size() const { return _size; }
	
	template<typename... Ts> inline bool cross(Ts... b) const { return matches(0, b...); }

	inline void put(int index, typh_instance i) { array[index] = i; }

	typh_generic_array typeArray() const {
		typh_generic_array garray = new TyphlosionTypeArray(size());
		for(int i = 0; i < _size; i++){
			garray->put(i, array[i]->type());
		}
		return garray;
	}
};

typedef TyphlosionFloat* typh_float;
typedef TyphlosionBool* typh_bool;
typedef TyphlosionInt* typh_int;

#include "members.h"
