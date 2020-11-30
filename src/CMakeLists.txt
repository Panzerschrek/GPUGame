cmake_minimum_required(VERSION 3.10)

project(GPUGame)

# Setup language options.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

# Setup compiler options.
if(MSVC)
	add_compile_options(/utf-8 /W3 /WX)
	add_definitions(
		-DNOMINMAX
		-D_CRT_SECURE_NO_WARNINGS)
else()
	add_compile_options(-Werror -Wall -Wextra -Wconversion)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDEBUG)
endif()

# Search dependencies.
find_package(SDL2 REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")
add_executable(GPUGame ${SOURCES} ${SHADERS})
target_link_libraries(GPUGame PRIVATE ${SDL2_LIBRARIES} OpenCL)
target_include_directories(GPUGame PRIVATE ${SDL2_INCLUDE_DIRS})
target_include_directories(GPUGame PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
