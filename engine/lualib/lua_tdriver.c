/*
 * luatdriver.c
 *
 *  Created on: 2015年10月15日
 *      Author: Random
 */

#ifdef __cplusplus
extern "C" {
#endif
#include "lua.h"
#include "lauxlib.h"
#include "lmem.h"
#include "extra_lualib.h"
#ifdef __cplusplus
}
#endif

#include "../tool/tdriver.h"

#define LUA_TDRIVER	"Lua TDriver"

time_driver td;

static int lua_tdriver_open(lua_State *L)
{
	time_driver_init(&td);
	return 0;
}

static int lua_tdriver_update(lua_State *L)
{
	time_driver_update(&td);
	return 0;
}

static int lua_tdriver_pass_msec(lua_State *L)
{
	time_driver_pass_msec(&td, (unsigned int)luaL_checkinteger(L, 1));
	return 0;
}

static int lua_tdriver_get_new_sec(lua_State *L)
{
	lua_pushinteger(L, (lua_Integer)time_driver_get_new_sec(&td));
	return 1;
}

static int lua_tdriver_get_new_100msec(lua_State *L)
{
	lua_pushinteger(L, (lua_Integer)time_driver_get_new_100msec(&td));
	return 1;
}

static int lua_tdriver_get_pass_seconds(lua_State *L)
{
	lua_pushinteger(L, (lua_Integer)td.UTC_pass_seconds);
	return 1;
}

static int lua_tdriver_get_UTC_seconds(lua_State *L)
{
	lua_pushinteger(L, (lua_Integer)(td.UTC_pass_seconds + td.UTC_base_seconds));
	return 1;
}

static int lua_tdriver_get_UTC_tm(lua_State *L)
{
	lua_pushinteger(L, (lua_Integer)(td.tm_time.tm_year));
	lua_pushinteger(L, (lua_Integer)(td.tm_time.tm_mon));
	lua_pushinteger(L, (lua_Integer)(td.tm_time.tm_mday));
	lua_pushinteger(L, (lua_Integer)(td.tm_time.tm_hour));
	lua_pushinteger(L, (lua_Integer)(td.tm_time.tm_min));
	lua_pushinteger(L, (lua_Integer)(td.tm_time.tm_sec));
	lua_pushinteger(L, (lua_Integer)(td.tm_time.tm_wday));
	lua_pushinteger(L, (lua_Integer)(td.tm_time.tm_yday));
	return 8;
}

int luaopen_tdriver(lua_State*L)
{
	struct luaL_Reg methods[] = {
			{"open", lua_tdriver_open},
			{"update", lua_tdriver_update},
			{"pass_msec", lua_tdriver_pass_msec},
			{"get_new_sec", lua_tdriver_get_new_sec},
			{"get_new_100msec", lua_tdriver_get_new_100msec},
			{"get_pass_seconds", lua_tdriver_get_pass_seconds},
			{"get_UTC_seconds", lua_tdriver_get_UTC_seconds},
			{"get_UTC_tm", lua_tdriver_get_UTC_tm},
			{0, 0}
	};

	lua_newtable(L);
	luaL_setfuncs(L, methods, 0);
	return 1;
}
