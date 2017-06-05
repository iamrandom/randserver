

#ifdef __cplusplus
extern "C" {
#endif
#include "lua.h"
#include "lauxlib.h"
#include "lmem.h"

#include "../tool/sbtree.h"

#include "extra_lualib.h"

#define LUA_TICK	"Lua Tick"
#define LUA_TICK_CURSOR	"Tick cursor"

static struct sbtree_node* tick_root = 0;


static int	lua_tick_close(lua_State *L)
{
	sb_tree_clean(&tick_root);
	return 0;
}

static int	lua_tick_reg(lua_State *L)
{
	int seconds;
	unsigned int index;
	sb_tree_key key;
	sb_tree_key nextkey;
	const struct sbtree_node* node;

	// if(!tick_root) return 0;

	key = 0;
	index = 0;
	seconds = (int)luaL_checkinteger(L, 1);
	key = seconds;
	key *= 1073741824;
	key *= 4;

	nextkey = seconds + 1;
	nextkey *= 1073741824;
	nextkey *= 4;

	index = sb_tree_lt(tick_root, nextkey);
	if(index > 0)
	{
		node = sb_tree_index(tick_root, index - 1);
		if(node)
		{
			if(node->key >= key)
			{
				key = node->key + 1;
			}
		}
	}
	sb_tree_value v;
	v.ptr = 0;
	sb_tree_insert(&tick_root, key, v);
	lua_pushinteger(L, (lua_Integer)key);
	return 1;
}

static int	lua_tick_unreg(lua_State *L)
{

	lua_Integer key;
	key = luaL_checkinteger(L, 1);
	sb_tree_delete(&tick_root, (sb_tree_key)key);
	return 0;
}

static int	lua_tick_size(lua_State *L)
{
	lua_pushinteger(L, (lua_Integer)sb_tree_size(tick_root));
	return 1;
}

static int	lua_tick_index(lua_State *L)
{
	const struct sbtree_node* node;
	lua_Integer index;

	index = luaL_checkinteger(L, 1);

	node = sb_tree_index(tick_root, (unsigned int)index);
	if (!node)
	{
		return 0;
	}
	lua_pushinteger(L, (lua_Integer)node->key);
	return 1;
}

static int	lua_tick_lt(lua_State *L)
{
	sb_tree_key key;
	key = (sb_tree_key)luaL_checkinteger(L, 1);
	key *= 1073741824;
	key *= 4;

	lua_pushinteger(L, (lua_Integer)sb_tree_lt(tick_root, (sb_tree_key)key));
	
	return 1;
}


int luaopen_tick(lua_State*L)
{
	struct luaL_Reg heap_methods[] = {
			{"close", lua_tick_close},
			{"reg", lua_tick_reg},
			{"unreg", lua_tick_unreg},
			{"size", lua_tick_size},
			{"index", lua_tick_index},
			{"lt", lua_tick_lt},
			{0, 0}
	};

	lua_newtable(L);
	luaL_setfuncs(L, heap_methods, 0);
	return 1;
}

#ifdef __cplusplus
}
#endif