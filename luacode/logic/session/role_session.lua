local m = {}
print(debug.getinfo(1))

local network = require("common.network")
local msg_define = require("common.msg_define")
local sec_tick = require("common.sec_tick")

function m.handle_error(session, events)
    if session then
        session:close("error")
    end
end

function m.handle_msg(session, msg_size, msg)
    local msg_type = service:unpack(msg, msg_size, 1)
    print("now should be here", msg_size)
    local fun = m[msg_type]
    if fun then
        fun(session, service:unpack(msg, msg_size, -1, 1))
    else
        warn("role session can't distribute msg", msg_type)
    end
end

function m.close(session, reason)
    net.socket_close(session.fd)
    network.del_session(session.fd)
    print("GREEN close one role", session.fd, reason, session.role_id, session.role_name)
    m.all_roles[session.role_id] = nil
end

function m.reg_msg(msg_type, msg_fun)
    m[msg_type] = msg_fun
end

m.send_msg = network.send_msg


if __init__ then
    m.all_roles = {}
end

return m