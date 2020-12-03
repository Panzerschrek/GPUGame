#include "GameLauncher.hpp"
#include "Assert.hpp"
#include "Log.hpp"
#include <SDL_opengl.h>

namespace GPUGame
{

GameLauncher::GameLauncher()
	: window_width_ (320)
	, window_height_(200)
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	if (platforms.empty())
		Log::FatalError("OpenCL platforms not found.");

	cl::Platform selected_platform;
	for( const cl::Platform& platform : platforms)
		Log::Info( "Platform: ", platform.getInfo<CL_PLATFORM_VERSION>() );
	selected_platform= platforms.back();

	std::vector<cl::Device> devices;
	selected_platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

	if(devices.empty())
		Log::FatalError("No devices");
	for(const cl::Device& device : devices)
		Log::Info(
			"Device: ", device.getInfo<CL_DEVICE_NAME>(),
			" vendor: ", device.getInfo<CL_DEVICE_VENDOR>(),
			" profile: ", device.getInfo<CL_DEVICE_PROFILE>(),
			" version: ", device.getInfo<CL_DEVICE_VERSION>(),
			" platform: ", device.getInfo<CL_DEVICE_PLATFORM>() );

	devices.resize(1);
	cl::Device& device= devices.front();

	cl_context_ = cl::Context(devices);
	cl_queue_= cl::CommandQueue(cl_context_, device);

	try
	{

		#include "test.sprv.h"
		const unsigned char* program_data= c_cl_program_spirv_file_content;
		const size_t program_size= sizeof(c_cl_program_spirv_file_content);

		cl_device_id dev= device.get();

		cl_int code= 0;

		auto program_spirv= clCreateProgramWithBinary( cl_context_.get(), 1u, &dev, &program_size, &program_data, nullptr, &code );
		Log::Info( "clCreateProgramWithBinary code: ", code );

		const char* text_data= "";
		const size_t text_size= 0;
		auto program_text= clCreateProgramWithSource( cl_context_.get(), 1u, &text_data, &text_size, &code );
		Log::Info( "clCreateProgramWithSource code: ", code );

		code= clCompileProgram( program_text, 1, &dev, "-cl-std=CL2.0", 0, nullptr, nullptr, nullptr, nullptr );
		Log::Info( "clCompileProgram code: ", code );

		const cl_program programs[2]{ program_text, program_spirv };
		auto program_combined= clLinkProgram( cl_context_.get(), 1, &dev, "", 2, programs, nullptr, nullptr, &code );
		Log::Info( "clLinkProgram code: ", code );

		cl_program_.SetProgram( program_combined );

		char buff[1024]{};
		size_t s= 0;
		clGetProgramInfo( cl_program_.get(), CL_PROGRAM_BUILD_LOG, sizeof(buff), buff, &s );
		buff[s]= 0;
		Log::Info( "Build log: ", buff );

		clGetProgramInfo( cl_program_.get(), CL_PROGRAM_BUILD_STATUS, sizeof(buff), buff, &s );
		buff[s]= 0;
		Log::Info( "Build status: ", buff );
	}
	catch(const std::exception& e)
	{
		Log::Warning("Build error: ", e.what());
	}

	cl_frame_buffer_= cl::Buffer(cl_context_, CL_MEM_READ_WRITE, window_width_ * window_height_ * 4);


	// Initialize game.
	cl::Kernel func(cl_program_, "start");
	cl_queue_.enqueueNDRangeKernel(func, cl::NullRange, 1, cl::NullRange);

}

GameLauncher::~GameLauncher()
{
}

void GameLauncher::RunFrame(const float time_s)
{
	cl::Kernel func(cl_program_, "frame_step");
	func.setArg(0, cl_frame_buffer_);
	func.setArg(1, window_width_  );
	func.setArg(2, window_height_ );
	func.setArg(3, 0xFF00FF00 );
	func.setArg(4, time_s);

	cl_queue_.enqueueNDRangeKernel(func, cl::NullRange, 1, cl::NullRange);

	const auto buffer_mapped= cl_queue_.enqueueMapBuffer( cl_frame_buffer_, CL_TRUE, CL_MAP_READ, 0, window_width_ * window_height_ * 4 );
	glDrawPixels(window_width_, window_height_, GL_RGBA, GL_UNSIGNED_BYTE, buffer_mapped );
	cl_queue_.enqueueUnmapMemObject( cl_frame_buffer_, buffer_mapped );
}

} // namespace GPUGame
