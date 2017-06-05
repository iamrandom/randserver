
local m = {}
m.__index = m

m.sessions = {}

function m.reg_session(fd, session)
	session.fd = fd
	m.sessions[fd] = session
end

function m.del_session(fd)
	m.sessions[fd] = nil
end

local function do_round()
	
	local fd, events, param = net.queue()

	if not fd or fd <= 0 then return false end

	if events & net.Eve_Post_Listen_Error > 0 then
		error("post listen error happend")
	end

	local session = m.sessions[fd]
	if not session then
		warn ("no session", fd)
		return false
	end

	if events & net.Eve_Accept > 0 then
		if (events & net.Eve_Error) > 0 then
			print ("GE_EXC, events & net.Eve_Error", fd)
			return false
		end

		while true do
			local newfd = net.accept(fd)
			if newfd <= 0 then
				break 
			end
            local handle_accept = session.handle_accept
			handle_accept(session, newfd)
		end
		return true
	end

	if events & net.Eve_Connect > 0 then
		session.handle_connect(session, events & net.Eve_Error)
		return true
	end

	if events & net.Eve_Error > 0 then
		session.handle_error(session, events)
		return true
	end

	if events & net.Eve_Read > 0 then
		while true do
			local msg_size, msg = net.socket_read(fd, ser)
			if not msg_size or msg_size == 0 then break end
			if msg_size < 0 then
				session.handle_error(session, events)
				return true
			end
			if true ~= try_catch(session.handle_msg, session, msg_size, msg) then
                net.free(msg)
            end            
		end
	end
	return true
end

function m.net_round()
	try_catch(do_round)
end


function m.reg_who(msg_type, who)
    local old_who = m[msg_type]
    if old_who then
        print("WARN reg_who msg_type", msg_type, " exist")
    end
    m[msg_type] = who
end

function m.handle_error(session, events)
    if session then
        net.socket_close(session.fd)
		m.del_session(session.fd)
    end
end

function m.handle_msg(session, msg_size, msg)

    local msg_type = service:unpack(msg, msg_size, 1)
    if msg_type then
        local who = m[msg_type]
        if who then
            setmetatable(session, nil)
            if type(who) == "table" then
                setmetatable(session, who)
            else
                try_catch(who, session, service:unpack(msg, msg_size, -1, 1))
            end
            return
        end
    end
    warn("listen handle_msg have no handle to command msg type ", msg_type)
    session.handle_error(session, net.Eve_Error)
end

function m.handle_accept(lsession, newfd)
    local session = {}
    session.type = "accept"
    session.listen_fd = lsession.fd
    setmetatable(session, m)
    m.reg_session(newfd, session)
    net.socket_cfg(newfd, lsession.enByte, lsession.read_msg_cnt, lsession.write_buff_cnt);
    ip, port = net.ip_port(newfd)
    session.ip = ip
    session.port = port
    return session
end

function m.send_msg(session, ...)
    local msg, msg_size = service:pack(...)
    if msg then
        local send_size = net.socket_write(session.fd, msg, msg_size)
    end
end

function m.net_listen(ip, port, cnt, enByte, read_msg_cnt, write_buff_cnt)
    local listen_fd = net.listen(ip, port, cnt)
    if not listen_fd or listen_fd == 0 then
        return nil
    end
    local lsession = {}
    lsession.type = "listen"
    lsession.ip = ip
    lsession.port = port
    lsession.cnt = cnt
    lsession.enByte = enByte
    lsession.read_msg_cnt = read_msg_cnt
    lsession.write_buff_cnt = write_buff_cnt
    setmetatable(lsession, m)
    m.reg_session(listen_fd, lsession)
    
    return lsession
end

return m




