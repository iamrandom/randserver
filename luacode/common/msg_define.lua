
local m = {}


-- Who
m.I_am_client = 1
m.I_am_gm = 2
m.I_am_logic = 3
m.I_am_world = 4
m.I_am_db = 5

-- LOGIC
m.logic_test = 999
m.Msg_Callback = 1000

-- DB - SERVER msg
m.DB_Login = 10000
m.DB_Request_Load_Account = 10001           --DB账号信息
m.DB_Response_Load_Account = 10001          --DB账号信息
m.DB_Request_Load_Role = 10002              --DB加载信息
m.DB_Response_Load_Role = 10002             --DB加载信息
m.DB_Request_Load_Command = 10003           --DB离线命令信息
m.DB_Response_Load_Command = 10003          --DB离线命令信息
m.DB_Request_Load_Mail = 10004              --DB邮件信息
m.DB_Response_Load_Mail = 10004             --DB邮件信息

m.DB_Request_Save_Role = 10005              --DB保存角色
m.DB_Response_Save_Role = 10005
             --DB保存角色
m.DB_Request_Load_Table = 10006             --DB加载字典
m.DB_Response_Load_Table = 10006             --DB加载角色
m.DB_Request_Save_Table = 10007             --DB保存字典
m.DB_Response_Save_Table = 10007           --DB保存字典
m.DB_Request_Save_BigTable = 10008          --DB返回保存特大字典
m.DB_Response_Save_BigTable = 10008         --DB返回保存特大字典
m.DB_Request_Load_BigTable = 10009          --DB返回保存特大字典
m.DB_Response_Load_BigTable = 10009         --DB返回保存特大字典

m.DB_Save_Transaction = 10010 -- DB保存日志事务

return m
