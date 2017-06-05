
require("common.log")
local Env = require("common.Env")

local main_service, db_info, index = service:pop_msg()

__init__ = true
Env.load_all_module("db.zone")
__init__ = false


local function command_msg(nd, msgType, ...)
	if not msgType then return nil end
	print("BLUE command_msg recive msg", msgType, ...)
	local fun = m[msgType]
	if fun then
		return fun(...)
	end
	return nil
end

local function command_service_msg()
	return try_catch(command_msg, service:pop_msg())
end

local function command_main_service_msg()
	return try_catch(command_msg, main_service:pop_msg())
end

while service:is_run() do
	local is_busy = 0
	
	while command_service_msg() ~= nil do
		is_busy = is_busy + 1
	end
	if main_service then
		if command_main_service_msg() ~= nil then
		 	is_busy = is_busy + 1
		end
	end
	if is_busy == 0 then
		service.sleep(200)
	end
end






