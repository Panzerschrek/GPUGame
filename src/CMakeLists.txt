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

set(CL_CLANG ${CMAKE_CURRENT_SOURCE_DIR}/../../U-00DC-Sprache/build-release-msvc-ninja/llvm/bin/clang)
set(CL_CLANG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../U-00DC-Sprache/build-release-msvc-ninja/llvm/lib/clang/9.0.0/include/)
set(LLVM_SPIRV ${CMAKE_CURRENT_SOURCE_DIR}/../../SPIRV-LLVM-Translator/build-debug/tools/llvm-spirv/llvm-spirv)

set(CL_PROGRAM ${CMAKE_CURRENT_SOURCE_DIR}/test.cl)
set(CL_PROGRAM_BC ${CMAKE_CURRENT_BINARY_DIR}/test.bc)
set(CL_PROGRAM_SPRV ${CMAKE_CURRENT_BINARY_DIR}/test.sprv)
set(CL_PROGRAM_H ${CMAKE_CURRENT_BINARY_DIR}/test.sprv.h)
add_custom_command(
	OUTPUT ${CL_PROGRAM_BC}
	DEPENDS ${CL_PROGRAM}
	COMMAND ${CL_CLANG} -cc1 -triple spir ${CL_PROGRAM} -O2 -emit-llvm-bc -o ${CL_PROGRAM_BC} -cl-std=clc++ -finclude-default-header -I${CL_CLANG_INCLUDE_DIR}
	)
add_custom_command(
	OUTPUT ${CL_PROGRAM_SPRV}
	DEPENDS ${CL_PROGRAM_BC}
	COMMAND ${LLVM_SPIRV} ${CL_PROGRAM_BC} -o ${CL_PROGRAM_SPRV}
	)
add_custom_command(
	OUTPUT ${CL_PROGRAM_H}
	DEPENDS ${CL_PROGRAM_SPRV}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/bin2c.cmake
	COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/bin2c.cmake ${CL_PROGRAM_SPRV} ${CL_PROGRAM_H} c_cl_program_spirv_file_content
	)

include_directories(../../OpenCL-CLHPP/include)
include_directories(../../OpenCL-Headers/)
set(OPENCL_LIB "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.1/lib/x64/OpenCL.lib")

file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")
add_executable(GPUGame WIN32 ${SOURCES} ${SHADERS} ${CL_PROGRAM} ${CL_PROGRAM_H})
target_link_libraries(GPUGame PRIVATE ${SDL2_LIBRARIES} opengl32 ${OPENCL_LIB})
target_include_directories(GPUGame PRIVATE ${SDL2_INCLUDE_DIRS})
target_include_directories(GPUGame PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
