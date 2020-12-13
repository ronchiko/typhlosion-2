
#include <iostream>

#include "allocator.h"
#include "shell.h"

#include "types.h"
#include "error.h"
#include "environment.h"
#include "function.h"
#include "strings.h"
#include "parser.h"

int main(){	
	typh_env env = initRootEnvironment();
	
	TyphlosionShell shell(env);	
	
	shell.start();

	delete env;
	
	return 0;
}
