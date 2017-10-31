
#include <pthread.h>
#include "serialize.h"
#include "../net/net_atomic.h"
#include "extra_lualib.h"

#define LUA_SERVER_CURSOR	"server cursor"

struct msg_node
{
	void*					data;
	size_t 					size;
	struct msg_node*		next;
};

#define SERVICE_SIZE 4096

struct lua_service
{
	lua_State*				L;
	pthread_t 				pt;
	serialize_data			ser; // don't use it in other thread
	deserialize_data		deser;
	net_atomic_flag			flag;
	int 					state;
	int 					node_size;
	struct msg_node*		head;
	struct msg_node*		tail;
	size_t 					ser_buff_size;
	char					ser_buff[1];
};


static int error_handle(lua_State * L)
{
	lua_getglobal(L, "print");
	lua_pushvalue(L, -2);
	luaL_traceback(L, L, 0, 1);
	lua_call(L, 2, 0);
	return 1;
}

lua_State *  init_lua_script(int thread_safe)
{
	lua_State *L;
	L = luaL_newstate();
	luaL_openlibs(L);

	luaL_requiref(L, "luasql", luaopen_luasql_mysql, 1);
	lua_pop(L, 1);  /* remove lib */
	luaL_requiref(L, "heap", luaopen_heap, 1);
	lua_pop(L, 1);  /* remove lib */
	luaL_requiref(L, "net", luaopen_net, 1);
	lua_pop(L, 1);  /* remove lib */
	luaL_requiref(L, "server", luaopen_server, 1);
	lua_pop(L, 1);  /* remove lib */
	luaL_requiref(L, "filesystem", luaopen_filesystem, 1);
	lua_pop(L, 1);  /* remove lib */
	luaL_requiref(L, "rio", luaopen_rio, 1);
	lua_pop(L, 1);  /* remove lib */

	if(thread_safe)
	{
		luaL_requiref(L, "tdriver", luaopen_tdriver, 1);
		lua_pop(L, 1);  /* remove lib */
		luaL_requiref(L, "tick", luaopen_tick, 1);
		lua_pop(L, 1);  /* remove lib */
	}
	return L;
}


static struct msg_node* create_and_push_msg_node(struct lua_service *service, const void* data, size_t size, size_t malloc_size)
{
	struct msg_node* node;
	if(!data)
	{
		return 0;
	}
	if(malloc_size < size)
	{
		malloc_size = size;
	}
	node = (struct msg_node*)malloc(sizeof(struct msg_node));
	if (!node) return 0;
	node->data = malloc(malloc_size);
	if(!node->data)
	{
		free(node);
		return 0;
	}
	memcpy(node->data, data, size);
	node->size = malloc_size;
	node->next = 0;

	if(service->tail)
	{
		service->tail->next = node;
		service->tail = node;
	}
	else
	{
		service->head = node;
	}
	++service->node_size;
	service->tail = node;

	return node;
} 

static void* thread_run(void* param)
{
	int top;
	int err;

	struct lua_service* service;
	service = (struct lua_service*)param;

	top = lua_gettop(service->L);
	if((err = lua_pcall(service->L, top - 2, LUA_MULTRET, 1)))
	{
		return (void*)(size_t)err;
	}
	return 0;
}

struct lua_service*
lua_service_c_create_help(lua_State *L, int thread_safe)
{
	struct lua_service *service;
	struct lua_service **pservice;

	service = (struct lua_service*)malloc(SERVICE_SIZE);
	if(!service)
	{
		return 0;
	}
	service->ser_buff_size = SERVICE_SIZE - (int)(service->ser_buff - (char*)service);
	service->L = init_lua_script(thread_safe);
	if(!service->L)
	{
		free(service);
		return 0;
	}
	service->pt = 0;
	serialize_init(&service->ser, 0, service->ser_buff, service->ser_buff_size);
	net_atomic_flag_clear(&service->flag);
	service->state = 1;
	service->head = 0;
	service->tail = 0;
	service->node_size = 0;

	if(L)
	{
		// from L copy path
		lua_getglobal(L, "package");
		lua_getfield(L, -1, "path");
		
		lua_getglobal(service->L, "package");
		lua_pushstring(service->L, lua_tostring(L, -1));
		lua_setfield(service->L, -2, "path");
		lua_remove(service->L, -1);

		lua_remove(L, -1);
		lua_remove(L, -1);
	}

	// set global service to service->L
	pservice = (struct lua_service **)lua_newuserdata(service->L, sizeof(struct lua_service *));
	if(!pservice)
	{
		lua_close(service->L);
		free(service);
		return 0;
	}
	*pservice = service;
	luaL_getmetatable(service->L, LUA_SERVER_CURSOR);
	lua_setmetatable(service->L, -2);
	lua_setglobal(service->L, "service");
	return service;
}

