#pragma once
#include "GameLauncher.hpp"
#include "SystemWindow.hpp"
#include "WindowVulkan.hpp"
#include <chrono>


namespace GPUGame
{

class Host final
{
public:
	Host();

	// Returns false on quit
	bool Loop();

private:
	SystemWindow system_window_;
	WindowVulkan window_vulkan_;
	GameLauncher game_launcher_;

	bool quit_requested_= false;
};

} // namespace GPUGame
