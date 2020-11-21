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

	SystemEvents ProcessEvents();

	InputState GetInputState();

	SDL_Window* GetSDLWindow() const;

private:
	SDL_Window* window_= nullptr;
};

} // namespace GPUGame
