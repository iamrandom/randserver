/*
 * luanet.c
 *
 *  Created on: 2015年9月16日
 *      Author: Random
 */

#include <stdlib.h>
#include <string.h>
#include "../net/net_atomic.h"
#include "../net/net_service.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"
#include "extra_lualib.h"
#include <pthread.h>


#define LUA_NET	"Lua Net"
#define LUA_NET_CURSOR	"Net cursor"

typedef struct
{
	struct net_service*			ns;
	struct buff_pool*			pool;
	int 						thread_cnt;
	pthread_t 					delay_pt;
	pthread_t 					*pts;
	volatile int				run_state;
}lua_net_data;

lua_net_data nd;

static void* net_thread_run_delay(void* param)
{
	int cnt;
	lua_net_data* pnd;
	pnd = (lua_net_data*)param;
	while(pnd->run_state > 0)
	{
		cnt = net_delay(pnd->ns, 64);
		if(cnt == 0)
		{
			net_thread_sleep(100);
		}
	}

	return 0;
}

static void* net_thread_run_work(void* param)
{
	lua_net_data* pnd;
	pnd = (lua_net_data*)param;
	while(pnd->run_state)
	{
		net_wait(pnd->ns, 500);
	}
	return 0;
}

static int	lua_net_open(lua_State *L)
{
	int i;
	lua_Integer net_size;
	lua_Integer thread_cnt;

	net_size = luaL_checkinteger(L, 1);
	thread_cnt = luaL_checkinteger(L, 2);

	memset(&nd, 0, sizeof(lua_net_data));
	nd.pool = buff_pool_create((size_t)1024, (size_t)64);
	if(!nd.pool)
	{
		return 0;
	}
	nd.ns = net_create((int)net_size);
	if(!nd.ns)
	{
		buff_pool_release(nd.pool);
		return 0;
	}
	
	nd.pts = (pthread_t*)malloc(sizeof(pthread_t) * thread_cnt);
	if(!nd.pts)
	{
		net_close(nd.ns);
		buff_pool_release(nd.pool);
		return 0;
	}
	nd.thread_cnt = thread_cnt;
	nd.run_state = 1;
	
	pthread_create(&nd.delay_pt, 0, net_thread_run_delay, &nd);
	for(i = 0; i < thread_cnt; ++i)
	{
		pthread_create(&nd.pts[i], 0, net_thread_run_work, &nd);
	}
	lua_pushinteger(L, 1);
	return 1;
}

static int	lua_net_close(lua_State *L)
{
	int i;
	void* result;
	
	if(nd.run_state)
	{
		nd.run_state = 0;
		pthread_join(nd.delay_pt, &result);
		for(i = 0; i < nd.thread_cnt; ++i)
		{
			pthread_join(nd.pts[i], &result);
		}
		net_close(nd.ns);
		buff_pool_release(nd.pool);
		lua_pushinteger(L, 1);
		return 1;
	}
	nd.ns = 0;
	nd.pool = 0;
	return 0;
}

static int lua_net_listen(lua_State *L)
{
	const char* ip;
	unsigned short port;
	unsigned short listen_cnt;
	ip = luaL_checkstring(L, 1);
	port = (unsigned short)luaL_checkinteger(L, 2);
	listen_cnt = (unsigned short)luaL_checkinteger(L, 3);
	lua_pushinteger(L, (lua_Integer)net_listen(nd.ns, ip, port, listen_cnt));
	return 1;
}

static int lua_net_accept(lua_State *L)
{
	net_socket nsocket;
	nsocket = (net_socket)luaL_checkinteger(L, 1);
	lua_pushinteger(L, (lua_Integer)net_accept(nd.ns, nsocket));
	return 1;
}

static int lua_net_queue(lua_State *L)
{
	struct net_event ev;
	if(!net_queue(nd.ns, &ev, 1))
	{
		return 0;
	}
	lua_pushinteger(L, (lua_Integer)ev.nd);
	lua_pushinteger(L, ev.events);
	lua_pushinteger(L, (lua_Integer)ev.data);
	return 3;
}

static int lua_net_connect(lua_State *L)
{
	const char* ip;
	unsigned short port;

	ip = luaL_checkstring(L, 1);
	port = (unsigned short)luaL_checkinteger(L, 2);
	lua_pushinteger(L, (lua_Integer)net_connect(nd.ns, ip, port));
	return 1;
}

static int lua_net_socket_close(lua_State *L)
{
	net_socket nsocket;
	char send_rest;

	nsocket = (net_socket)luaL_checkinteger(L, 1);
	send_rest = 0;
	if(lua_gettop(L) >= 2)
	{
		send_rest = (char)luaL_checkinteger(L, 2);
	}
	net_socket_close(nd.ns, nsocket, send_rest);
	return 0;
}

