#!/bin/bash

# Script to run the artifact system test

# Exit on any error
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Artifact System Test ===${NC}"

# Create artifacts directory if it doesn't exist
mkdir -p ./artifacts

# Build the test artifact creator
echo -e "${YELLOW}Building test artifact creator...${NC}"
gcc -I./include -o ./tools/create_test_artifact ./tools/create_test_artifact.c ./core/artifact_manager.c ./core/artifact_storage.c

# Create test artifact
echo -e "${YELLOW}Creating test artifact...${NC}"
./tools/create_test_artifact

# Build the example
echo -e "${YELLOW}Building example...${NC}"
make

# Start artifact server in background
echo -e "${YELLOW}Starting artifact server...${NC}"
python3 ./tools/artifact_server.py &

# Store the server PID
SERVER_PID=$!

# Give the server a moment to start
sleep 2

# Run the example
echo -e "${YELLOW}Running example...${NC}"
./bin/artifact-example

# Stop the server
echo -e "${YELLOW}Stopping artifact server...${NC}"
kill $SERVER_PID

echo -e "${GREEN}=== Test completed ===${NC}"