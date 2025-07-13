#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_TYPE="Debug"
EXECUTABLE_NAME="main"  # Changed from VulkanTutorial
BUILD_DIR="build"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -r|--release)
            BUILD_TYPE="Release"
            shift
            ;;
        -c|--clean)
            echo -e "${YELLOW}Cleaning build directory...${NC}"
            rm -rf "$BUILD_DIR"
            shift
            ;;
        -h|--help)
            echo "Usage: ./run.sh [options]"
            echo "Options:"
            echo "  -r, --release    Build in Release mode"
            echo "  -c, --clean      Clean build directory before building"
            echo "  -h, --help       Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            exit 1
            ;;
    esac
done

echo -e "${BLUE}=== Vulkan Tutorial Build Script ===${NC}"
echo -e "${YELLOW}Build type: ${BUILD_TYPE}${NC}"

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo -e "${YELLOW}Creating build directory...${NC}"
    mkdir "$BUILD_DIR"
fi

# Change to build directory
cd "$BUILD_DIR"

# Run CMake if needed
if [ ! -f "Makefile" ] || [ ! -f "CMakeCache.txt" ]; then
    echo -e "${YELLOW}Configuring with CMake...${NC}"
    cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
    if [ $? -ne 0 ]; then
        echo -e "${RED}CMake configuration failed!${NC}"
        exit 1
    fi
fi

# Build the project
echo -e "${YELLOW}Building project...${NC}"
make -j$(nproc)

# Check if build succeeded
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Build successful!${NC}"
    
    if [ -f "./${EXECUTABLE_NAME}" ]; then
        echo -e "${YELLOW}Running ${EXECUTABLE_NAME}...${NC}"
        echo "========================================"
        ./${EXECUTABLE_NAME}
        EXIT_CODE=$?
        echo "========================================"
        
        if [ $EXIT_CODE -eq 0 ]; then
            echo -e "${GREEN}✓ Program exited successfully${NC}"
        else
            echo -e "${RED}✗ Program exited with code: $EXIT_CODE${NC}"
        fi
    else
        echo -e "${RED}✗ Executable not found: ${EXECUTABLE_NAME}${NC}"
        exit 1
    fi
else
    echo -e "${RED}✗ Build failed!${NC}"
    exit 1
fi
