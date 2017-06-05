local m = {}



function add_event_base(enEve, roleID, content)
	local tra = m.current_tra
	if not tra then return end
	table.insert(tra, {enEve, roleID, content})
end

function add_event_obj(enEve, roleID, objID, objType, oldCnt, newCnt, objData)
	local tra = m.current_tra
	if not tra then return end
	table.insert(tra, {enEve, roleID, objID, objType, oldCnt, newCnt, objData})
end

function add_event_value(enEve, roleID, valueType, oldValue, newValue)
	local tra = m.current_tra
	if not tra then return end
	table.insert(tra, {enEve, roleID, valueType, oldValue, newValue})
end


function m.DBLog(enTra, fun, ...)
	local tra = {}
	m.current_tra = tra
	try_catch(fun, ...)
	m.current_tra = nil
	--TODO add to DB
end


return m