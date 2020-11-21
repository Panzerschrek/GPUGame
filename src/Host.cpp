#include "Host.hpp"
#include <thread>


namespace GPUGame
{

Host::Host()
	: system_window_()
	, window_vulkan_(system_window_)
	, game_launcher_(window_vulkan_)
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

	{
		const vk::CommandBuffer command_buffer= window_vulkan_.BeginFrame();
		game_launcher_.BeginFrame(command_buffer);
	}

	window_vulkan_.EndFrame(
	{
		[&](const vk::CommandBuffer command_buffer, const vk::Image swapchain_image) { game_launcher_.EndFrame(command_buffer, swapchain_image); },
	});

	return false;
}

} // namespace GPUGame
