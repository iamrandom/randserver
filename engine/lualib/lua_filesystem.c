/*
 * luaheap.c
 *
 *  Created on: 2016-5-5
 *      Author: Random
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "lua.h"
#include "lauxlib.h"
#include "lmem.h"

#include "../tool/heap.h"
#ifdef __cplusplus
}
#endif


#include "extra_lualib.h"

#define LUA_FILE_SYSTEM	"Lua File System"
#define LUA_FILE_SYSTEM_CURSOR	"File System cursor"

#include <io.h>

static int	lua_list_file(lua_State *L)
{
	const char* path;
	long handle;
	struct _finddata_t file_info;
	int all_dir;
	int cnt;

	all_dir = 0;
	cnt = 0;
	path = luaL_checkstring(L, 1);
	if (lua_gettop(L) > 1)
	{
		all_dir = lua_toboolean(L, 2);
	}
	if((handle=_findfirst(path,&file_info))!=-1L)
	{
		if(!all_dir || (file_info.attrib == _A_SUBDIR))
		{
			lua_pushstring(L, file_info.name);
			++cnt;
		}
		while(_findnext(handle,&file_info)==0)
		{
			if(!all_dir || (file_info.attrib == _A_SUBDIR))
			{
				lua_pushstring(L, file_info.name);
				++cnt;
			}
		}
		_findclose(handle);
	}
	return cnt;
}


int luaopen_filesystem(lua_State*L)
{

	struct luaL_Reg h_methods[] = {
			{"list_file", lua_list_file},
			{0, 0}
	};

	lua_newtable(L);
	luaL_setfuncs(L, h_methods, 0);
	return 1;
}