static int lua_net_socket_read(lua_State *L)
{
	net_socket nsocket;
	int read_size;
	void* pMsg;

	nsocket = (net_socket)luaL_checkinteger(L, 1);
	pMsg = 0;
	read_size = net_socket_read(nd.ns, nsocket, (void*)&pMsg, sizeof(pMsg));
	lua_pushinteger(L, (lua_Integer)read_size);
	lua_pushlightuserdata(L, pMsg);
	return 2;
}

static int lua_net_socket_write(lua_State *L)
{
	net_socket nsocket;
	void* pMsg;
	int msg_size;
	int err;

	nsocket = (net_socket)luaL_checkinteger(L, 1);

	luaL_argcheck(L, lua_islightuserdata(L, 2), 2, "lua_net_socket_write #2 must lightuserdata");
	pMsg = lua_touserdata(L, 2);
	msg_size = (int)luaL_checkinteger(L, 3);
	err = net_socket_write(nd.ns, nsocket, pMsg, msg_size, 1);
	lua_pushinteger(L, err);
	return 1;
}

static int lua_net_socket_cfg(lua_State *L)
{
	net_socket nsocket;
	struct net_config config;

	nsocket = (net_socket)luaL_checkinteger(L, 1);
	config.enByte = (enum EnByteSize)luaL_checkinteger(L, 2);
	config.read_buff_cnt = luaL_checkinteger(L, 3);
	config.write_buff_cnt = luaL_checkinteger(L, 4);
	config.read_buff_version = RECV_BUFF_USE_QUEUE;
	config.pool = nd.pool;

	lua_pushinteger(L, net_socket_cfg(nd.ns, nsocket, &config));
	return 1;
}


static int lua_net_socket_ctl(lua_State *L)
{
	net_socket nsocket;
	param_type param;

	nsocket = (net_socket)luaL_checkinteger(L, 1);
	if(2 > lua_gettop(L))
	{
		lua_pushinteger(L, (lua_Integer)net_socket_ctl(nd.ns, nsocket, 0));
	}
	else
	{
		param = (param_type)luaL_checkinteger(L, 2);
		lua_pushinteger(L, (lua_Integer)net_socket_ctl(nd.ns, nsocket, &param));
	}
	return 1;
}

static int lua_net_socket_size(lua_State* L)
{
	lua_pushinteger(L, (lua_Integer)net_socket_size(nd.ns));
	return 1;
}

static int lua_net_free(lua_State* L)
{
	void* pMsg;
	luaL_argcheck(L, lua_islightuserdata(L, 1), 1, "lua_net_free #1 must lightuserdata");
	pMsg = lua_touserdata(L, 1);
	free(pMsg);
	return 0;
}

static int lua_net_socket_ip_port(lua_State* L)
{
	char ip[128];
	unsigned short port;
	net_socket nsocket;

	memset(ip, 0, sizeof(ip));
	port = 0;
	nsocket = (net_socket)luaL_checkinteger(L, 1);

	if(!net_socket_ip_port(nd.ns, nsocket, ip, &port))
	{
		return 0;
	}
	lua_pushstring(L, ip);
	lua_pushinteger(L, (lua_Integer)port);
	return 2;
}

int luaopen_net(lua_State*L)
{
	struct luaL_Reg net_methods[] = {
			{"close", lua_net_close},
			{"connect", lua_net_connect},
			{"listen", lua_net_listen},
			{"accept", lua_net_accept},
			{"queue", lua_net_queue},
			{"socket_cfg", lua_net_socket_cfg},
			{"socket_close", lua_net_socket_close},
			{"socket_read", lua_net_socket_read},
			{"socket_write", lua_net_socket_write},
			{"socket_ctl", lua_net_socket_ctl},
			{"size", lua_net_socket_size},
			{"open", lua_net_open},
			{"free", lua_net_free},
			{"ip_port", lua_net_socket_ip_port},
			{0, 0}
	};

	lua_newtable(L);
	luaL_setfuncs(L, net_methods, 0);
	lua_pushinteger(L, Eve_Accept);
	lua_setfield(L, -2, "Eve_Accept");
	lua_pushinteger(L, Eve_Read);
	lua_setfield(L, -2, "Eve_Read");
	lua_pushinteger(L, Eve_Connect);
	lua_setfield(L, -2, "Eve_Connect");
	lua_pushinteger(L, Eve_Error);
	lua_setfield(L, -2, "Eve_Error");
	lua_pushinteger(L, Eve_Post_Listen_Error);
	lua_setfield(L, -2, "Eve_Post_Listen_Error");

	lua_pushinteger(L, enByte8);
	lua_setfield(L, -2, "enByte8");
	lua_pushinteger(L, enByte16);
	lua_setfield(L, -2, "enByte16");
	lua_pushinteger(L, enByte24);
	lua_setfield(L, -2, "enByte24");
	lua_pushinteger(L, enByte31);
	lua_setfield(L, -2, "enByte31");
	return 1;
}


#ifdef __cplusplus
}
#endif