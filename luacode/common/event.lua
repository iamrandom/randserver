local m = {}

local log = require("common.log")


function m.reg_fun(eventType, fun)
	local funs = m[eventType]
	if not funs then
		funs = {}
		m[eventType] = funs
	end
	table.insert(funs, fun)
end

function m.unreg_fun(eventType, fun)
	local funs = m[eventType]
	if not funs then return end
	for i, f in ipairs(funs) do
		if f == fun then
			table.remove(funs, i)
			return
		end
	end
end

function m.trigger_fun(eventType, ...)
	local funs = m[eventType]
	if not funs then return end
	for i, fun in ipairs(funs) do
		log.try_catch(fun, ...)
	end
end



m.Main_DB_Reconnect = 1			-- 主DB重新链接ok

return m

