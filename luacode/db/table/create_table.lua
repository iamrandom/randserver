local m = {}


function m.run(db_info)
	local db = require("common.db")
	local log = require("common.log")

	local tables = require("db.table.zone_table")

	local dbname = db_info.db
	local luamysql = luasql.mysql()
	local con, err = luamysql:connect("", db_info.user, db_info.password, db_info.ip, db_info.port)
	con:execute("alter character set utf-8")
	con:execute("create database if not exists ".. dbname .. " DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci")
	con:execute("use ".. dbname)

	for k, v in pairs(tables) do
		db.create_or_update_table(con, k, v)
	end
	con:close()
end


return m