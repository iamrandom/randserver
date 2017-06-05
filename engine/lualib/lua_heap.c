/*
 * luaheap.c
 *
 *  Created on: 2015-10-16
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

#define LUA_HEAP	"Lua Heap"
#define LUA_HEAP_CURSOR	"Heap cursor"


static int	lua_heap_open(lua_State *L)
{
	lua_Integer is_head = luaL_checkinteger(L, 1);
	struct heap* hp = (struct heap*) lua_newuserdata(L, sizeof(struct heap));
	if(!hp)
	{
		lua_pushnil(L);
		return 1;
	}
	heap_init(hp, (char)is_head);
	luaL_getmetatable(L, LUA_HEAP_CURSOR);
	lua_setmetatable(L, -2);
	return 1;
}

static int	lua_heap_close(lua_State *L)
{
	struct heap* hp = (struct heap*) luaL_checkudata(L, 1, LUA_HEAP_CURSOR);
	luaL_argcheck (L, hp != 0, 1, LUA_HEAP_CURSOR" close expected");
	heap_release(hp);
	return 0;
}


static int	lua_heap_push(lua_State *L)
{
	lua_Integer key;
	struct heap* hp = (struct heap*) luaL_checkudata(L, 1, LUA_HEAP_CURSOR);
	luaL_argcheck (L, hp != 0, 1, LUA_HEAP_CURSOR" push expected");
	key = luaL_checkinteger(L, 2);
	heap_push(hp, key);
	return 0;
}

static int	lua_heap_pop(lua_State *L)
{
	lua_Integer index;
	int64_t key;
	struct heap* hp = (struct heap*) luaL_checkudata(L, 1, LUA_HEAP_CURSOR);
	luaL_argcheck (L, hp != 0, 1, LUA_HEAP_CURSOR" pop expected");
	index = luaL_checkinteger(L, 2);
	if(!heap_pop(hp, &key, (int)index))
	{
		return 0;
	}
	lua_pushinteger(L, (lua_Integer)key);
	return 1;
}

static int	lua_heap_get(lua_State *L)
{
	lua_Integer index;
	int64_t key;
	struct heap* hp = (struct heap*) luaL_checkudata(L, 1, LUA_HEAP_CURSOR);
	luaL_argcheck (L, hp != 0, 1, LUA_HEAP_CURSOR" head expected");
	index = luaL_checkinteger(L, 2);
	if(!heap_get(hp, &key, (int)index))
	{
		return 0;
	}
	lua_pushinteger(L, (lua_Integer)key);
	return 1;
}

static int	lua_heap_size(lua_State *L)
{
	struct heap* hp = (struct heap*) luaL_checkudata(L, 1, LUA_HEAP_CURSOR);
	luaL_argcheck (L, hp != 0, 1, LUA_HEAP_CURSOR" size expected");
	lua_pushinteger(L, (lua_Integer)hp->size);
	return 1;
}


int luaopen_heap(lua_State*L)
{
	struct luaL_Reg heap_methods[] = {
			{"close", lua_heap_close},
			{"push", lua_heap_push},
			{"pop", lua_heap_pop},
			{"get", lua_heap_get},
			{"size", lua_heap_size},
			{0, 0}
	};

	struct luaL_Reg h_methods[] = {
			{"open", lua_heap_open},
			{0, 0}
	};

	if(!luaL_newmetatable(L, LUA_HEAP_CURSOR))
	{
		return 0;
	}
	luaL_setfuncs (L, heap_methods, 0);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);
	lua_pop(L, 1);

	lua_newtable(L);
	luaL_setfuncs(L, h_methods, 0);
	return 1;
}




