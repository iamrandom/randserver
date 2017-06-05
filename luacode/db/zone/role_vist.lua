local m = {}

local msg_define = require("common.msg_define")

local function load_account()

end

local function load_role()

end

local function load_command()

end

local function load_mail()

end

local function save_role()

end

m[msg_define.DB_Request_Load_Account] = load_account
m[msg_define.DB_Request_Load_Role] = load_role
m[msg_define.DB_Request_Load_Command] = load_command
m[msg_define.DB_Request_Load_Mail] = load_mail
m[msg_define.DB_Request_Save_Role] = save_role


return m