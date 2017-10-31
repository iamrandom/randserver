
require("common.log")
-- local mysql = require("luasql.mysql")
-- local m = mysql.mysql()

local ds = service.create()
ds:run("test.test2", 1, 2, 3)
local d = {1, 2, 3, {4, 5, 6}}
while service:is_run() do

    service.sleep(1000)
    print("GREEN -------------------------- ")
    data, size = service:pack("fsdfs", "vvdsfsd", "fdsfgsdf", d, 1, 2, 3, d)
    -- print ("package data ", size)
    ds:pack(1, 2, 3)
    ds:push_data(data, size)
end

ds:stop()
print(ds.join())