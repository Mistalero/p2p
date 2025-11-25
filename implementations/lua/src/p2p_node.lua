-- P2P Node Implementation
-- This module implements a complete P2P node with all layers integrated

local socket = require("socket")
local IdentityLayer = require("src.identity_layer")
local StateSyncLayer = require("src.state_sync_layer")
local TransportLayer = require("src.transport_layer")

local P2PNode = {}
P2PNode.__index = P2PNode

function P2PNode.new(host, port)
    local self = setmetatable({}, P2PNode)
    
    -- Initialize layers
    self.identity_layer = IdentityLayer.new()
    self.state_sync_layer = StateSyncLayer.new()
    self.transport_layer = TransportLayer.new(host, port)
    
    -- Generate keys and create DID
    self.identity_layer:generate_nostr_keys()
    self.did_document = self.identity_layer:create_did_document()
    self.node_id = self.did_document.id
    
    -- Set node ID in transport layer
    self.transport_layer:set_node_id(self.node_id)
    
    -- Event log
    self.event_log = {}
    
    -- Register transport event handlers
    self.transport_layer:on("connection", function(data)
        self:_on_connection(data)
    end)
    
    self.transport_layer:on("event", function(data)
        self:_on_event(data)
    end)
    
    self.transport_layer:on("event_log", function(data)
        self:_on_event_log(data)
    end)
    
    -- Custom event handlers
    self.event_handlers = {}
    
    return self
end

-- Initialize the P2P node
function P2PNode:initialize()
    -- This method could be used for any additional initialization
end

-- Start listening for incoming connections
function P2PNode:start_listening()
    local port = self.transport_layer:start_server()
    print("P2P node listening on port " .. port)
    return port
end

-- Connect to a peer
function P2PNode:connect_to_peer(host, port)
    return self.transport_layer:connect_to_peer(host, port)
end

-- Add an event to the local log
function P2PNode:add_event(event)
    -- Add metadata if not present
    if not event.timestamp then
        event.timestamp = socket.gettime() * 1000
    end
    if not event.author then
        event.author = self.node_id
    end
    
    table.insert(self.event_log, event)
    return event
end

-- Broadcast an event to all connected peers
function P2PNode:broadcast_event(event)
    -- Add to local log first
    self:add_event(event)
    
    -- Broadcast to peers
    self.transport_layer:send_event(event)
end

-- Get the current event log
function P2PNode:get_event_log()
    -- Return a copy of the event log
    local copy = {}
    for i, event in ipairs(self.event_log) do
        copy[i] = event
    end
    return copy
end

-- Merge another event log with the local log
function P2PNode:merge_event_log(other_log)
    for _, event in ipairs(other_log) do
        -- Check if event already exists
        local exists = false
        for _, e in ipairs(self.event_log) do
            if e.author == event.author and 
               e.action_type == event.action_type and
               e.timestamp == event.timestamp then
                exists = true
                break
            end
        end
        
        if not exists then
            table.insert(self.event_log, event)
        end
    end
end

-- Get the node's public key
function P2PNode:get_public_key()
    return self.identity_layer.nostr_public_key
end

-- Get the node's DID
function P2PNode:get_did()
    return self.node_id
end

-- Get the node's DID document
function P2PNode:get_did_document()
    return self.did_document
end

-- Resolve a DID
function P2PNode:resolve_did(did)
    return self.identity_layer:resolve_did(did)
end

-- Create a new CRDT
function P2PNode:create_crdt(id, type, initial_value)
    return self.state_sync_layer:create_crdt(id, type, initial_value)
end

-- Get a CRDT by ID
function P2PNode:get_crdt(id)
    return self.state_sync_layer:get_crdt(id)
end

-- Update a CRDT
function P2PNode:update_crdt(id, operation)
    return self.state_sync_layer:update_crdt(id, operation)
end

-- Merge with another CRDT
function P2PNode:merge_crdt(id, other_crdt)
    return self.state_sync_layer:merge_crdt(id, other_crdt)
end

-- Serialize all CRDTs to a table
function P2PNode:serialize_state()
    return self.state_sync_layer:serialize_state()
end

-- Deserialize state from a table
function P2PNode:deserialize_state(state)
    self.state_sync_layer:deserialize_state(state)
end

-- Register an event handler
function P2PNode:on(event_type, handler)
    if not self.event_handlers[event_type] then
        self.event_handlers[event_type] = {}
    end
    table.insert(self.event_handlers[event_type], handler)
end

-- Handle incoming connection
function P2PNode:_on_connection(data)
    print("New connection from " .. data.peer_id)
    
    -- Send our event log to the new peer
    self.transport_layer:send_event_log(self.event_log, data.peer_id)
    
    -- Emit connection event to custom handlers
    if self.event_handlers.connection then
        for _, handler in ipairs(self.event_handlers.connection) do
            local success, err = pcall(function()
                handler(data)
            end)
            
            if not success then
                print("Error in connection handler: " .. err)
            end
        end
    end
end

-- Handle incoming event
function P2PNode:_on_event(data)
    local event = data.event
    local peer_id = data.peer_id
    
    print("Received event from " .. peer_id .. ": " .. tostring(event))
    
    -- Add to local log if not duplicate
    local exists = false
    for _, e in ipairs(self.event_log) do
        if e.author == event.author and 
           e.action_type == event.action_type and
           e.timestamp == event.timestamp then
            exists = true
            break
        end
    end
    
    if not exists then
        table.insert(self.event_log, event)
    end
    
    -- Emit event to custom handlers
    if self.event_handlers.event then
        for _, handler in ipairs(self.event_handlers.event) do
            local success, err = pcall(function()
                handler(event)
            end)
            
            if not success then
                print("Error in event handler: " .. err)
            end
        end
    end
end

-- Handle incoming event log
function P2PNode:_on_event_log(data)
    local events = data.events
    local peer_id = data.peer_id
    
    print("Received event log from " .. peer_id .. " with " .. #events .. " events")
    
    -- Merge with local log
    self:merge_event_log(events)
end

-- Stop the P2P node
function P2PNode:stop()
    self.transport_layer:stop()
end

return P2PNode