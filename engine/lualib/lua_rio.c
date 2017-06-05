


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../net/net_atomic.h"
#include "lua.h"
#include "lauxlib.h"
#include "lmem.h"

#include "extra_lualib.h"

net_atomic_flag rio_flag = {0};

static int	lua_rio_lock(lua_State *L)
{
	net_lock(&rio_flag);
	return 0;
}

static int	lua_rio_unlock(lua_State *L)
{
	net_unlock(&rio_flag);
	return 0;
}


#if defined(WIN32) || defined(WIN64)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static int	lua_rio_blue(lua_State *L)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE
		|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
	return 0;
}

static int	lua_rio_red(lua_State *L)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
		FOREGROUND_RED|FOREGROUND_INTENSITY);
	return 0;
}


static int	lua_rio_green(lua_State *L)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
		FOREGROUND_GREEN|FOREGROUND_INTENSITY);
	return 0;
}


static int	lua_rio_yello(lua_State *L)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
		FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
	return 0;
}



static int	lua_rio_white(lua_State *L)
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdout,
		FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
	return 0;
}


int luaopen_rio(lua_State*L)
{
	struct luaL_Reg rio_methods[] = {
			{"blue", lua_rio_blue},
			{"red", lua_rio_red},
			{"green", lua_rio_green},
			{"yello", lua_rio_yello},
			{"white", lua_rio_white},
			{"lock", lua_rio_lock},
			{"unlock", lua_rio_unlock},
			{0, 0}
	};

	lua_newtable(L);
	luaL_setfuncs(L, rio_methods, 0);
	return 1;
}


#else

int luaopen_rio(lua_State*L)
{
	lua_newtable(L);
	struct luaL_Reg rio_methods[] = {
			{"lock", lua_rio_lock},
			{"unlock", lua_rio_unlock},
			{0, 0}
	};
	luaL_setfuncs(L, rio_methods, 0);
	return 1;
}

#endif

#ifdef __cplusplus
}
#endif

