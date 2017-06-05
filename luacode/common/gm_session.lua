
local m = {}

local msg_define = require("common.msg_define")
local listen_session = require("common.listen_session")

if true == __init__ then
    listen_session.reg_who(msg_define.GM_Login, m)
end

function m.handle_msg(session, msg_size, msg)

end


return m
