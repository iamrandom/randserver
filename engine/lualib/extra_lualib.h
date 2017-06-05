/*
 * extra_lualib.h
 *
 *  Created on: 2014年11月19日
 *      Author: Random
 */

#ifndef EXTRA_LUALIB_H_
#define EXTRA_LUALIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

// int luaopen_serialize(lua_State *L);
int luaopen_luasql_mysql (lua_State *L);
int luaopen_tdriver(lua_State*L);
int luaopen_heap(lua_State*L);
int luaopen_net(lua_State*L);
int luaopen_server(lua_State*L);
int luaopen_tick(lua_State*L);
int luaopen_filesystem(lua_State*L);
int luaopen_rio(lua_State*L);

struct lua_service;
struct lua_service* lua_service_c_create(const char* path);
int lua_service_c_run(struct lua_service* service, const char* lua_file, int argc, char** argv);

lua_State * init_lua_script();

#ifdef __cplusplus
}
#endif

#endif /* EXTRA_LUALIB_H_ */
