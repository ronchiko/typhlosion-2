#pragma once

#include <unordered_map>
#include <iostream>
#include <functional>

class TyphAllocator {
private:
	struct _DeletionArg {
		std::function<void()> destructor;
		void* identifier;
		bool valid = false;

		_DeletionArg(std::function<void()> arg, void* vp) : destructor(arg), identifier(vp), valid(true) {}

		void destroy() { 
			valid = false;
			destructor();
	       	}
	};
	std::unordered_map<void*, _DeletionArg*> allocated;

public:
	TyphAllocator() : allocated() {}
	~TyphAllocator() { clear(); }

	void clear() {
		for(auto& v : allocated){
			v.second->destroy();
			delete v.second;
		}
		allocated.clear();
	}

	template<class T> T* allocate(T* o){
		allocated[(void*)o] = new _DeletionArg([o]() { delete o; }, o);
		return o;
	}

	void deallocate(void* o){
		if(allocated.find(o) != allocated.end()) {
			allocated[o]->destroy();
			delete allocated[o];
			allocated.erase(o);	
		}
	}
};

#define ALLOCATE(type, ...) allocate<type>(new type(__VA_ARGS__))
