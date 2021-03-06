#include "Host.hpp"
#include "Log.hpp"
#include <thread>


namespace GPUGame
{

Host::Host()
	: system_window_()
	, window_vulkan_(system_window_)
	, game_launcher_(window_vulkan_)
	, ticks_counter_(std::chrono::seconds(1))
	, init_time_(Clock::now())
	, prev_tick_time_(init_time_)
{
}

bool Host::Loop()
{
	const Clock::time_point tick_start_time= Clock::now();
	prev_tick_time_ = tick_start_time;

	const float time_s= float((tick_start_time - init_time_).count()) * float(Clock::duration::period::num) / float(Clock::duration::period::den);

	const SystemEvents system_events= system_window_.ProcessEvents();
	for(const SystemEvent& system_event : system_events)
	{
		if(std::get_if<SystemEventTypes::QuitEvent>(&system_event) != nullptr)
			return true;
	}

	ticks_counter_.Tick();
	if( ticks_counter_.GetTotalTicks() % 60 == 0 )
		Log::Info( "FPS: ", ticks_counter_.GetTicksFrequency());

	{
		const vk::CommandBuffer command_buffer= window_vulkan_.BeginFrame();
		game_launcher_.BeginFrame(command_buffer, time_s);
	}

	window_vulkan_.EndFrame(
	{
		[&](const vk::CommandBuffer command_buffer, const vk::Image swapchain_image) { game_launcher_.EndFrame(command_buffer, swapchain_image); },
	});

	return false;
}

} // namespace GPUGame
