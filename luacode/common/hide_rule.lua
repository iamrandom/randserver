

local m = {}



function m.get_db_port(server_id)
    return 30000 + server_id
end



return m