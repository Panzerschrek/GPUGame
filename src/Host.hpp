#pragma once
#include "GameLauncher.hpp"
#include "SystemWindow.hpp"
#include "TicksCounter.hpp"
#include "WindowOpenGL.hpp"
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
	using Clock= std::chrono::steady_clock;

private:
	SystemWindow system_window_;
	WindowOpenGL window_opengl_;
	GameLauncher game_launcher_;

	TicksCounter ticks_counter_;

	const Clock::time_point init_time_;
	Clock::time_point prev_tick_time_;

	bool quit_requested_= false;
};

} // namespace GPUGame
