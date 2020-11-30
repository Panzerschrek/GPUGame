#pragma once
#include "SystemEvent.hpp"
#include <SDL_video.h>


namespace GPUGame
{

class SystemWindow final
{
public:
	SystemWindow();
	~SystemWindow();

	void BeginFrame();
	void EndFrame();

	SDL_Surface& GetWindowSurface();

	SystemEvents ProcessEvents();
	InputState GetInputState();

private:
	SDL_Window* window_= nullptr;
	SDL_Surface* window_surface_= nullptr;
};

} // namespace GPUGame
