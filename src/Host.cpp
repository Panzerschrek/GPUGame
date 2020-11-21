#include "Host.hpp"
#include <thread>


namespace GPUGame
{

Host::Host()
	: system_window_()
	, window_vulkan_(system_window_)
{
}

bool Host::Loop()
{
	const SystemEvents system_events= system_window_.ProcessEvents();
	for(const SystemEvent& system_event : system_events)
	{
		if(std::get_if<SystemEventTypes::QuitEvent>(&system_event) != nullptr)
			return true;
	}

	window_vulkan_.BeginFrame();
	window_vulkan_.EndFrame({});

	return false;
}

} // namespace GPUGame
