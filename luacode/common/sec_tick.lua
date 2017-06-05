local m = {}

require("tick")

m.funs = {}

function m.__gc()
	m.funs = {}
end

function m.reg(seconds, fun, ...)
	local now_seconds =  tdriver.get_pass_seconds()
	local tick_id = tick.reg(now_seconds + seconds)
	m.funs[tick_id] = table.pack(fun, ...)
	return tick_id
end

function m.unreg(tick_id)
	tick.unreg(tick_id)
	m.funs[tick_id] = nil
end

function m.trigger(tick_id)
	tick.unreg(tick_id)
	local fs = m.funs[tick_id]
	if not fs then return end
	m.funs[tick_id] = nil
	try_catch(table.unpack(fs))
end


m.hour_funs = {} -- check per hour
m.minute_funs = {} -- check per minute
m.second_funs = {} -- check per minute
m.check_time = nil, nil, nil
m.cur_hour_funs = {} -- cur hour run fun
m.cur_minute_funs = {} -- cur minute run fun
m.cur_second_funs = {} -- cur second run fun

local time_env = {}

function m.call_per_hour(fun, hour)
	local info = {fun=fun, hour=load(hour, nil, "t", time_env)}
	table.insert(m.hour_funs, info)
end

function m.call_per_minute(fun, minute)
	local info = {fun=fun, minute=load(minute, nil, "t", time_env)}
	table.insert(m.minute_funs, info)
end

function m.call_per_second(fun, minute, second)
	local info = {fun=fun, minute=load(minute, nil, "t", time_env), second=load(second, nil, "t", time_env)}
	table.insert(m.second_funs, info)
end

local function deriver_per_second(year, month, day, hour, minute, second, weekday, yearday)

	local check_day, check_hour, check_minut = m.check_time
	local ch, cm = false, false
	if check_day ~= day or check_hour ~= hour then
		ch = true
		cm = true
	end
	if check_minut ~= minute then
		cm = true
	end

	m.check_time = day, minute, second
	if ch then
		local chf = {}
		for k, v in ipairs(m.hour_funs) do
			if try_catch(v.hour) then
				table.insert(chf, v)
			end
		end
		m.cur_hour_funs = chf
	end
	if cm then
		local chm = {}
		for k, v in ipairs(m.minute_funs) do
			if try_catch(v.minute) then
				table.insert(chm, v)
			end
		end
		m.cur_minute_funs = chm

		local chs = {}
		for k, v in ipairs(m.second_funs) do
			if try_catch(v.minute) then
				table.insert(chs, v)
			end
		end
		m.cur_second_funs = chs
	end

	time_env.y = year
	time_env.m = month
	time_env.d = day
	time_env.H = hour
	time_env.M = minute
	time_env.S = second
	time_env.wd = weekday
	time_env.yd = yearday

	if second == 0 then
		if minute == 0 then
			if minute == 0 and second == 0 then
				for k, v in ipairs(m.cur_hour_funs) do
					try_catch(v.fun)
				end
			end
		end
		for k, v in ipairs(m.cur_minute_funs) do
			try_catch(v.fun)
		end
	end
	for k, v in ipairs(m.cur_second_funs) do
		try_catch(v.fun)
	end
end

function m.do_round()
	local now_seconds = tdriver.get_pass_seconds()
	local index = tick.lt(now_seconds + 1)
	local ret = index
	
	while index > 0 do
		index = index - 1
		local tick_id = tick.index(0)
		m.trigger(tick_id)
	end
	deriver_per_second(tdriver.get_UTC_tm())
	return ret
end


return m