struct lua_service*
lua_service_c_create(const char* path)
{
	struct lua_service* service;
	service = lua_service_c_create_help(NULL, 1);
	if(service)
	{
		lua_getglobal(service->L, "package");
		lua_pushstring(service->L, path);
		lua_setfield(service->L, -2, "path");
		lua_remove(service->L, -1);
	}
	return service;
}

static int set_default_module(lua_State* L, const char* module_name)
{
	int top;
	top = lua_gettop(L);
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_newtable(L);
	lua_setfield(L, -2, module_name);
	lua_settop(L, top);
	return 0;
}

static int get_filename_from_module(lua_State* L, const char* module_name, char* filename)
{
	const char* name;
	size_t size;
	int top;
	top = lua_gettop(L);
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "searchpath");
	lua_pushstring(L, module_name);
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "path");
	lua_remove(L, -2);
	if(lua_pcall(L, 2, LUA_MULTRET, 0))
	{
		return -1;
	}
	if(lua_isnil(L, -1))
	{
		return -2;
	}
	name = lua_tolstring(L, -1, &size);
	memcpy(filename, name, size);
	filename[size] = 0;
	lua_settop(L, top);
	return 0;
}

int	lua_service_c_run(struct lua_service* service, const char* module_name, int argc, char** argv)
{
	int i;
	struct msg_node* node;
	char lua_file[256];
	if(argc > 0)
	{
		for(i = 0; i < argc; ++i)
		{
			lua_pushstring(service->L, argv[i]);
		}
		if(serialize_pack(&service->ser, service->L, 1))
		{
			serialize_clean(&service->ser);
			return -1;
		}
		node = create_and_push_msg_node(service, service->ser.data, service->ser.size, service->ser.size);
		if(!node)
		{
			serialize_clean(&service->ser);
			return -1;
		}

		serialize_clean(&service->ser);
		lua_settop(service->L, 0);
	}
	if(get_filename_from_module(service->L, module_name, lua_file))
	{
		printf("get_filename_from_module error : %s", lua_tostring(service->L, -1));
		lua_close(service->L);
		free(service);
		return -1;
	}
	set_default_module(service->L, module_name);
	lua_pushcfunction(service->L, error_handle);
	if(luaL_loadfile(service->L, lua_file))
	{
		printf("load error : %s", lua_tostring(service->L, -1));
		lua_close(service->L);
		free(service);
		return -1;
	}
	service->state = 1;
	if(lua_pcall(service->L, 0, LUA_MULTRET, 1))
	{
		printf("do error : %s",lua_tostring(service->L, -1));
		lua_close(service->L);
		free(service);
		return -1;
	}
	lua_close(service->L);
	free(service);
	return 0;
}

static int	lua_service_create(lua_State *L)
{
	struct lua_service *service;
	struct lua_service **pservice;

	service = lua_service_c_create_help(L, 0);
	if(!service)
	{
		return 0;
	}
	pservice = (struct lua_service **)lua_newuserdata(L, sizeof(struct lua_service *));
	if(!pservice)
	{
		lua_close(service->L);
		free(service);
		luaL_argcheck(L, 0, 0, "lua_service_create L lua_newuserdata is nil");
		return 0;
	}
	*pservice = service;
	luaL_getmetatable(L, LUA_SERVER_CURSOR);
	lua_setmetatable(L, -2);
	return 1;
}


