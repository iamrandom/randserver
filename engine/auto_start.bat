@ECHO OFF
start randserver.exe "../luacode/?.lua" "db.db_server" 1
randserver.exe "../luacode/?.lua" "logic.logic_server" 1