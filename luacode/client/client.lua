

package.path = "../luacode/?.lua"
require("common.log")
local Env = require("common.Env")
local network = require("common.network")
local msg_define = require("common.msg_define")
local sec_tick = require("common.sec_tick")
local sid = service:pop_msg()
sid = tonumber(sid)
Env.SERVER_TYPE = "client"
Env.SERVER_ID = sid

tdriver.open()
net.open(32, 1)
local connect_fd = net.connect("127.0.0.1", 3214)
if connect_fd == 0 then
    print("connect error 0 ", connect_fd)
end

local csession = {}
csession.fd = connect_fd


network.reg_session(connect_fd, csession)

function csession.close(session, reason)
    net.socket_close(session.fd)
    network.del_session(session.fd)
    print("GREEN close one session", session.fd, reason)
end

function csession:handle_error(events)
    print("handle_error", csession.fd)
    net.socket_close(csession.fd, 1)
    network.del_session(csession.fd)
end

function csession:handle_msg(msg_size, msg)
    print("recv one msg", csession.fd, service:unpack(msg, msg_size))
    -- csession:close("ok")
end

function csession:handle_connect(err)
    if err ~= 0 then
        print("connect error", csession.fd, err)
    else
        print("connect ok", csession.fd, err)
        net.socket_cfg(csession.fd, net.enByte16, 16, 16);
        local role_id = math.random(132142131, 232142131)

        network.send_msg(csession, msg_define.I_am_client, {role_id=role_id, role_name = "random"})

        network.send_msg(csession, msg_define.logic_test, 3, "hello logic, i am client")
    end
end
print("BLUE start client", service:is_run())
math.randomseed(os.time())
local tick_cnt = 0
while service:is_run() do
    count = 0
    while network.net_round() and count < 7 do
        count = count + 1
    end

    tdriver:update()

    if tdriver.get_new_sec() > 0 then
        print ("GREEN now seconds", tdriver.get_pass_seconds())
        sec_tick.do_round()
    end
    if count == 0 then
        service.sleep(100)
    end
end


sec_tick.__gc()
net.close()
tick.close()

