#pragma once

#include <CL/cl.hpp>

namespace GPUGame
{

class GameLauncher
{
public:
	GameLauncher();
	~GameLauncher();

	void RunFrame(float time_s);

private:
	cl::Context cl_context_;
	cl::CommandQueue cl_queue_;
	cl::Program cl_program_;
	cl::Buffer cl_frame_buffer_;
};

} // namespace GPUGame
