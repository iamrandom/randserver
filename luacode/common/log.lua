
local m = {}
old_print = print

function print_trace()
	print(debug.traceback(""))
end

function errfun(errmsg)
	rio.unlock()
	rio.lock()
	old_print(debug.traceback(tostring(errmsg)))
	rio.unlock()
end

function try_catch(fun, ...)
	local err, ret = xpcall(fun , errfun, ...)
	if err then return ret end
	return nil
end

local show_return_modles = {}
local print_break = false

local function hook_return(...)
	if print_break then return end
	local info = debug.getinfo(2)
	if not info then return end
	if info.currentline == -1 then return end
	local mdpath = info["short_src"]
	local md = show_return_modles[mdpath]
	if not mdpath or not md then return end
	
	local s = string.format("%s:%d in <%s> %s", md, info.currentline,  info.func, info.name)
	print_break = true
	print("BLUE", s)
	print_break = false
end

function show_return(module_name)
	local module_path = package.searchpath(module_name, package.path)
	show_return_modles[module_path] = module_name
	print("BLUE show_return", module_name, module_path)
	debug.sethook(hook_return, "r")
end

function clear_return()
	show_return_modles = {}
	print_break = false
end


error = function(...)
	print("RED", ...)
	errfun()
end

warn = function (...)
	print("YELLO", ...)
end

local function print_help(strTable, refTable, v)
	local t = type(v)
	if t == "table" then
		if refTable[v] then
			-- local sv = tostring(v)
			-- sv =  string.gsub(sv, "table: ", "")
			table.insert(strTable, "{...")
			-- table.insert(strTable, sv)
			table.insert(strTable, "}")
			return
		end
		refTable[v] = 1
		local isHead = false
		table.insert(strTable, "{")
		for ii,vv in pairs(v) do
			if not isHead then
				isHead = true
			else
				table.insert(strTable, ", ")
			end
			print_help(strTable, refTable, ii)
			table.insert(strTable, ":")
			print_help(strTable, refTable, vv)
		end
		table.insert(strTable, "}")
	elseif t == "string" then
		table.insert(strTable, v)
	else
		table.insert(strTable, tostring(v))
	end
end


function m.to_string(argOne, ...)
	local strTable = {}
	local isHead = false
	for k, v in pairs({argOne, ...}) do
		if not isHead then
			isHead = true
		else
			table.insert(strTable, "	")
		end
		local refTable = {}
		print_help(strTable, refTable, v)
	end
	return table.concat(strTable)
end

local now_print = function(...)
	local s = m.to_string(...)
	local color = nil
	local c = string.sub(s,1, 1)
	if c == "R" or c == "E" then
		if string.sub(s,1, 3) == "RED" or string.sub(s,1, 5) == "ERROR" then
			color = rio.red
		end
	elseif c == "Y" or c == "W" then
		if string.sub(s,1, 5) == "YELLO" or string.sub(s,1, 4) == "WARN" then
			color = rio.yello
		end
	elseif c == "B" then
		if string.sub(s,1, 4) == "BLUE" then
			color = rio.blue
		end
	elseif c == "G" then
		if string.sub(s,1, 5) == "GREEN" then
			color = rio.green
		end
	end

	rio.lock()
	if color  then
		color()
	end
	old_print(s)
	if color and rio.white then
		rio.white()
	end
	rio.unlock()
end

print = function(...)
	xpcall(now_print , errfun, ...)
end

return m
