
#include <iostream>

#include "allocator.h"
#include "shell.h"

#include "types.h"
#include "error.h"
#include "environment.h"

int main(){	
	typh_env env = new TyphlosionEnv();
	
	new TyphlosionFloat(); new TyphlosionError();

	typh_instance a = env->make_float(10), b = env->make_float(20);
	typh_instance err = env->make_err("Extreme %s", "super kali");	
	typh_instance d = env->call("sub", a, b);
	
	typh_instance_array arr = *new TyphlosionInstanceArray(4, a, b, d, err);
	
	for(int i = 0; i < arr.size(); i++){
		std::cout << env->namet(arr[i]->type());
		if(i != arr.size() - 1) std::cout << ", ";
	}
	
	std::cout << std::endl;
	env->call("log", d);
	std::cout << std::endl;
	env->call("log", err); 
	std::cout << std::endl;

	delete env;
	delete &arr;
	
	return 0;
}
