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
		const void* const program_data= c_cl_program_spirv_file_content;
		const size_t program_size= sizeof(c_cl_program_spirv_file_content);

		auto func= reinterpret_cast<clCreateProgramWithILKHR_fn>(
			clGetExtensionFunctionAddressForPlatform(
				device.getInfo<CL_DEVICE_PLATFORM>(),
				"clCreateProgramWithILKHR") );

		if( func != nullptr )
		{
			cl_int code= 0;
			cl_program_.SetProgram( func( cl_context_.get(), program_data, program_size, &code ) );
			Log::Info( "Set program code: ", code );

			cl_device_id dev= device.get();
			code= clBuildProgram( cl_program_.get(), 1, &dev, "", nullptr, nullptr );
			Log::Info( "Build program code: ", code );

			char buff[1024]{};
			size_t s= 0;
			clGetProgramInfo( cl_program_.get(), CL_PROGRAM_BUILD_LOG, sizeof(buff), buff, &s );
			buff[s]= 0;
			Log::Info( "Build log: ", buff );

			clGetProgramInfo( cl_program_.get(), CL_PROGRAM_BUILD_STATUS, sizeof(buff), buff, &s );
			buff[s]= 0;
			Log::Info( "Build status: ", buff );
		}
	}
	catch(const std::exception& e)
	{
		Log::Warning("Build error: ", e.what());
	}

	cl_frame_buffer_= cl::Buffer(cl_context_, CL_MEM_READ_WRITE, window_width_ * window_height_ * 4);
}

GameLauncher::~GameLauncher()
{
}

void GameLauncher::RunFrame(const float time_s)
{
	cl::Kernel func(cl_program_, "entry");
	func.setArg(0, cl_frame_buffer_);
	func.setArg(1, window_width_ );
	func.setArg(2, window_height_);
	func.setArg(3, time_s);

	cl_queue_.enqueueNDRangeKernel(func, cl::NullRange, 1, cl::NullRange);

	std::vector<uint32_t> buffer(window_width_ * window_height_);
	cl_queue_.enqueueReadBuffer(cl_frame_buffer_, CL_TRUE, 0, buffer.size() * sizeof(uint32_t), buffer.data());

	glDrawPixels(window_width_, window_height_, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
	glFinish();
}

} // namespace GPUGame
