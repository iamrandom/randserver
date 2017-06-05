
#include <stdio.h>
# include <stdlib.h>

#include "lualib/extra_lualib.h"



int main(int argc, char* argv[])
{
	
	struct lua_service* s;
	if(argc < 3) return 0;
	s = lua_service_c_create(argv[1]);
	lua_service_c_run(s, argv[2], argc - 3, argv + 3);
	printf("end");
	return 0;
}