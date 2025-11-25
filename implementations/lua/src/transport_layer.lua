-- Transport Layer Implementation
-- This module implements the TCP transport layer for P2P communication

local socket = require("socket")
local copas = require("copas")

local TransportLayer = {}
TransportLayer.__index = TransportLayer

function TransportLayer.new(host, port)
    local self = setmetatable({}, TransportLayer)
    self.host = host or "localhost"
    self.port = port or 0
    self.server_socket = nil
    self.client_sockets = {}  -- peer_id -> socket
    self.event_handlers = {}  -- event_type -> {handlers}
    self.node_id = nil
    self.running = false
    return self
end

-- Set the node ID
function TransportLayer:set_node_id(node_id)
    self.node_id = node_id
end

-- Start the TCP server
function TransportLayer:start_server()
    self.server_socket = socket.bind(self.host, self.port)
    
    -- Get the actual port if it was 0
    self.port = self.server_socket:getsockname()
    
    self.running = true
    
    -- Start accepting connections
    copas.addserver(self.server_socket, function(client_socket)
        copas.addthread(function()
            self:_handle_client(client_socket)
        end)
    end)
    
    return self.port
end

-- Handle a client connection
function TransportLayer:_handle_client(client_socket)
    local peer_name = client_socket:getpeername()
    local peer_id = peer_name .. ":" .. tostring(socket.gettime())
    
    -- Add to client sockets
    self.client_sockets[peer_id] = client_socket
    
    -- Emit connection event
    self:_emit_event("connection", {peer_id = peer_id})
    
    -- Listen for messages from this client
    self:_listen_to_peer(client_socket, peer_id)
end

-- Connect to a peer
function TransportLayer:connect_to_peer(host, port, peer_id)
    local client_socket = socket.connect(host, port)
    
    if not client_socket then
        print("Error connecting to peer " .. host .. ":" .. port)
        return nil
    end
    
    -- If no peer_id provided, use host:port
    if not peer_id then
        peer_id = host .. ":" .. port
    end
    
    self.client_sockets[peer_id] = client_socket
    
    -- Start listening for messages from this peer
    copas.addthread(function()
        self:_listen_to_peer(client_socket, peer_id)
    end)
    
    -- Emit connection event
    self:_emit_event("connection", {peer_id = peer_id})
    
    return peer_id
end

-- Listen for messages from a peer
function TransportLayer:_listen_to_peer(client_socket, peer_id)
    while self.running do
        local line, err = client_socket:receive("*l")
        
        if not line then
            break
        end
        
        if line ~= "" then
            local message = loadstring("return " .. line)()
            if message then
                self:_handle_message(message, peer_id)
            else
                print("Invalid message from " .. peer_id .. ": " .. line)
            end
        end
    end
    
    -- Remove client socket
    self.client_sockets[peer_id] = nil
    client_socket:close()
end

-- Handle an incoming message
function TransportLayer:_handle_message(message, peer_id)
    local msg_type = message.type
    
    if msg_type == "event" then
        -- Emit event
        self:_emit_event("event", {
            event = message.data,
            peer_id = peer_id
        })
    elseif msg_type == "event_log" then
        -- Emit event log
        self:_emit_event("event_log", {
            events = message.data,
            peer_id = peer_id
        })
    else
        -- Emit unknown message type
        self:_emit_event("unknown_message", {
            message = message,
            peer_id = peer_id
        })
    end
end

-- Send an event to a peer or all peers
function TransportLayer:send_event(event, peer_id)
    local message = {
        type = "event",
        data = event
    }
    
    self:_send_message(message, peer_id)
end

-- Send an event log to a peer or all peers
function TransportLayer:send_event_log(events, peer_id)
    local message = {
        type = "event_log",
        data = events
    }
    
    self:_send_message(message, peer_id)
end

-- Send a message to a peer or all peers
function TransportLayer:_send_message(message, peer_id)
    local message_str = serpent.dump(message) .. "
"
    
    if not peer_id then
        -- Send to all peers
        for pid, client_socket in pairs(self.client_sockets) do
            local success, err = pcall(function()
                client_socket:send(message_str)
            end)
            
            if not success then
                print("Error sending message to " .. pid .. ": " .. err)
                -- Remove dead connection
                self.client_sockets[pid] = nil
            end
        end
    else
        -- Send to specific peer
        if self.client_sockets[peer_id] then
            local success, err = pcall(function()
                self.client_sockets[peer_id]:send(message_str)
            end)
            
            if not success then
                print("Error sending message to " .. peer_id .. ": " .. err)
                -- Remove dead connection
                self.client_sockets[peer_id] = nil
            end
        else
            print("Peer " .. peer_id .. " not connected")
        end
    end
end

-- Register an event handler
function TransportLayer:on(event_type, handler)
    if not self.event_handlers[event_type] then
        self.event_handlers[event_type] = {}
    end
    table.insert(self.event_handlers[event_type], handler)
end

-- Emit an event to all registered handlers
function TransportLayer:_emit_event(event_type, data)
    if self.event_handlers[event_type] then
        for _, handler in ipairs(self.event_handlers[event_type]) do
            local success, err = pcall(function()
                handler(data)
            end)
            
            if not success then
                print("Error in " .. event_type .. " handler: " .. err)
            end
        end
    end
end

-- Stop the transport layer
function TransportLayer:stop()
    self.running = false
    
    -- Close all client sockets
    for _, client_socket in pairs(self.client_sockets) do
        pcall(function()
            client_socket:close()
        end)
    end
    self.client_sockets = {}
    
    -- Close server socket
    if self.server_socket then
        pcall(function()
            self.server_socket:close()
        end)
        self.server_socket = nil
    end
end

return TransportLayer