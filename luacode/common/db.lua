local modle = {}

local log = require("common.log")

modle.PriKey = 1
modle.MulKey = 2
modle.UniKey = 3

local function get_old_columns(conn, table_name)
	local cur, err = conn:execute("SHOW COLUMNS FROM " .. table_name)
	if err then
		print("GREEN", err)
		return nil, err
	end
	local  row = {}
	cur:fetch(row, "a")
	local dict = {}
	while row do
		local column = {}
		for k, v in pairs(row) do
			column[string.lower(k)] = v
		end
		column.field = string.lower(column.field)
		if column.null then
			if string.find( string.lower(column.null), "yes") then
				column.null = true
			else
				column.null = false
			end
		end
		if column.key then
			local key = string.lower(column.key)
			if key == "pri" then column.key = modle.PriKey
			elseif key == "mul" then column.key = modle.MulKey
			elseif key == "uni" then column.key = modle.UniKey
			else column.key = nil end
		end
		dict[column.field] = column
		row = cur:fetch(row,  'a')
	end
	return dict
end

--field, type, null, default, key, extra
local function check_columns(conn, new_table, old_columns, add_columns, update_columns, update_infos, remove_keys, add_keys)
	for k, v in ipairs(new_table) do
		local field = string.lower(v.field)
		if not old_columns[field] then
			add_columns[#add_columns + 1] = v
		else
			local oldV = old_columns[field]
			local need_update = false;
			local check_error = nil;
			if not v.null ~= not oldV.null then need_update = true; update_infos[#update_infos +1] = field .. " null no the same" end
			if v.default ~= oldV.default then
				if v.default and oldV.default then
					if tostring(v.default) ~= oldV.default  then need_update = true; update_infos[#update_infos +1] = field .. " default no the same" end
				else
					need_update = true; update_infos[#update_infos +1] = field .. " default no the same"
				end
			end
			if v.extra ~= oldV.extra then
				if v.extra and oldV.extra then
					if string.lower(v.extra) ~= oldV.extra then need_update = true; update_infos[#update_infos +1] = field .. " default no the same" end
				elseif  (v.extra and #v.extra ~= 0) or (oldV.Extra and #oldV.extra == 0) then
					need_update = true
					update_infos[#update_infos +1] = field .. " extra no the same"
				end
			end
			if v.key ~= oldV.key then
				--检测key
				if  v.key == modle.PriKey or oldV.key == modle.PriKey then return "no same prikey"  end
				if oldV.key then remove_keys[#remove_keys + 1] = oldV end
				if v.key then add_keys[#add_keys + 1] = v end
			end

			local vType = string.lower( v.type )
			local oldVType = oldV.type
			if vType ~= oldVType then
				local vNameB, vNameE = string.find(vType, "%w+")
				local vName = string.sub(vType, vNameB, vNameE)
				local oldVNameB, oldVNameE = string.find(oldVType, "%w+")
				local oldVName = string.sub(oldVType, oldVNameB, oldVNameE)
				local vTypeSize = 0
				local sizeB, sizeE = string.find(vType, "%(%w+%)")
				if sizeB then vTypeSize = tonumber(string.sub(vType, sizeB + 1, sizeE -1)) end
				local vOldTypeSize = 0
				sizeB, sizeE = string.find(oldVType, "%(%w+%)")
				if sizeB then vOldTypeSize = tonumber(string.sub(oldVType, sizeB + 1, sizeE -1)) end
				local noVUnsignedB = string.find(vType, "unsigned", vNameE)
				local noOldVUnsignedB = string.find(oldVType, "unsigned", oldVNameE)
				noVUnsignedB = not noVUnsignedB
				noOldVUnsignedB = not noOldVUnsignedB
				if vName ~= oldVName or vTypeSize ~= vOldTypeSize or noVUnsignedB ~=noOldVUnsignedB then
					update_infos[#update_infos +1] = field .. " type no the same"
					need_update = true
				end
				print("GREEN", vName, oldVName, vTypeSize, vOldTypeSize, noVUnsignedB, noOldVUnsignedB)
			end
			if need_update then update_columns[#update_columns + 1] = v end
		end
	end
	return nil
end

local function column_sql(column)
	local sql = "`" .. column.field .. "` " .. column.type
	if not column.null then sql = sql .. " NOT NULL" end
	if column.extra then sql = sql .. " " .. column.extra end
	if column.default ~= nil then
		local value = tostring(column.default)
		if #value > 0 then sql = sql .. " DEFAULT '" .. value ..  "'" end
	end
	if column.comment and #column.comment > 0 then sql = sql .. " COMMENT '" .. column.comment .. "'" end
	return sql
end

local function key_sql(column)
	if not column.key then return nil end
	local field = string.lower(column.field)
	if column.key == modle.PriKey then return "PRIMARY KEY (`" .. field .. "`)" end
	if column.key == modle.MulKey then return "KEY `" .. field .. "` (`" .. column.field .. "`)" end
	if column.key == modle.UniKey then return "UNIQUE KEY `" .. field .. "` (`" .. column.field .. "`)" end
	return nil
end

local function creae_table(conn, table_name, new_table)
	--创建表
	local sql = "CREATE TABLE `" .. table_name .. "` ("
	for index, column in ipairs(new_table) do
		if index == 1 then
			sql = sql .. column_sql(column)
		else
			sql = sql .. "," .. column_sql(column)
		end
	end
	for index, column in ipairs(new_table) do
		local keysql = key_sql(column)
		if keysql then sql = sql .. "," .. keysql end
	end
	sql = sql .. ")"
	if new_table.engine then
		sql = sql .. " ENGINE=" .. new_table.engine
	else
		sql = sql .. " ENGINE=InnoDB"
	end
	if new_table.auto_increment then  sql = sql .. " AUTO_INCREMENT=" .. new_table.auto_increment end
	if new_table.default_charset then  sql = sql .. " DEFAULT CHARSET=" .. new_table.default_charset end
	sql = sql .. " CHECKSUM=1 DELAY_KEY_WRITE=1 ROW_FORMAT=DYNAMIC"
	print("GREEN", sql)
	return conn:execute(sql)
end

local function update_table(conn, table_name, new_table, status)
	--    获取老的列类型
	old_columns, err = get_old_columns(conn, table_name)
	if err then return  err end

	local add_columns = {}
	local update_columns = {}
	local update_infos = {}
	local remove_keys = {}
	local add_keys = {}
	--    检测columns中需要更新的部分
	local err = check_columns(conn, new_table, old_columns, add_columns, update_columns, update_infos, remove_keys, add_keys)
	if err then return err end
	--    更新column
	for i, v in ipairs(update_infos) do print("GREEN", v) end
	local all_sql = {}

	for k, column in ipairs(update_columns) do
		all_sql[#all_sql + 1] = "ALTER TABLE " .. table_name .. " MODIFY " .. column_sql(column)
	end
	for k, column in ipairs(remove_keys) do
		all_sql[#all_sql + 1] = "ALTER TABLE " .. table_name .. " DROP KEY " .. string.lower(column.field)
	end
	for k, column in ipairs(add_columns) do
		all_sql[#all_sql + 1] = "ALTER TABLE " .. table_name .. " ADD " .. column_sql(column)
	end
	for k, column in ipairs(add_keys) do
		if column.key == modle.MulKey then all_sql[#all_sql + 1] = "ALTER TABLE " .. table_name .. " ADD KEY `" .. string.lower(column.field) .. "`(`" .. column.field .."`)" end
		if column.key == modle.UniKey then all_sql[#all_sql + 1] = "ALTER TABLE " .. table_name .. " ADD UNIQUE KEY `" .. string.lower(column.field) .. "`(`" .. column.field .."`)" end
	end
	--    检测table中需要更新的部分
	if status.auto_increment then
		status.auto_increment = tonumber(status.auto_increment)
	else
		status.auto_increment = 1
	end
	if not new_table.auto_increment then new_table.auto_increment = 1 end
	if new_table.auto_increment > status.auto_increment then
		all_sql[#all_sql + 1] = "ALTER TABLE " .. table_name .. " AUTO_INCREMENT=" .. new_table.auto_increment
	end
	local error = false
	for k, sql in ipairs(all_sql) do
		print("GREEN", sql)
		local cur, err = conn:execute(sql)
		if err then 
			return cur, err
--			log.Show(err) break
		end
	end
end

function modle.create_or_update_table(conn, table_name, new_table)
	if not table_name then
		return "no table name"
	end
	local cur, err = conn:execute("SHOW TABLE STATUS LIKE '" .. table_name .. "'")
	if err then return err end
	local row = cur:fetch({}, "a")
	if not row then
		return creae_table(conn, table_name, new_table)
	else
		local status = {}
		for k, v in pairs(row) do
			status[string.lower(k)] = v
		end
		return update_table(conn, table_name, new_table, status)
	end
end


return modle