static int lua_service_run(lua_State *L)
{
	struct lua_service *service;
	struct lua_service **pservice;
	const char* module_name;
	struct msg_node* node;
	char lua_file[256];

	pservice = (struct lua_service**) luaL_checkudata(L, 1, LUA_SERVER_CURSOR);
	luaL_argcheck (L, pservice != 0, 1, LUA_SERVER_CURSOR" lua_service_stop #1 is nil");
	service = *pservice;
	module_name = luaL_checkstring(L, 2);
	service->ser.support_flag = USER_DATA_FLAG;
	if(serialize_pack(&service->ser, L, 3))
	{
		service->ser.support_flag = 0;
		serialize_clean(&service->ser);
		luaL_argcheck(L, 0, 3, "lua_service_run serialize_pack params error");
		return 0;
	}

	// from now can not pack userdata
	service->ser.support_flag = 0;

	node = create_and_push_msg_node(service, service->ser.data, service->ser.size, service->ser.size);
	if(!node)
	{
		serialize_clean(&service->ser);
		luaL_argcheck(L, 0, 3, "lua_service_run create_msg_node null error");
		return 0;
	}

	serialize_clean(&service->ser);


	if(get_filename_from_module(service->L, module_name, lua_file))
	{
		luaL_argcheck(L, 0, 2, "get_filename_from_module faild");
		return 0;
	}
	set_default_module(service->L, module_name);
	set_default_module(L, module_name);


	lua_pushcfunction(service->L, error_handle);
	luaL_loadfile(service->L, lua_file);
	if(pthread_create(&service->pt, 0, thread_run, service))
	{
		service->pt = 0;
		lua_close(service->L);
		free(service);
		luaL_argcheck(L, 0, 1, "lua_service_run faild");
		return 0;
	}
	service->state = 1;
	lua_pushinteger(L, 1);
	return 1;
}

static int	lua_service_stop(lua_State *L)
{
	struct lua_service** pservice;
	struct lua_service* service;
	pservice = (struct lua_service**) luaL_checkudata(L, 1, LUA_SERVER_CURSOR);
	luaL_argcheck (L, pservice != 0, 1, LUA_SERVER_CURSOR" lua_service_stop #1 is nil");
	service = *pservice;
	luaL_argcheck (L, service!= 0, 1, LUA_SERVER_CURSOR" lua_service_stop #1 service is nil");

	net_lock(&service->flag);
	service->state = 0;
	net_unlock(&service->flag);
	return 0;
}

static int	lua_service_is_run(lua_State *L)
{
	struct lua_service** pservice;
	struct lua_service* service;
	int state;

	pservice = (struct lua_service**) luaL_checkudata(L, 1, LUA_SERVER_CURSOR);
	luaL_argcheck (L, pservice != 0, 1, LUA_SERVER_CURSOR" lua_service_stop #1 is nil");
	service = *pservice;
	luaL_argcheck (L, service!= 0, 1, LUA_SERVER_CURSOR" lua_service_stop #1 service is nil");

	net_lock(&service->flag);
	state = service->state;
	net_unlock(&service->flag);
	lua_pushboolean(L, state);
	return 1;
}



static int	lua_servivce_node_size(lua_State *L)
{
	struct lua_service** pservice;
	struct lua_service* service;
	int node_size;
	pservice = (struct lua_service**) luaL_checkudata(L, 1, LUA_SERVER_CURSOR);
	luaL_argcheck (L, pservice != 0, 1, LUA_SERVER_CURSOR" lua_servivce_node_size #1 is nil");
	service = *pservice;
	luaL_argcheck (L, service!= 0, 1, LUA_SERVER_CURSOR" lua_servivce_node_size #1 service is nil");
	net_lock(&service->flag);
	node_size = service->node_size;
	net_unlock(&service->flag);
	lua_pushinteger(L, node_size);
	return 1;
}

