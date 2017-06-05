local m = {}

require("luasql")
local db = require("common.db")

-- role data
local role_data = {
	{field="role_id", type="bigint(20) unsigned", null=false, command="角色ID", key=db.PriKey, extra="AUTO_INCREMENT"},
	{field="role_name", type="varchar(128)", null=false,  command="角色名称", key=db.UniKey},
	{field="account", type="varchar(128)",null=false, command="账号", key=db.UniKey},
	{field="data", type="mediumblob", null=true, command="数据"},

	{field="vip", type="bigint(30)", null=false, default=0, command="角色VIP"},
	{field="level", type="bigint(30)", null=false, default=0, command="角色level"},
	{field="exp", type="bigint(30)", null=false, default=0, command="角色exp"},
	{field="charge_money", type="bigint(30)", null=false, default=0, command="角色充值货币"},
	{field="system_money", type="bigint(30)", null=false, default=0, command="角色系统货币"},
	{field="game_money", type="bigint(30)", null=false, default=0, command="角色游戏货币"},
	{field="game_gift", type="bigint(30)", null=false, default=0, command="角色游戏礼券"},
}

role_data.name = "role_data"
role_data.auto_increment = 4
role_data.engine = "InnoDB"
role_data.default_charset="utf8"
role_data.PriKey = role_data[1]

m.role_data = role_data

-- sys_dict

local sys_table = {
	{field="table_id", type="bigint(20) unsigned", null=false, command="字典ID", key=db.PriKey},
	{field="data", type="mediumblob", null=true, command="数据"},
	{field="save_time", type="datetime", default="2015-01-01 00:00:00", command="保存时间"}
}
sys_table.name="sys_table"
sys_table.engine = "InnoDB"
sys_table.default_charset="utf8"
sys_table.PriKey = sys_table[1]
m.sys_table = sys_table

return m





