


#include <stdio.h>
# include <stdlib.h>

#include "../lualib/extra_lualib.h"
#include "../lualib/serialize.h"
#include "../lualib/serialize.c"
#include "../lualib/luasql.h"
#include "../lualib/lua_serialize.c"

lua_State * init_script()
{
	lua_State *L;
	L = luaL_newstate();
	luaL_openlibs(L);
	
	// luaL_requiref(L, LUASQL_TABLENAME, luaopen_luasql_mysql, 1);
	// lua_pop(L, 1);  /* remove lib */
	luaL_requiref(L, "serialize", luaopen_serialize, 1);
	lua_pop(L, 1);  /* remove lib */
	// luaL_requiref(L, "heap", luaopen_heap, 1);
	// lua_pop(L, 1);  /* remove lib */
	// luaL_requiref(L, "tdriver", luaopen_tdriver, 1);
	// lua_pop(L, 1);  /* remove lib */
	return L;
}

int main(int argc, char* argv[])
{
	
	lua_State * L = init_script();
	// lua_pushstring(L, argv[1]);
	// lua_setglobal(L, "Env");
	// lua_pushinteger(L, atoi(argv[2]));
	// lua_setglobal(L, "ServerID");
	serialize_init123();
	if(luaL_dofile(L, "test_serialize.lua"))
	{
		printf(lua_tostring(L, -1));
		printf("error: %s \n", lua_tostring(L, -1));
	}

	lua_close(L);

	printf("end");
	return 0;
}