static int	lua_service_push_data(lua_State *L)
{
	struct lua_service** pservice;
	struct lua_service* service;
	struct msg_node* node;
	void* data;
	size_t size;
	void* data2;
	size_t size2;
	int top;

	pservice = (struct lua_service**) luaL_checkudata(L, 1, LUA_SERVER_CURSOR);
	top = lua_gettop(L);
	luaL_argcheck (L, pservice != 0, 1, LUA_SERVER_CURSOR" lua_service_push_data #1 is nil");
	service = *pservice;
	luaL_argcheck (L, service!= 0, 1, LUA_SERVER_CURSOR" lua_service_push_data #1 service is nil");
	luaL_argcheck(L, lua_islightuserdata(L, 2), 2, LUA_SERVER_CURSOR" lua_service_push_data 2 must be lightuserdata");
	
	data = (void*)lua_touserdata(L, 2);
	size = (size_t)luaL_checkinteger(L, 3);
	data2 = 0;
	size2 = 0;
	if(top == 5)
	{
		luaL_argcheck(L, lua_islightuserdata(L, 4), 4, LUA_SERVER_CURSOR" lua_service_push_data 4 must be lightuserdata");
		data2 = (void*)lua_touserdata(L, 4);
		size2 = (size_t)luaL_checkinteger(L, 5);
	}

	net_lock(&service->flag);
	if(!service->state)
	{
		net_unlock(&service->flag);
		return 0;
	}

	node = create_and_push_msg_node(service, data, size, size + size2);
	if(!node)
	{
		net_unlock(&service->flag);
		return 0;
	}
	if(size2 > 0)
	{
		memcpy(node->data + size, data2, size2);
	}
	serialize_clean(&service->ser);
	net_unlock(&service->flag);
	lua_pushinteger(L, 1);
	return 1;
}


static int	lua_service_pop_msg(lua_State *L)
{
	struct lua_service** pservice;
	struct lua_service* service;
	struct msg_node* node;
	int ret;

	pservice = (struct lua_service**) luaL_checkudata(L, 1, LUA_SERVER_CURSOR);
	luaL_argcheck (L, pservice != 0, 1, LUA_SERVER_CURSOR" lua_service_pop_msg #1 is nil");
	service = *pservice;
	luaL_argcheck (L, service!= 0, 1, LUA_SERVER_CURSOR" lua_service_pop_msg #1 service is nil");

	net_lock(&service->flag);
	if(!service->state || !service->head)
	{
		net_unlock(&service->flag);
		return 0;
	}
	node = service->head;
	if(!node)
	{
		net_unlock(&service->flag);
		return 0;
	}
	service->head = node->next;
	if(node == service->tail)
	{
		service->tail = 0;
	}
	// to keep thread safe, the unpack must in lock scope
	ret = deserialize_unpack(&service->deser, L, (char*)node->data, node->size, -1);
	free(node->data);
	free(node);
	--service->node_size;
	net_unlock(&service->flag);
	return ret;
}

static int lua_service_pack(lua_State *L)
{
	struct lua_service** pservice;
	struct lua_service* service;

	pservice = (struct lua_service**) luaL_checkudata(L, 1, LUA_SERVER_CURSOR);
	luaL_argcheck (L, pservice != 0, 1, LUA_SERVER_CURSOR" lua_service_pack #1 is nil");
	service = *pservice;
	luaL_argcheck (L, service!= 0, 1, LUA_SERVER_CURSOR" lua_service_pack #1 service is nil");

	if(!service->state)
	{
		return 0;
	}
		// this function not thread safe
	luaL_argcheck (L, service->L == L, 1, LUA_SERVER_CURSOR" lua_service_pack #1 service not thread safe");
	
	serialize_clean(&service->ser);
	// to keep thread safe, the pack must in lock scope
	if(serialize_pack(&service->ser, L, 2))
	{
		luaL_argcheck(L, 0, 2, "lua_service_pack serialize_pack params error");
		return 0;
	}
	lua_pushlightuserdata(L, (void*)service->ser.data);
	lua_pushinteger(L, service->ser.size);
	return 2;
}

