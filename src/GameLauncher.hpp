#pragma once

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_MINIMUM_OPENCL_VERSION 100
#define CL_HPP_TARGET_OPENCL_VERSION 100
#include <CL/cl2.hpp>

namespace GPUGame
{

class GameLauncher
{
public:
	GameLauncher();
	~GameLauncher();

	void RunFrame();

private:
	cl::Context cl_context_;
	cl::CommandQueue cl_queue_;
	cl::Program cl_program_;
	cl::Buffer cl_frame_buffer_;
};

} // namespace GPUGame
