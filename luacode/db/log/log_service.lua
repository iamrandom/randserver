

require("common.log")
local Env = require("common.Env")

local index = service:pop_msg()
print("BLUE log service ", index ,"run")

__init__ = true
Env.load_all_module("db.log")
__init__ = false

local function command_msg(nd, msgType, ...)
	if not msgType then return nil end
	local err, ret = xpcall(m[msgType] , errfun, ...)
	return err
end


while service:is_run() do
	local is_busy = 0
	while command_msg( service:pop_msg() ) ~= nil do
		is_busy = is_busy + 1
	end

	if is_busy == 0 then
		service.sleep(200)
	end
end


