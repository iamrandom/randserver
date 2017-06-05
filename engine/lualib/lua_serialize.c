#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "lua.h"
#include "lauxlib.h"
#include "lmem.h"
#include "serialize.h"
#include "extra_lualib.h"

#define LUA_SERIALIZE_CURSOR "Serialize cursor"
#define LUA_DESERIALIZE_CURSOR "Deserialize cursor"

void serialize_init123()
{
	return;
}


static int	lua_serialize_open(lua_State *L)
{
	serialize_data* ser_data;
	unsigned int support_flag;
	const char* buff;
	lua_Integer init_size;

	support_flag = (unsigned int)luaL_checkinteger(L, 1);
	buff = 0;
	init_size = 0;

	if(lua_gettop(L)  > 2)
	{
		luaL_argcheck(L, lua_islightuserdata(L, 2), 2, LUA_SERIALIZE_CURSOR" #2 is not a lightuserdata");
		
		buff = (const char*)lua_touserdata(L, 2);
		init_size = luaL_checkinteger(L, 3);
	}
	ser_data = (serialize_data*) lua_newuserdata(L, sizeof(serialize_data));
	if(!ser_data)
	{
		return 0;
	}
	serialize_init(ser_data, support_flag, buff, init_size);
	luaL_getmetatable(L, LUA_SERIALIZE_CURSOR);
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_serialize_clean(lua_State *L)
{
	serialize_data* ser_data;

	ser_data = (serialize_data*) luaL_checkudata(L, 1, LUA_SERIALIZE_CURSOR);
	if(!ser_data )
	{
		return 0;
	}
	serialize_clean(ser_data);
	return 0;
}

static int lua_serialize_pack(lua_State *L)
{
	serialize_data* ser_data;

	ser_data = (serialize_data*) luaL_checkudata(L, 1, LUA_SERIALIZE_CURSOR);
	if(!ser_data )
	{
		lua_pushinteger(L, err_no_fit_serialize_data);
		return 1;
	}
	serialize_pack(ser_data, L, 2);
	lua_pushinteger(L, ser_data->error);
	return 1;
}

static int lua_serialize_get_error(lua_State *L)
{
	serialize_data* ser_data;
	ser_data = (serialize_data*) luaL_checkudata(L, 1, LUA_SERIALIZE_CURSOR);
	if(!ser_data )
	{
		lua_pushinteger(L, err_no_fit_serialize_data);
		return 1;
	}
	lua_pushinteger(L, ser_data->error);
	lua_pushinteger(L, ser_data->error_info);
	return 2;
}

static int lua_serialize_get_string(lua_State *L)
{
	serialize_data* ser_data;

	ser_data = (serialize_data*) luaL_checkudata(L, 1, LUA_SERIALIZE_CURSOR);
	
	if(!ser_data )
	{
		return 0;
	}
	lua_pushlstring(L, ser_data->data, ser_data->size);
	return 1;
}

static int lua_serialize_get_data(lua_State *L)
{
	serialize_data* ser_data;

	ser_data = (serialize_data*) luaL_checkudata(L, 1, LUA_SERIALIZE_CURSOR);
	
	if(!ser_data )
	{
		return 0;
	}
	lua_pushlightuserdata(L, ser_data->data);
	lua_pushinteger(L, (lua_Integer)ser_data->size);
	return 2;
}


static int lua_deserialize_open(lua_State* L)
{
	deserialize_data* deser_data;
	deser_data = (deserialize_data*) lua_newuserdata(L, sizeof(deserialize_data));
	if(!deser_data)
	{
		return 0;
	}
	deser_data->data = 0;
	deser_data->size = 0;
	deser_data->error = 0;
	deser_data->error_info = 0;
	luaL_getmetatable(L, LUA_DESERIALIZE_CURSOR);
	lua_setmetatable(L, -2);
	return 1;
}

static int lua_deserialize_unpack(lua_State* L)
{
	deserialize_data* deser_data;
	serialize_data* ser_data;
	int top;
	const char* data;
	size_t size;
	int index;

	deser_data = (deserialize_data*) luaL_checkudata(L, 1, LUA_DESERIALIZE_CURSOR);

	if(!deser_data)
	{
		return 0;
	}
	top = lua_gettop(L);
	switch(top)
	{
	case 2:
	case 3:
		if(top == 2)
		{
			index = -1;
		}
		else
		{
			index = luaL_checkinteger(L, 3);
		}
		if (lua_isstring(L, 2))
		{
			data = luaL_checklstring(L, 2, &size);
			
			return deserialize_unpack(deser_data, L, data, size, index);
		}
		else
		{
			ser_data = (serialize_data*) luaL_checkudata(L, 2, LUA_SERIALIZE_CURSOR);
			return deserialize_unpack(deser_data, L, ser_data->data, ser_data->size, index);
		}
		
	case 4:
		luaL_argcheck(L, lua_islightuserdata(L, 2), 2, LUA_SERIALIZE_CURSOR" uppack function #2 is not a lightuserdata");
		data = (const char*)lua_touserdata(L, 2);
		size = luaL_checkinteger(L, 3);
		index = luaL_checkinteger(L, 4);
		return deserialize_unpack(deser_data, L, data, size, index);
	}
	return 0;
}

static int lua_deserialize_get_error(lua_State *L)
{
	deserialize_data* deser_data;
	deser_data = (deserialize_data*) luaL_checkudata(L, 1, LUA_DESERIALIZE_CURSOR);
	if(!deser_data )
	{
		lua_pushinteger(L, err_no_fit_deserialize_data);
		return 1;
	}
	lua_pushinteger(L, deser_data->error);
	lua_pushinteger(L, deser_data->error_info);
	return 2;
}


static int lua_new_buff(lua_State *L)
{
	size_t size;
	void* buff;
	size = (size_t)luaL_checkinteger(L, 1);
	buff = malloc(size);
	if (!buff) return 0;
	lua_pushlightuserdata(L, buff);
	return 1;
}

static int lua_free_buff(lua_State *L)
{
	void* buff;
	luaL_argcheck(L, lua_islightuserdata(L, 1), 1, "lua_free_buff #1 must lightuserdata");
	buff = lua_touserdata(L, 1);
	free(buff);
	return 0;
}

int luaopen_serialize (lua_State *L) {

	struct luaL_Reg ser_methods[] = {
			{"clean", lua_serialize_clean},
			{"pack", lua_serialize_pack},
			{"get_error", lua_serialize_get_error},
			{"get_data", lua_serialize_get_data},
			{"get_data", lua_serialize_get_string},
			{NULL, NULL}
	};

	struct luaL_Reg deser_methods[] = {
			{"unpack", lua_deserialize_unpack},
			{"get_error", lua_deserialize_get_error},
			{NULL, NULL}
	};

	struct luaL_Reg methods[] = {
			{"new_serialize", lua_serialize_open},
			{"new_deserialize", lua_deserialize_open},
			{"new_buff", lua_new_buff},
			{"free_buff", lua_free_buff},
			{NULL, NULL}
	};

	if(!luaL_newmetatable(L, LUA_SERIALIZE_CURSOR))
	{
		return 0;
	}
	luaL_setfuncs (L, ser_methods, 0);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);
	lua_pop(L, 1);

	if(!luaL_newmetatable(L, LUA_DESERIALIZE_CURSOR))
	{
		return 0;
	}
	luaL_setfuncs (L, deser_methods, 0);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);
	lua_pop(L, 1);

	lua_newtable(L);
	luaL_setfuncs(L, methods, 0);
	return 1;
}



#ifdef __cplusplus
}
#endif