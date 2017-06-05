local m = {}

require("debug")


local function get_function_ups()

end



function m.update_module(module_name)
    for k, v in pairs(package.loaded) do
        print(k)
    end
    local om = package.loaded[module_name]
    if not om then
        return nil
    end
    local path = package.searchpath(module_name, package.path)
    local nf = loadfile(path)
    -- local info = debug.getinfo(om, "S")
    -- print(info)
end

return m
