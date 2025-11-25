#!/usr/bin/env lua

-- Ready-to-Run P2P Node Implementation
-- This module provides a ready-to-run P2P node implementation

local copas = require("copas")
local P2PNode = require("src.p2p_node")

-- Parse command line arguments
local args = {...}
local host = "localhost"
local port = 0
local connect = nil

for i = 1, #args do
    if args[i] == "--host" and i + 1 <= #args then
        host = args[i + 1]
        i = i + 1
    elseif args[i] == "--port" and i + 1 <= #args then
        port = tonumber(args[i + 1])
        i = i + 1
    elseif args[i] == "--connect" and i + 1 <= #args then
        connect = args[i + 1]
        i = i + 1
    end
end

-- Create P2P node
local node = P2PNode.new(host, port)

-- Start listening for connections
local actual_port = node:start_listening()
print("Node ID: " .. node:get_did())
print("Listening on " .. host .. ":" .. actual_port)

-- Connect to peer if specified
if connect then
    local connect_host, connect_port = connect:match("([^:]+):(%d+)")
    if connect_host and connect_port then
        local peer_id = node:connect_to_peer(connect_host, tonumber(connect_port))
        if peer_id then
            print("Connected to peer " .. peer_id)
        else
            print("Failed to connect to peer " .. connect)
        end
    else
        print("Invalid peer address: " .. connect)
    end
end

-- Register event handlers
local function handle_event(event)
    print("Event received: " .. tostring(event))
end

local function handle_connection(data)
    print("New connection: " .. data.peer_id)
end

node:on("event", handle_event)
node:on("connection", handle_connection)

-- Example: Create a CRDT
local counter = node:create_crdt("counter", "LWWRegister", 0)
print("Created counter CRDT with initial value: " .. counter:read())

-- Example: Create a set
local items = node:create_crdt("items", "ORSet")
items:add("item1")
items:add("item2")
local elements = items:get_elements()
print("Created items CRDT with elements: " .. table.concat(elements, ", "))

-- Keep the node running
print("Node is running. Press Ctrl+C to stop.")

-- Run the copas loop
copas.loop()