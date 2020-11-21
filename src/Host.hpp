#pragma once
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

	bool quit_requested_= false;
};

} // namespace GPUGame
