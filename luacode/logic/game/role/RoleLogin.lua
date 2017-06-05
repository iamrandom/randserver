local m = {}

local msg_define = require("common.msg_define")
local network = require("common.network")
local role_session = require("logic.session.role_session")

local function ClientRequestLogin(session, login_info)
    local role_id = login_info.role_id
    if not role_id then
        session:close("client login no role_id")
        return
    end
    all_roles = role_session.all_roles
    local role = all_roles[role_id]
    if not role then
        role = session
        all_roles[role_id] = role
        session.role_id = login_info.role_id
        session.role_name = login_info.role_name
        setmetatable (session, nil)
        setmetatable(session, {__index = role_session})
    else
        role.fd = session.fd
        network.reg_session(role.fd, role)
    end

    print("GREEN role ", role.role_id, role.role_name, role.type, "login ok", role)
    print("GREEN total role:", all_roles)
    role:send_msg("ok I login success", role.role_id, role.role_name)
end

local function hello_random(session, ...)
    print("GREEN hello random", ...)
end

if __init__ then
    network.reg_who(msg_define.I_am_client, ClientRequestLogin)
    role_session.reg_msg(msg_define.logic_test, hello_random)
end

return m





