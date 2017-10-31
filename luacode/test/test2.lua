
require("common.log")

print("BLUE recv", service:pop_msg())

local function command_msg(nd, msgType, ...)
    if not msgType then return nil end
    print("BLUE ", nd, msgType, ...)
end


while service:is_run() do
    local is_busy = 0
    while command_msg( service:pop_msg() ) ~= nil do
        is_busy = is_busy + 1
    end

    if is_busy == 0 then
        service.sleep(200)
    end
end