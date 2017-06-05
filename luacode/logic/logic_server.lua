

package.path = "../luacode/?.lua"

require("common.log")
local Env = require("common.Env")
local network = require("common.network")
local sec_tick = require("common.sec_tick")

local sid = service:pop_msg()
sid = tonumber(sid)
Env.SERVER_TYPE = "logic"
Env.SERVER_ID = sid

tdriver.open()
net.open(1024, 2)

local lsession = network.net_listen("0.0.0.0", 3214, 5, net.enByte16, 1024, 64)
if not lsession then
    error("ERROR listen faild!")
    net.close()
    os.exit(false)
end

print("BLUE listen ok ", lsession.ip, lsession.port)

__init__ = true
Env.load_all_module("logic")
__init__ = false
print("BLUE all module ok")


while service:is_run() do
    local count = 0
    while network.net_round() and count < 7 do
        count = count + 1
    end

    tdriver.update()

    if tdriver.get_new_sec() > 0 then
         -- print ("GREEN now seconds", tdriver.get_pass_seconds())
        count = count + sec_tick.do_round()
    end
    if count == 0 then
        service.sleep(100)
    end
end

sec_tick.__gc()
net.close()
tick.close()

