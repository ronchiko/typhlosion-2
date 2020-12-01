#pragma once

#include <stdarg.h>
#include <iostream>
#include <functional>
#include <unordered_map>

class TyphlosionEnv;
typedef TyphlosionEnv* typh_env;

#define TyphFunc_1A(name) typh_instance name(typh_env env, typh_instance a, typh_instance b) const
#define TyphFunc_0A(name) typh_instance name(typh_env env, typh_instance a) const
#define TyphComp_1A(name) bool name(typh_env env, typh_instance a, typh_instance b) const

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

	/* Logical operation*/
	NoDef(TyphComp_1A(eql));
	
	/* Misc functions */
	NoDef(void log(std::ostream&, typh_instance) const);
};

typedef TyphlosionType* typh_type;
typedef TyphFunc_1A((TyphlosionType::*typh_func_1a));
typedef TyphFunc_0A((TyphlosionType::*typh_func_0a));
typedef TyphComp_1A((TyphlosionType::*typh_comp_1a));

/* An instance has a type & a value */
class TyphlosionInstance {
private:
	typh_type _type;
	void* address;
	std::function<void()> destructor;
public:
	TyphlosionInstance(typh_type type, void* data, std::function<void()> destructor) : _type(type),
		address(data), destructor(destructor) {}
	~TyphlosionInstance() {
		destructor();
	}

	void* data() const { return address; }
	typh_type type() const { return _type; }
};

template<typename T>
typh_instance make(typh_type t, T* d){
	return new TyphlosionInstance(t, (void*)d, [d]() { delete d; });
}	

class TyphlosionFloat : public TyphlosionType {
public:
	TyphlosionFloat();
	
	TyphFunc_CA(mkn, typh_instance_array) override;
	typh_instance mkn(float v) { return make<float>(this, new float(v)); }

	TyphFunc_1A(add) override;
	TyphFunc_1A(sub) override;
	
	TyphComp_1A(eql) override;

	void log(std::ostream&, typh_instance) const override;

	static float cfloat(typh_instance i) { return *reinterpret_cast<float*>(i->data()); }
};

struct TyphlosionInstanceArray
{
private:
	typh_instance* array;
	int _size;

	/* Variadic constructor helper functions */
	int countVargs() { return 0; }
	template<typename T, typename... Types>
	int countVargs(T a, Types... b){
		return 1 + countVargs(b...);
	}

	void put(typh_instance* f, int i) {}
	template<typename T, typename... Types>
	void put(typh_instance* f, int i, T a, Types... b){
		f[i] = static_cast<typh_instance>(a);
		put(f, i+1, b...);
	}


public:
	template<typename... Types>
	TyphlosionInstanceArray(int size, Types... b) : array(new typh_instance[size]), _size(size) {
		put(array, 0, b...);
	}

	template<typename... Types>
	TyphlosionInstanceArray(Types... b) : array(nullptr), _size(0) {
		_size = countVargs(b...);
		array = new typh_instance[_size];
		put(array, 0, b...);
	}
	~TyphlosionInstanceArray() { delete[] array; }

	typh_instance operator[](int i) { return array[i]; }
	int size() const { return _size; }
};

typedef TyphlosionFloat* typh_float;
