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
#define TyphComp_1A(name) int name(typh_env env, typh_instance a, typh_instance b) const
#define TyphLogFunc void log(std::ostream stream, typh_instance i) const

#define TyphFunc_CA(name, ...) typh_instance name(typh_env env, typh_instance a, __VA_ARGS__) const  

#define NoDef(v) virtual v = 0;

class TyphlosionInstance;
typedef TyphlosionInstance* typh_instance;
struct TyphlosionInstanceArray;
typedef TyphlosionInstanceArray& typh_instance_array;

class TyphlosionType {
private:
	unsigned int typeId;

	static unsigned int typeIdCounter;
	static std::unordered_map<std::string, TyphlosionType*> types;
public:
	TyphlosionType() : typeId(typeIdCounter++) {}
	
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

	/* Logical operation*/
	NoDef(TyphFunc_1A(eql));
	
	/* Misc functions */
	NoDef(void log(std::ostream&, typh_instance) const);
};

typedef TyphlosionType* typh_type;
typedef TyphFunc_1A((TyphlosionType::*typh_func_1a));
typedef TyphFunc_0A((TyphlosionType::*typh_func_0a));
typedef TyphComp_1A((TyphlosionType::*typh_comp_1a));

enum InstanceFlags : unsigned short int {
	INF_Const = 1 << 0,
	INF_ForceCopy = 1 << 1
};

/* An instance has a type & a value */
class TyphlosionInstance {
private:
	typh_type _type;
	void* address;
	size_t size;
	std::function<void()> destructor;
	unsigned short int flags;
public:
	TyphlosionInstance(typh_type type, void* data, size_t size, std::function<void()> destructor, 
			unsigned short int flags=0) : _type(type), address(data), size(size), destructor(destructor), flags(flags) {}
	~TyphlosionInstance() {
		destructor();
	}

	void* data() const { return address; }
	typh_type type() const { return _type; }
	
	bool isConst() const { return flags & INF_Const; }
	bool mustCopy() const { return flags & INF_ForceCopy; }

	typh_instance copy(unsigned short int flags=0) {
		void* cpy;
		std::memcpy(cpy, address, size);
		return new TyphlosionInstance(_type, cpy, size, nullptr, flags);
	}

	bool is(const TyphlosionType* t) const { return _type == t; }
};

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

	TyphFunc_1A(eql) override;

	void log(std::ostream&, typh_instance) const override;

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

	TyphFunc_1A(eql) override;

	void log(std::ostream&, typh_instance) const override;

	static bool cbool(typh_instance i) { return *reinterpret_cast<bool*>(i->data()); }
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
	template<typename... Ts> inline bool matches(int i, typh_type type, Ts... b) const { 
		if(i >= _size) return false;
		if(!array[i]->is(type)) return false;
		return matches(i + 1, b...);
	}
public:
	template<typename... Types>
	TyphlosionInstanceArray(int size, Types... b) : array(new typh_instance[size]), _size(size) {
		vput(0, b...);
	}

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
};

typedef TyphlosionFloat* typh_float;
typedef TyphlosionBool* typh_bool;
