

require("common.log")
local Env = require("common.Env")

local index = service:pop_msg()
print("BLUE log service ", index ,"run")

local log_vist = require("db.log.log_vist")

local function command_msg(nd, msgType, ...)
	if not msgType then return nil end
	local err, ret = xpcall(m[msgType] , errfun, ...)
	return err
end

local function command_msg(nd, msgType, ...)
	if not msgType then return nil end
	local fun = log_vist[msgType]
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

while service:is_run() do
	local is_busy = 0
	while command_msg( service:pop_msg() ) ~= nil do
		is_busy = is_busy + 1
	end

	if is_busy == 0 then
		-- print("log service sleep", index)
		service.sleep(1)
	end
end


