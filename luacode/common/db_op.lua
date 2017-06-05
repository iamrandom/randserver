local m = {}

local log = require("common.log")

local function adjust_line(con, ser, table_info, line_dict)
	for _, v in ipairs(table_info) do
		local field = v.field
		local f = line_dict[field]
		if f then
			isblob = string.find(v.type, "blob")
			if isblob then
				if ser:pack(f) ~= 0 then
					log.Error("adjust_line pack error", field, f)
				else
					line_dict[field] = con:escape(ser:get_string())
					ser:clean()
				end
			end
		end
	end
end

function m.update_insert_table(con, ser, table_info, all_line_dict)

	if not table_info.PriKey then
		log.Error("only table contail a PriKey can use update_insert_table function")
		return nil
	end
	local prk_field = table_info.PriKey.field

	local sql_table = {}
	table.insert(sql_table, "INSERT INTO ")
	table.insert(sql_table, table_info.name)
	table.insert(sql_table, " (")
	
	table.insert(sql_table, prk_field)

	local first_line = all_line_dict[1]
	local names = {}
	for i, v in ipairs(table_info) do
		if v.field ~= prk_field and first_line[v.field] then
			table.insert(sql_table, ",")
			table.insert(sql_table, v.field)
			table.insert(names, v.field)
		end
	end

	table.insert(sql_table, ") VALUES")
	for i, line_dict in ipairs(all_line_dict) do

		adjust_line(con, ser, table_info, line_dict)		
		if i ~= 1 then table.insert(sql_table, ",") end
		table.insert(sql_table, " (")		
		table.insert(sql_table, "'")
		table.insert(sql_table, line_dict[prk_field])
		table.insert(sql_table, "'")
		for j, k in ipairs(names) do
			table.insert(sql_table, ",")
			table.insert(sql_table, "'")
			table.insert(sql_table, line_dict[k])
			table.insert(sql_table, "'")
		end
		table.insert(sql_table, ") ")
	end
	table.insert(sql_table, " ON DUPLICATE KEY UPDATE ")
	
	for i, k in ipairs(names) do
		if i ~= 1 then table.insert(sql_table, ",") end
		table.insert(sql_table, k)
		table.insert(sql_table, "=VALUES(")
		table.insert(sql_table, k)
		table.insert(sql_table, ")")
	end
	table.insert(sql_table, ";")
	local ui_sql = table.concat(sql_table)
	return con:execute(ui_sql)
end

function m.insert_table( con, ser, table_info, all_line_dict )
	local sql_table = {}

	table.insert(sql_table, "INSERT INTO ")
	table.insert(sql_table, table_info.name)
	table.insert(sql_table, " (")
	
	local first_line = all_line_dict[1]
	local names = {}
	for i, v in ipairs(table_info) do
		if first_line[v.field] then
			if #names ~= 0 then
				table.insert(sql_table, ",")
			end
			table.insert(sql_table, v.field)
			table.insert(names, v.field)
		end
	end

	table.insert(sql_table, ") VALUES")
	for i, line_dict in ipairs(all_line_dict) do
		adjust_line(con, ser, table_info, line_dict)
		if i ~= 1 then table.insert(sql_table, ", ") end
		table.insert(sql_table, " (")
		for j, k in ipairs(names) do
			if j ~= 1 then table.insert(sql_table, ", ") end
			table.insert(sql_table, "'")
			table.insert(sql_table, line_dict[k])
			table.insert(sql_table, "'")
		end
		table.insert(sql_table, ") ")
	end
	table.insert(sql_table, ";")
	local i_sql = table.concat(sql_table)
	return con:execute(i_sql)
end

