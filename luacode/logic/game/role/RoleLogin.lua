local m = {}

local msg_define = require("common.msg_define")
local network = require("common.network")
local role_session = require("logic.session.role_session")
local db_session = require("logic.session.db_session")

local function ClientRequestLogin(session, login_info)
    local role_id = login_info.role_id
    if not role_id then
        session:close("client login no role_id")
        return
    end
    local all_roles = role_session.all_roles
    local role = all_roles[role_id]
    local newfd = session.fd
    if not role then
        session.relogin = -1
        session.type = "role"
    else
        network.del_session(role.fd)
        for k, v in pairs(role) do
            session[k] = v
        end
    end
    session.fd = newfd
    session.role_id = login_info.role_id
    session.role_name = login_info.role_name
    session.relogin = session.relogin + 1
    all_roles[role_id] = session
    setmetatable (session, nil)
    setmetatable(session, {__index = role_session})

    -- print("GREEN role ", role.role_id, role.role_name, role.type, "login ok", role)
    -- print("GREEN total role:", all_roles)
    session:send_msg("ok I login success", session.role_id, session.role_name)
    local ss = {{{a=1}, b=2}}
    ss.ss = ss
    -- print(getmetatable(db_session.main_db))
    db_session.main_db.send_msg(db_session.main_db, msg_define.DB_Request_Load_Account, role_id, "xxdfsfsdf", "fdsfdsf", ss)
end

local function hello_random(session, ...)
    -- print("GREEN hello random", ...)
    session:send_msg("ok I response you msg ", session.role_id, session.role_name)
    db_session.main_db.send_msg(db_session.main_db, msg_define.DB_Request_Load_Account, session.role_id, "xxdfsfsdf222", "fdsfdsf222", {{{a=1}, b=2}})
end

if __init__ then
    network.reg_who(msg_define.I_am_client, ClientRequestLogin)
    role_session.reg_msg(msg_define.logic_test, hello_random)
end

return m





