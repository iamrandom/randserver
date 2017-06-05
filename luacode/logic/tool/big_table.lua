

local m = {}
local log = require("common.log")
local sec_tick = require("common.sec_tick")
local msg_define = require("common.msg_define")
local event = require("common.event")

m.tables = {}
m.tick_id = nil

function m.create_big_table(name, after_load_fun)
	if m.tables[name] then
		log.Error("create_big_table repert name ", name)
	end
	local bt = {}
	bt.name = name
	bt.after_load_fun = after_load_fun
	bt.data = {}
	bt.load_ok = false
	m.tables[name] = bt
	return bt
end


function m.on_main_db_reconnect(main_db)
	if m.tick_id then return end
	m.tick_id = nil
	local need_load_cnt = 0
	for i, bt in ipairs(m.tables) do
		if not bt.load_ok then
			need_load_cnt = need_load_cnt + 1
			main_db:send_msg(msg_define.DB_Request_Load_Table, bt.name)
			print("BLUE start to load big table", bt.name)
		end
	end
	if need_load_cnt > 0 then
		m.tick_id = sec_tick.reg(7, m.on_main_db_reconnect, db_id)
	end
end


event.reg_fun(event.Main_DB_Reconnect, on_main_db_reconnect)



return m
