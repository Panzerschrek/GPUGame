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
set(SDL2_DIR "C:/Users/user/Documents/Projects/other/SDL2-2.0.10-vc")
set(SDL2_INCLUDE_DIRS ${SDL2_DIR}/include)
set(SDL2_LIBRARIES ${SDL2_DIR}/lib/x64/SDL2.lib ${SDL2_DIR}/lib/x64/SDL2main.lib)
find_program(GLSLANGVALIDATOR glslangValidator)
if(NOT GLSLANGVALIDATOR)
	message(FATAL_ERROR "glslangValidator not found")
endif()

# Compile shaders.
file(GLOB SHADERS "shaders/*")
foreach(SHADER_FILE ${SHADERS})
	file(RELATIVE_PATH OUT_FILE ${CMAKE_CURRENT_SOURCE_DIR} ${SHADER_FILE})
	set(OUT_FILE_BASE ${CMAKE_CURRENT_BINARY_DIR}/${OUT_FILE})
	string(REPLACE ".glsl" ".h" OUT_FILE_H ${OUT_FILE_BASE})
	get_filename_component(VARIABLE_NAME ${SHADER_FILE} NAME)
	string(REPLACE ".glsl" "" VARIABLE_NAME ${VARIABLE_NAME})
	string(REPLACE "." "_" VARIABLE_NAME ${VARIABLE_NAME})
	add_custom_command(
		OUTPUT ${OUT_FILE_H}
		DEPENDS ${SHADER_FILE}
		COMMAND ${GLSLANGVALIDATOR} -V ${SHADER_FILE} --vn ${VARIABLE_NAME} -o ${OUT_FILE_H}
		)

	list(APPEND SHADERS_COMPILED ${OUT_FILE_H})
endforeach()

include_directories(../../OpenCL-CLHPP/include)
include_directories(../../OpenCL-Headers/)
set(OPENCL_LIB "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.1/lib/x64/OpenCL.lib")

file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")
add_executable(GPUGame WIN32 ${SOURCES} ${SHADERS} ${SHADERS_COMPILED})
target_link_libraries(GPUGame PRIVATE ${SDL2_LIBRARIES} opengl32 ${OPENCL_LIB})
target_include_directories(GPUGame PRIVATE ${SDL2_INCLUDE_DIRS})
target_include_directories(GPUGame PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
