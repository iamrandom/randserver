

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
net.open(1024, 1)
math.randomseed(os.time())



local csession = {}
function csession.close(session, reason)
    network.del_session(session.fd)
    print("GREEN close one session", session.fd, reason)
end

function csession.handle_error(session, events)
    print("handle_error", session.fd)
    network.del_session(session.fd)
end

function csession.handle_msg(session, msg_size, msg)
    -- print("recv one msg", session.fd, service:unpack(msg, msg_size))
    network.send_msg(session, msg_define.logic_test, 3, "hello logic, i am client")
    if math.random(1, 1000) < 2 then
        session:close("ok")
    end
    -- session:close("ok")
end

function csession.handle_connect(session, err)
    if err ~= 0 then
        print("connect error", session.fd, err)
    else
        print("connect ok", session.fd, err)
        net.socket_cfg(session.fd, net.enByte16, 16, 16);
        local role_id = math.random(132142131, 232142131)

        network.send_msg(session, msg_define.I_am_client, {role_id=role_id, role_name = "random"})

        network.send_msg(session, msg_define.logic_test, 3, "hello logic, i am client")
        network.send_msg(session, msg_define.logic_test, 3, "hello logic, i am client2")
    end
end
print("BLUE start client", service:is_run())



local tick_cnt = 0
while service:is_run() do
    local count = 0
    while network.net_round() and count < 30 do
        count = count + 1
    end

    tdriver:update()

    if tdriver.get_new_sec() > 0 then
        print ("GREEN now seconds", tdriver.get_pass_seconds())
        sec_tick.do_round()
        print("now connect cnt : ", network.count, net.size())
    end
    if count == 0 then
        if network.count < 1000 then
            local connect_fd = net.connect("127.0.0.1", 3214)
            if connect_fd == 0 then
                print("connect error 0 ", connect_fd)
            else
                local cssn = {}
                cssn.fd = connect_fd
                network.reg_session(connect_fd, cssn)
                setmetatable(cssn, {__index = csession})
            end
        end
        service.sleep(1)
    end
end


sec_tick.__gc()
net.close()
tick.close()

