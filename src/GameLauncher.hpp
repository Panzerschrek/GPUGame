#pragma once

//#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_MINIMUM_OPENCL_VERSION 200
#define CL_HPP_TARGET_OPENCL_VERSION 200
//#define CL_HPP_USE_IL_KHR
#include <CL/opencl.hpp>

namespace GPUGame
{

class GameLauncher
{
public:
	GameLauncher();
	~GameLauncher();

	void RunFrame(float time_s);

private:
	class MyProgram : public cl::Program
	{
	public:
		void SetProgram(cl_program p){ object_= p; }
	};

private:
	const uint32_t window_width_;
	const uint32_t window_height_;

	cl::Context cl_context_;
	cl::CommandQueue cl_queue_;
	MyProgram cl_program_;
	cl::Buffer cl_frame_buffer_;
};

} // namespace GPUGame
