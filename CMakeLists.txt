cmake_minimum_required(VERSION 3.10)
project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# Set source files
set(SOURCES
    main.cpp
    # Add any additional .cpp files here as you create them
)

# Set header files
set(HEADERS
    HelloTriangle.hpp
    # Add any additional .hpp files here as you create them
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS before including vulkan.hpp
target_compile_definitions(${PROJECT_NAME} PRIVATE 
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${Vulkan_LIBRARIES}
    glfw
    ${CMAKE_DL_LIBS}  # for dlopen (required on some systems)
)

# Platform-specific settings
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME}
        pthread
        X11
        Xxf86vm
        Xrandr
        Xi
    )
endif()

# Copy shaders to build directory (for when you get to the shaders chapter)
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)

# Create shader directory in build folder
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BUILD_DIR}
)

# Function to compile shaders (you'll use this later)
function(add_shader TARGET SHADER)
    set(SHADER_SOURCE ${SHADER_DIR}/${SHADER})
    set(SHADER_OUTPUT ${SHADER_BUILD_DIR}/${SHADER}.spv)
    
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    
    add_custom_command(
        OUTPUT ${SHADER_OUTPUT}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER_SOURCE} -o ${SHADER_OUTPUT}
        DEPENDS ${SHADER_SOURCE}
        COMMENT "Compiling shader ${SHADER_NAME}"
    )
    
    set_source_files_properties(${SHADER_OUTPUT} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${SHADER_OUTPUT})
endfunction()

# Uncomment these lines when you create your shader files
# add_shader(${PROJECT_NAME} vert.vert)
# add_shader(${PROJECT_NAME} frag.frag)

# Enable validation layers in Debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_VALIDATION_LAYERS)
endif()

# Set up IDE folders (for Visual Studio, etc.)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add compile options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Print useful information
message(STATUS "Vulkan found: ${Vulkan_FOUND}")
message(STATUS "Vulkan include: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan library: ${Vulkan_LIBRARIES}")
message(STATUS "GLFW found: ${glfw3_FOUND}")
