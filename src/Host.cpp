#include "Host.hpp"
#include "Log.hpp"
#include <thread>


namespace GPUGame
{

Host::Host()
	: game_launcher_()
	, system_window_()
	, window_opengl_(system_window_)
	, ticks_counter_(std::chrono::seconds(1))
	, init_time_(Clock::now())
	, prev_tick_time_(init_time_)
{
}

bool Host::Loop()
{
	const Clock::time_point tick_start_time= Clock::now();
	const auto dt= tick_start_time - prev_tick_time_;
	prev_tick_time_ = tick_start_time;

	const float time_s= float((tick_start_time - init_time_).count()) * float(Clock::duration::period::num) / float(Clock::duration::period::den);

	const float dt_s= float(dt.count()) * float(Clock::duration::period::num) / float(Clock::duration::period::den);

	const SystemEvents system_events= system_window_.ProcessEvents();
	for(const SystemEvent& system_event : system_events)
	{
		if(std::get_if<SystemEventTypes::QuitEvent>(&system_event) != nullptr)
			return true;
	}

	ticks_counter_.Tick();
	if( ticks_counter_.GetTotalTicks() % 60 == 0 )
		Log::Info( "FPS: ", ticks_counter_.GetTicksFrequency());

	window_opengl_.BeginFrame();
	game_launcher_.RunFrame(system_window_.GetInputState(), time_s);
	window_opengl_.EndFrame();

	const Clock::time_point tick_end_time= Clock::now();
	const auto frame_dt= tick_end_time - tick_start_time;

	const float max_fps= 120.0f;

	const std::chrono::milliseconds min_frame_duration(uint32_t(1000.0f / max_fps));
	if(frame_dt <= min_frame_duration)
	{
		std::this_thread::sleep_for(min_frame_duration - frame_dt);
	}

	return false;
}

} // namespace GPUGame
