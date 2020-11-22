#pragma once
#include "SystemWindow.hpp"
#include <functional>


namespace GPUGame
{

class WindowOpenGL final
{
public:

public:
	explicit WindowOpenGL(const SystemWindow& system_window);
	~WindowOpenGL();

	void BeginFrame();
	void EndFrame();

private:
	SDL_Window* const sdl_window_;
	SDL_GLContext gl_context_= nullptr;
};

} // namespace GPUGame
