
local m = {}
m.code_path="../luacode"
m.config_path="../config"
m.SERVER_ID = 0
m.SERVER_TYPE = None


local function search_lua_files(files, folderpath, mdle)
	for _, fn in ipairs({filesystem.list_file(folderpath .. "/*.lua")}) do
		fn = string.sub(fn, 1, -5)
		table.insert(files, mdle .. fn)		
	end
	for _, fn in ipairs({filesystem.list_file(folderpath .. "/*", true)}) do
		if fn ~= "." and fn ~= ".." then
			search_lua_files(files, folderpath .. "/".. fn, mdle .. fn .. ".")
		end
	end
end

function m.load_all_module(root)
	root_path = root.gsub(root, "%.", "/")
	local code_path = m.code_path .. "/" .. root_path
	local all_modules = {}
	search_lua_files(all_modules, code_path, root .. ".")
	for _, mdle in ipairs(all_modules) do
		require(mdle)
	end
end


return m

