-- Complete Example of P2P Node Usage
-- This module demonstrates how to use the P2P layer implementation

local copas = require("copas")
local P2PNode = require("src.p2p_node")

local function create_node(host, port, peer_address)
    -- Create P2P node
    local node = P2PNode.new(host, port)
    
    -- Start listening for connections
    local actual_port = node:start_listening()
    print("Node " .. node:get_did():sub(1, 8) .. " listening on " .. host .. ":" .. actual_port)
    
    -- Connect to peer if specified
    if peer_address then
        local peer_host, peer_port = peer_address:match("([^:]+):(%d+)")
        if peer_host and peer_port then
            local peer_id = node:connect_to_peer(peer_host, tonumber(peer_port))
            if peer_id then
                print("Node " .. node:get_did():sub(1, 8) .. " connected to peer " .. peer_id:sub(1, 8))
            else
                print("Node " .. node:get_did():sub(1, 8) .. " failed to connect to peer " .. peer_address)
            end
        else
            print("Node " .. node:get_did():sub(1, 8) .. " error parsing peer address: " .. peer_address)
        end
    end
    
    -- Register event handlers
    local function handle_event(event)
        print("Node " .. node:get_did():sub(1, 8) .. " received event: " .. event.action_type .. " - " .. tostring(event.payload))
    end
    
    local function handle_connection(data)
        print("Node " .. node:get_did():sub(1, 8) .. " new connection: " .. data.peer_id:sub(1, 8))
    end
    
    node:on("event", handle_event)
    node:on("connection", handle_connection)
    
    return node
end

local function main()
    -- Create two nodes on different ports
    print("Creating P2P nodes...")
    local node1 = create_node("localhost", 6001)
    
    -- Give node1 time to start
    copas.sleep(1)
    
    local node2 = create_node("localhost", 6002, "localhost:6001")
    
    -- Wait a bit for connection to establish
    copas.sleep(2)
    
    -- Create some CRDTs on node1
    print("
Creating CRDTs on node1...")
    local counter = node1:create_crdt("counter", "LWWRegister", 0)
    local items = node1:create_crdt("items", "ORSet")
    
    -- Add some items to the set
    items:add("apple")
    items:add("banana")
    local elements = items:get_elements()
    print("Node1 items: " .. table.concat(elements, ", "))
    
    -- Broadcast an event from node1
    print("
Broadcasting events from node1...")
    local event1 = {
        action_type = "add_item",
        payload = {item = "apple"}
    }
    node1:broadcast_event(event1)
    
    local event2 = {
        action_type = "increment_counter",
        payload = {amount = 1}
    }
    node1:broadcast_event(event2)
    
    -- Update the counter
    counter:write(counter:read() + 1)
    print("Node1 counter: " .. counter:read())
    
    -- Wait a bit for events to propagate
    copas.sleep(2)
    
    -- Check that node2 received the events
    print("
Node2 event log size: " .. #node2:get_event_log())
    
    -- Create CRDTs on node2
    print("
Creating CRDTs on node2...")
    local counter2 = node2:create_crdt("counter", "LWWRegister", 0)
    local items2 = node2:create_crdt("items", "ORSet")
    
    -- Add some items to node2's set
    items2:add("carrot")
    items2:add("date")
    local elements2 = items2:get_elements()
    print("Node2 items: " .. table.concat(elements2, ", "))
    
    -- Update node2's counter
    counter2:write(counter2:read() + 5)
    print("Node2 counter: " .. counter2:read())
    
    -- Simulate some time passing
    copas.sleep(2)
    
    -- Stop the nodes
    print("
Stopping nodes...")
    node1:stop()
    node2:stop()
    
    print("Example completed!")
end

-- Run the example in a copas thread
copas.addthread(main)

-- Run the copas loop
copas.loop()