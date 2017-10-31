local m = {}

function m.read(filename)
    print("try to open  ", filename)
    local f = io.open(filename, "r")
    local config = {}
    local columns = {}
    
    for line in f:lines() do
        config[#config + 1] = line
    end
    local line = config[1]

    for index, line in ipairs(config) do
        print(index, line)
        if index == 1 then
            for value in string.gmatch(line, "[^\t]+") do
                columns[#columns + 1] = {name=value}
            end
        elseif index ==2  then
        end
        for value in string.gmatch(line, "[^\t]+") do
            
            column_index = column_index + 1
        end
        if index > 2 then
            break
        end
    end
    print(#config)
    f:close()
end


local path = "D:/git_workspace/randserver/bin/config/"
m.read(path.."aabc/examp#Sheet1.txt")


return m