static int lua_service_unpack(lua_State *L)
{
	struct lua_service** pservice;
	struct lua_service* service;
	int ret;
	void* data;
	size_t size;
	int start;

	pservice = (struct lua_service**) luaL_checkudata(L, 1, LUA_SERVER_CURSOR);
	luaL_argcheck (L, pservice != 0, 1, LUA_SERVER_CURSOR" lua_service_unpack #1 is nil");
	service = *pservice;
	luaL_argcheck (L, service!= 0, 1, LUA_SERVER_CURSOR" lua_service_unpack #1 service is nil");

	if(!service->state)
	{
		return 0;
	}

	luaL_argcheck (L, service->L == L, 1, LUA_SERVER_CURSOR" lua_service_unpack #1 service not thread safe");

	data = (void*)lua_touserdata(L, 2);
	size = (size_t)luaL_checkinteger(L, 3);
	ret = -1;
	start = 0;
	if(lua_gettop(L) > 3)
	{
		ret = (int)luaL_checkinteger(L, 4);
	}
	if(lua_gettop(L) > 4)
	{
		start = (int)luaL_checkinteger(L, 5);
	}
	ret = deserialize_unpack(&service->deser, L, (char*)data, size, ret);
	return ret - start;
}

static int	lua_service_join(lua_State *L)
{
	void* result;
	struct lua_service** pservice;
	struct lua_service* service;
	int ret_cnt;
	struct msg_node* node;

	ret_cnt = 0;

	pservice = (struct lua_service**) luaL_checkudata(L, 1, LUA_SERVER_CURSOR);
	luaL_argcheck (L, pservice != 0, 1, LUA_SERVER_CURSOR" lua_service_join #1 is nil");
	service = *pservice;
	luaL_argcheck (L, service!= 0, 1, LUA_SERVER_CURSOR" lua_service_join #1 service is nil");
	result = 0;
	if(service->pt)
	{
		pthread_join(service->pt, &result);
	}
	// service->ser can use in this thread now
	serialize_clean(&service->ser);

	if(result == 0)
	{
		// ++ret_cnt;
		// lua_pushinteger(L, 0);
		serialize_pack(&service->ser, service->L, 2);
		ret_cnt += deserialize_unpack(&service->deser, L, service->ser.data, service->ser.size, -1);
		serialize_clean(&service->ser);
	}
	else
	{
		// lua_pushinteger(L, (lua_Integer)result);
		// lua_pushstring(L, lua_tostring(service->L, -1));
		ret_cnt = 0;
	}
	node = service->head;
	while(node)
	{
		service->head = node->next;
		free(node->data);
		free(node);
		node = service->head;
	}
	service->node_size = 0;
	lua_close(service->L);
	free(service);
	*pservice = 0;

	return ret_cnt;
}

static int	lua_server_sleep(lua_State *L)
{
	int mseonds;
	mseonds = (int)luaL_checkinteger(L, 1);
	net_thread_sleep(mseonds);
	return 0;
}


int luaopen_server(lua_State*L)
{
	struct luaL_Reg service_index_methods[] = {
			{"run", lua_service_run},
			{"stop", lua_service_stop},
			{"is_run", lua_service_is_run},
			{"join", lua_service_join},
			{"push_data", lua_service_push_data},
			{"node_size", lua_servivce_node_size},
			{"pop_msg", lua_service_pop_msg},
			{"pack", lua_service_pack},
			{"unpack", lua_service_unpack},
			{"create", lua_service_create},
			{"sleep", lua_server_sleep},
			{0, 0}
	};

	struct luaL_Reg server_methods[] = {
			{0, 0}
	};

	if(!luaL_newmetatable(L, LUA_SERVER_CURSOR))
	{
		return 0;
	}

	luaL_setfuncs (L, service_index_methods, 0);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);
	lua_pushliteral(L, "__serialize");
	lua_pushinteger(L, sizeof(struct lua_service*));
	lua_settable(L, -3);
	lua_pop(L, 1);

	lua_newtable(L);
	luaL_setfuncs(L, server_methods, 0);
	return 1;
}
