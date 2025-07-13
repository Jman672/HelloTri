#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Change to script directory (base directory)
cd "$SCRIPT_DIR"

echo -e "${YELLOW}Building Vulkan Tutorial...${NC}"

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

# Change to build directory
cd build

# Run CMake if no Makefile exists
if [ ! -f "Makefile" ]; then
    echo -e "${YELLOW}Running CMake...${NC}"
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    if [ $? -ne 0 ]; then
        echo -e "${RED}CMake failed!${NC}"
        exit 1
    fi
fi

# Build the project
echo -e "${YELLOW}Building project...${NC}"
make -j$(nproc)

# Check if build succeeded
if [ $? -eq 0 ]; then
    echo -e "${GREEN}Build successful!${NC}"
    echo -e "${YELLOW}Running VulkanTutorial...${NC}"
    echo "----------------------------------------"
    ./VulkanTutorial
else
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi

