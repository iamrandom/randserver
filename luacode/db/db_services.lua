
local m = {}
m.__index = m

local network = require("common.network")
local msg_define = require("common.msg_define")
local role_vist = require("db.zone.role_vist")
local ser_vist = require("db.zone.ser_vist")
local log_vist = require("db.log.log_vist")

function m.handle_error(session, events)
    network.handle_error(session, events)
    print("BLUE close one session", session)
end

function m.handle_msg(session, msg_size, msg)
    print("BLUE recive msg", service:unpack(msg, msg_size))
    local msg_type, dbid = service:unpack(msg, msg_size, 2)

    if log_vist[msg_type] then
         local sv = m.log_services[math.random(#m.log_services)]
         sv:push_data(msg, msg_size)
         return true
    elseif ser_vist[msg_type] or role_vist[msg_type] then
        local sv = m.zone_services[(dbid % #m.zone_services) + 1]
        sv:push_data(msg, msg_size)
        return true
    else
        warn("db service can't distribute msg", service:unpack(msg, msg_size))
        return
    end
end


if __init__ then
    -- 主线程
    m.main_zone_service = nil
    -- 分线程
    m.zone_services = {}
    -- 日志线程
    m.log_services = {}

    network.reg_who(msg_define.I_am_logic, m)
end

return m
