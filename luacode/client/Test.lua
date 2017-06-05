

-- package.path = "../luacode/?.lua"

require("debug")
local log = require("common.log")
print (package.path)
-- print (package.loaded)
-- for k in pairs(package.loaded) do
--     print(k)
-- end
-- local hot_update = require("common.hot_update")

print (log)
local m = {}

local f = load("return a < 3", nil, "t", m)
m.a = 2
print(f())
m.a = 4
print(f())

-- hot_update.update_module("client.Test1")


local ts = service.create()
ts:run("db.service.db_service", 1, 2, 3, 4)
service.sleep(1000)
ts:stop()
print(ts:join())

return {}