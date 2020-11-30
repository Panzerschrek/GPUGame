#pragma once
#include "SystemWindow.hpp"
#include <CL/cl.hpp>

namespace GPUGame
{

class GameLauncher
{
public:
	explicit GameLauncher( SystemWindow& system_window );
	~GameLauncher();

	void RunFrame(float time_s);

private:
	const SDL_Surface out_surface_;

	cl::Context cl_context_;
	cl::CommandQueue cl_queue_;
	cl::Program cl_program_;
	cl::Buffer cl_frame_buffer_;
};

} // namespace GPUGame
