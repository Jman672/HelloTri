cmake_minimum_required(VERSION 3.10)
project(VulkanTutorial)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# Find slangc executable
find_program(SLANGC_EXECUTABLE 
    NAMES slangc
    HINTS 
        $ENV{SLANG_DIR}/bin
        /usr/local/bin
        /usr/bin
)

if(NOT SLANGC_EXECUTABLE)
    message(FATAL_ERROR "slangc not found. Please install Slang or set SLANG_DIR environment variable.")
endif()

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

# Create executable with name 'main'
add_executable(main ${SOURCES} ${HEADERS})

# Define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS before including vulkan.hpp
target_compile_definitions(main PRIVATE 
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
)

# Include directories
target_include_directories(main PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(main
    ${Vulkan_LIBRARIES}
    glfw
    ${CMAKE_DL_LIBS}  # for dlopen (required on some systems)
)

# Platform-specific settings
if(UNIX AND NOT APPLE)
    target_link_libraries(main
        pthread
        X11
        Xxf86vm
        Xrandr
        Xi
    )
endif()

# Shader directories
set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)

# Create shader directory in build folder
add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_BUILD_DIR}
)

# Function to compile Slang shaders
function(add_slang_shader_target TARGET)
    cmake_parse_arguments("SHADER" "" "" "SOURCES" ${ARGN})
    
    set(SHADERS_DIR ${CMAKE_SOURCE_DIR}/shaders)
    set(ENTRY_POINTS -entry vertMain -entry fragMain)
    
    # Create full paths for source files
    set(SHADER_SOURCE_PATHS)
    foreach(SOURCE ${SHADER_SOURCES})
        list(APPEND SHADER_SOURCE_PATHS ${SHADERS_DIR}/${SOURCE})
    endforeach()
    
    # Ensure shaders directory exists
    add_custom_command(
        OUTPUT ${SHADERS_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR}
    )
    
    # Compile shaders
    add_custom_command(
        OUTPUT  ${SHADERS_DIR}/slang.spv
        COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SOURCES} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_POINTS} -o slang.spv
        WORKING_DIRECTORY ${SHADERS_DIR}
        DEPENDS ${SHADER_SOURCE_PATHS}
        COMMENT "Compiling Slang Shaders"
        VERBATIM
    )
    
    add_custom_target(${TARGET} ALL DEPENDS ${SHADERS_DIR}/slang.spv)
endfunction()

# Function to compile GLSL shaders (keeping this for regular shaders)
function(add_shader TARGET SHADER)
    set(SHADER_SOURCE ${SHADER_DIR}/${SHADER})
    set(SHADER_OUTPUT ${SHADER_BUILD_DIR}/${SHADER}.spv)
    
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    
    add_custom_command(
        OUTPUT ${SHADER_OUTPUT}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER_SOURCE} -o ${SHADER_OUTPUT}
        DEPENDS ${SHADER_SOURCE}
        COMMENT "Compiling GLSL shader ${SHADER_NAME}"
    )
    
    set_source_files_properties(${SHADER_OUTPUT} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${SHADER_OUTPUT})
endfunction()

# Compile Slang shaders
add_slang_shader_target(SlangShaders SOURCES shader.slang)
add_dependencies(main SlangShaders)

# Enable validation layers in Debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(main PRIVATE ENABLE_VALIDATION_LAYERS)
endif()

# Set up IDE folders (for Visual Studio, etc.)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add compile options
if(MSVC)
    target_compile_options(main PRIVATE /W4)
else()
    target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Print useful information
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Vulkan found: ${Vulkan_FOUND}")
message(STATUS "Vulkan include: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan library: ${Vulkan_LIBRARIES}")
message(STATUS "GLFW found: ${glfw3_FOUND}")
message(STATUS "Slang compiler: ${SLANGC_EXECUTABLE}")
