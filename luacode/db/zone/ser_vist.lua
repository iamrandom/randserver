
local m = {}

local server_msg = require("common.msg_define")

local function load_table()
end

local function save_table()
end

local function load_big_table()
end

local function save_big_table()
end

m[server_msg.DB_Request_Load_Table] = load_table
m[server_msg.DB_Request_Save_Table] = save_table
m[server_msg.DB_Request_Load_BigTable] = load_big_table
m[server_msg.DB_Request_Save_BigTable] = save_big_table


return m