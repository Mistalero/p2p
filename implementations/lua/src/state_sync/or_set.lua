-- OR-Set (Observed-Remove Set) Implementation
-- This module implements an OR-Set CRDT

local socket = require("socket")
local CRDT = require("src.state_sync.crdt")

local ORSet = {}
ORSet.__index = ORSet
setmetatable(ORSet, CRDT)

function ORSet.new(id, node_id)
    local self = CRDT.new(id, node_id)
    setmetatable(self, ORSet)
    -- Store elements with their tags
    self.elements = {}  -- element -> set of tags
    -- Store tags for lookup
    self.tags = {}  -- tag -> element
    return self
end

-- Add an element to the set
function ORSet:add(element, node_id)
    node_id = node_id or self.node_id
    local timestamp = socket.gettime() * 1000
    local tag = element .. ":" .. node_id .. ":" .. tostring(timestamp)
    
    if not self.elements[element] then
        self.elements[element] = {}
    end
    
    self.elements[element][tag] = true
    self.tags[tag] = element
end

-- Remove an element from the set
function ORSet:remove(element)
    if self.elements[element] then
        -- Remove all tags for this element
        for tag, _ in pairs(self.elements[element]) do
            self.tags[tag] = nil
        end
        self.elements[element] = nil
    end
end

-- Check if an element is in the set
function ORSet:contains(element)
    return self.elements[element] ~= nil
end

-- Get all elements in the set
function ORSet:get_elements()
    local elements = {}
    for element, _ in pairs(self.elements) do
        table.insert(elements, element)
    end
    return elements
end

-- Apply an operation to the set
function ORSet:apply_operation(operation)
    if operation.type == "add" then
        local tag = operation.tag
        local element = operation.element
        
        if not self.elements[element] then
            self.elements[element] = {}
        end
        
        self.elements[element][tag] = true
        self.tags[tag] = element
        
    elseif operation.type == "remove" then
        local element = operation.element
        if self.elements[element] then
            -- Remove all tags for this element
            for tag, _ in pairs(self.elements[element]) do
                self.tags[tag] = nil
            end
            self.elements[element] = nil
        end
    end
end

-- Merge with another OR-Set
function ORSet:merge(other)
    -- Add all tags from the other set
    for element, tags in pairs(other.elements) do
        if not self.elements[element] then
            self.elements[element] = {}
        end
        for tag, _ in pairs(tags) do
            self.elements[element][tag] = true
        end
    end
    
    -- Update tag lookup
    for tag, element in pairs(other.tags) do
        self.tags[tag] = element
    end
    
    -- Remove tags that are no longer present
    local current_tags = {}
    for element, tags in pairs(self.elements) do
        for tag, _ in pairs(tags) do
            current_tags[tag] = true
        end
    end
    
    -- Remove tags that are no longer present
    for tag, _ in pairs(self.tags) do
        if not current_tags[tag] then
            self.tags[tag] = nil
        end
    end
    
    -- Remove empty element entries
    for element, tags in pairs(self.elements) do
        local empty = true
        for _, _ in pairs(tags) do
            empty = false
            break
        end
        if empty then
            self.elements[element] = nil
        end
    end
end

-- Serialize the OR-Set to a table
function ORSet:to_table()
    -- Convert sets to tables for serialization
    local elements_table = {}
    for element, tags in pairs(self.elements) do
        local tags_table = {}
        for tag, _ in pairs(tags) do
            table.insert(tags_table, tag)
        end
        elements_table[element] = tags_table
    end
    
    return {
        type = "ORSet",
        id = self.id,
        elements = elements_table
    }
end

-- Deserialize the OR-Set from a table
function ORSet.from_table(table_data)
    local or_set = ORSet.new(table_data.id, "")  -- node_id will be set by operations
    
    -- Convert tables back to sets
    for element, tags in pairs(table_data.elements) do
        or_set.elements[element] = {}
        for _, tag in ipairs(tags) do
            or_set.elements[element][tag] = true
            or_set.tags[tag] = element
        end
    end
    
    return or_set
end

return ORSet