function m.update_set_table( con, ser, table_info, line_dict )
	if not table_info.PriKey then
		log.Error("only table contail a PriKey can use update_table function", table_info.name)
		return nil
	end
	local prk_field = table_info.PriKey.field
	if not line_dict[prk_field] then
		log.Error("only line_dict contail a PriKey can use update_table function", table_info.name, prk_field)
		return nil
	end

	adjust_line(con, ser, table_info, line_dict)
	
	local sql_table = {}
	table.insert(sql_table, "UPDATE ")
	table.insert(sql_table, table_info.name)
	table.insert(sql_table, " SET ")
	local names = {}
	for i, v in ipairs(table_info) do
		local field = v.field
		if field ~= prk_field and line_dict[field] then
			if #names ~= 0 then
				table.insert(sql_table, ", ")
			end
			table.insert(sql_table, v.field)
			table.insert(sql_table, "=")
			table.insert(sql_table, "'")
			table.insert(sql_table, line_dict[field])
			table.insert(sql_table, "'")
			table.insert(names, field)
		end
	end
	table.insert(sql_table, " WHERE ")
	table.insert(sql_table, prk_field)
	table.insert(sql_table, "=")
	table.insert(sql_table, "'")
	table.insert(sql_table, line_dict[prk_field])
	table.insert(sql_table, "'")

	table.insert(sql_table, ";")
	local u_sql = table.concat(sql_table)
	print(u_sql)
	return con:execute(u_sql)
end

function m.delete_from_table(con, ser, table_info, key_dict)
	if not table_info.PriKey then
		log.Error("only table contail a PriKey can use delete_from_table function", table_info.name)
		return nil
	end
	local prk_field = table_info.PriKey.field
	local sql_table = {}
	table.insert(sql_table, "DELETE FROM ")
	table.insert(sql_table, table_info.name)
	if key_dict then
		table.insert(sql_table, " WHERE ")
		table.insert(sql_table, prk_field)
		table.insert(sql_table, " IN (")
		for i, v in ipairs(key_dict) do
			if i ~= 1 then table.insert(sql_table, ", ") end
			table.insert(sql_table, "'")
			table.insert(sql_table, v)
			table.insert(sql_table, "'")
		end
		table.insert(sql_table, " )")
	end

	table.insert(sql_table, ";")
	local u_sql = table.concat(sql_table)
	print(u_sql)
	return con:execute(u_sql)
end


function fit_to_type(deser, field_type, s)
	local t = string.lower(field_type)
	if string.find(t, "int") then
		return math.tointeger(s)
	end
	if string.find(t, "float") or string.find(t, "double") then
		return tonumber(s)
	end
	if string.find(t, "blob") then
		return deser:unpack(s, -1)
	end
	return s
end

function m.select_from_table(con, deser, table_info, key_dict, columns)
	if not table_info.PriKey then
		log.Error("only table contail a PriKey can use delete_from_table function", table_info.name)
		return nil
	end
	local prk_field = table_info.PriKey.field
	local sql_table = {}
	table.insert(sql_table, "SELECT ")

	local column_dict = {}
	for i, v in ipairs(columns) do
		column_dict[v] = 1
	end

	local names = {}
	for i, v in ipairs(table_info) do
		local fn = v.field
		if (not column_dict) or column_dict[fn] then
			if #names ~= 0 then table.insert(sql_table, ", ") end
			table.insert(sql_table, fn)
			table.insert(names, v)
		end
	end

	table.insert(sql_table, " FROM ")
	table.insert(sql_table, table_info.name)

	if key_dict then
		table.insert(sql_table, " WHERE ")
		table.insert(sql_table, prk_field)
		table.insert(sql_table, " IN (")
		for i, v in ipairs(key_dict) do
			if i ~= 1 then table.insert(sql_table, ", ") end
			table.insert(sql_table, "'")
			table.insert(sql_table, v)
			table.insert(sql_table, "'")
		end
		table.insert(sql_table, " )")
	end

	table.insert(sql_table, ";")
	local u_sql = table.concat(sql_table)
	print(u_sql)
	local cur, errorInfo = con:execute(u_sql)
	if not cur then
		return nil
	end
	
	local row_len = cur:numrows()
	local all_lines = {}
	for i = 1, row_len, 1 do
		local d = {}
		local line = {}
		cur:fetch(d)
		for j, v in ipairs(names) do
			line[v.field] = fit_to_type(deser, v.type, d[j])
		end
		table.insert(all_lines, line)
	end

	cur:close()
	return all_lines


end



return m
