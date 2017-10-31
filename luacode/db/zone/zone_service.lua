
require("common.log")
local Env = require("common.Env")
local role_vist = require("db.zone.role_vist")
local ser_vist = require("db.zone.ser_vist")

local main_service, db_info, index = service:pop_msg()


local function command_msg(nd, msgType, ...)
	if not msgType then return nil end
	-- print("BLUE command_msg recive msg", nd, msgType, ...)
	local fun = role_vist[msgType]
	if not fun then
		fun = ser_vist[msgType]
	end
	if fun then
		fun(nd, ...)
		return true
	end
	return nil
end

local function command_service_msg()
	local err, ret = try_catch(command_msg, service:pop_msg())
	return ret
end

local function command_main_service_msg()
	local err, ret =  try_catch(command_msg, main_service:pop_msg())
	return ret
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
		-- print("zone_service sleep", index)
		service.sleep(1)
	end
end






