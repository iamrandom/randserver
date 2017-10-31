local m = {}

local Env = require("common.Env")
local event = require("common.event")
local sec_tick = require("common.sec_tick")
local network = require("common.network")
local msg_define = require("common.msg_define")
local hide_rule = require("common.hide_rule")

m.db_sessions = {}
m.db_msg_count = 0


function m.get_main_db_info()
	return {ip="192.168.8.210", port = hide_rule.get_db_port(Env.SERVER_ID), server_id=Env.SERVER_ID}
end

function m.get_merger_dbs()
	return nil
end

local session_funs = {}
session_funs.name = "db_session"

function session_funs.send_msg(session, msg_type, ...)
    local msg, msg_size = service:pack(msg_type, ...)
    return net.socket_write(session.fd, msg, msg_size)
end

function session_funs.handle_msg(session, msg_size, msg)
	-- print("BLUE recv msg from DB ", service:unpack(msg, msg_size))
	m.db_msg_count = m.db_msg_count + 1
end

function session_funs.handle_connect(session, err)
	if err == 0 then
		if 0 == net.socket_cfg(session.fd, session.enByte , session.read_msg_cnt, session.write_buff_cnt) then
			print("BLUE connect ", session.db_info, "success")
			if session.db_id == Env.SERVER_ID then
				-- event.trigger_fun(event.Main_DB_Reconnect, session)
				session:send_msg(msg_define.I_am_logic)
				print("GREEN send msg:", session:send_msg(msg_define.DB_Request_Load_Account, 1, "hello db, i am logic"))
			end
			session.wait_sec = 1
			return
		else
			warn("handle_connect", session.db_info, "socket_cfg faild")
		end
	end

	warn("handle_connect", session.fd, session.db_info, "faild")
	network.del_session(session.fd)
	
	session.tick_id = sec_tick.reg(session.wait_sec, session.connect_db, session)
end

function session_funs.handle_error(session)
	warn("connect", session.fd, session.db_info, "faild")
	network.del_session(session.fd)
	session.wait_sec = 1
	session.tick_id = sec_tick.reg(1, session.connect_db, session)
end

function session_funs.connect_db(session)
	if not session.tick_id then return end
	local db_info = session.db_info

	session.tick_id = nil

	local nd = net.connect(db_info.ip, db_info.port)
	if session.wait_sec < 30 then
		session.wait_sec = session.wait_sec + 1
	end
	if nd > 0 then
		network.reg_session(nd, session)
		print( "GREEN try to connect_db ", nd, db_info)
	else
		print("YELLOW connect", session.db_info, "faild")
		session.tick_id = sec_tick.reg(session.wait_sec, session.connect_db, session)
	end
end

function m.connect_main_db()
	return m.connect_db(m.get_main_db_info())
end

function m.connect_db(db_info)
	local session = {}
	m.db_sessions[db_info.server_id] = session
	session.db_info = db_info
	session.db_id = db_info.server_id
	session.wait_sec = 1
	session.enByte = net.enByte24
	session.read_msg_cnt = 1024 * 16
	session.write_buff_cnt = 1024 * 16
	setmetatable(session, {__index = session_funs})
	
	session.tick_id = sec_tick.reg(1, session.connect_db, session)
	return session
end


if __init__ then
	m.main_db = m.connect_main_db()
end

return m
