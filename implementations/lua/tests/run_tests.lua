-- Script to run all tests

package.path = package.path .. ";../src/?.lua"

print("Running all tests...")

-- Run identity layer tests
print("
--- Running Identity Layer Tests ---")
dofile("test_identity_layer.lua")

-- Run LWW-Register tests
print("
--- Running LWW-Register Tests ---")
dofile("test_lww_register.lua")

-- Run OR-Set tests
print("
--- Running OR-Set Tests ---")
dofile("test_or_set.lua")

-- Run State Synchronization Layer tests
print("
--- Running State Synchronization Layer Tests ---")
dofile("test_state_sync_layer.lua")

-- Run P2P Node tests
print("
--- Running P2P Node Tests ---")
dofile("test_p2p_node.lua")

print("
All tests completed successfully!")