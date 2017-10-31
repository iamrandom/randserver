
package.path = "../luacode/?.lua"
require("common.log")

local Env = require("common.Env")
local sec_tick = require("common.sec_tick")
local network = require("common.network")
local msg_define = require("common.msg_define")
local create_table = require("db.table.create_table")

__init__ = true
local db_session = require("db.db_session")
__init__ = false

local sid = service:pop_msg()
sid = tonumber(sid)
Env.SERVER_TYPE = "db"
Env.SERVER_ID = sid

tdriver.open()
net.open(44, 2)

local db_info = {
		db = "random_test1",
		ip = "192.168.8.111",
		port = 3306,
		password = "ly_101sql",
		user="root"
	}
create_table.run(db_info)

local lsession = network.net_listen("0.0.0.0", Env.SERVER_ID + 30000, 5, net.enByte24, 1024 * 16, 1024 * 16)
if not lsession then
    error("ERROR listen faild!")
    net.close()
    os.exit(false)
end
print("BLUE listen ok", lsession.ip, lsession.port)

db_session.main_zone_service = service.create()

for i = 1, 2, 1 do
	local ds = service.create()
	ds:run("db.zone.zone_service", db_session.main_zone_service, db_info, i)
	table.insert(db_session.zone_services, ds)
end

for i = 1, 2, 1 do
	local ls = service.create()
	ls:run("db.log.log_service", i)
	table.insert(db_session.log_services, ls)
end

while service:is_run() do
    local count = 0
    while network.net_round() and count < 20 do
        count = count + 1
    end

    tdriver.update()

    if tdriver.get_new_sec() > 0 then
        print ("GREEN now seconds", tdriver.get_pass_seconds(), db_session.msg_count)
        print(0, db_session.main_zone_service:node_size())
        for i, t in ipairs(db_session.zone_services) do
            print(i, t:node_size())
        end

        count = count + sec_tick.do_round()
    end
    if count == 0 then
        -- print("db_server  sleep")
        service.sleep(1)
    end
end


for i, t in ipairs(db_session.zone_services) do
	t:stop()
end
for i, t in ipairs(db_session.log_services) do
    t:stop()
end
db_session.main_zone_service:stop()

for i, t in ipairs(db_session.zone_services) do
	print(t:join())
end
for i, t in ipairs(db_session.log_services) do
	t:join()
end
db_session.main_zone_service:join()

sec_tick.__gc()
net.close()
tick.close()

