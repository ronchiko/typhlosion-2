
#include <iostream>

#include "allocator.h"
#include "shell.h"

#include "types.h"
#include "error.h"
#include "environment.h"
#include "parser.h"

int main(){	
	typh_env env = new TyphlosionEnv();
	
	new TyphlosionFloat(); new TyphlosionError();
	new TyphlosionBool();
	TyphlosionShell shell(env);
	
	shell.start();

	delete env;
	
	return 0;
}
