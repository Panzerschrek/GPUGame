#include "GameLauncher.hpp"
#include "Assert.hpp"
#include "Log.hpp"
#include <SDL_opengl.h>
#include <cstdio>

namespace GPUGame
{

GameLauncher::GameLauncher()
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
		FILE* const f= std::fopen("test.sprv", "rb");

		std::fseek( f, 0, SEEK_END );
		const auto file_size= std::ftell( f );
		std::fseek( f, 0, SEEK_SET );

		std::vector<char> binary_data;
		binary_data.resize( file_size );
		std::fread( binary_data.data(), 1, binary_data.size(), f );

		std::fclose( f );

		auto func= reinterpret_cast<clCreateProgramWithILKHR_fn>(
			clGetExtensionFunctionAddressForPlatform(
				device.getInfo<CL_DEVICE_PLATFORM>(),
				"clCreateProgramWithILKHR") );

		if( func != nullptr )
		{
			cl_int code= 0;
			cl_program_.SetProgram( func( cl_context_.get(), binary_data.data(), binary_data.size(), &code ) );
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

	const size_t buffer_size= 320 * 200;
	cl_frame_buffer_= cl::Buffer(cl_context_, CL_MEM_READ_WRITE, buffer_size * 4);
}

GameLauncher::~GameLauncher()
{

}

void GameLauncher::RunFrame(const float time_s)
{
	const int width= 320;
	const int height= 200;

	cl::Kernel func(cl_program_, "entry");
	func.setArg(0, cl_frame_buffer_);
	func.setArg(1, width);
	func.setArg(2, height);
	func.setArg(3, time_s);

	cl_queue_.enqueueNDRangeKernel(func, cl::NullRange, 1, cl::NullRange);

	std::vector<uint32_t> buffer(width * height);
	cl_queue_.enqueueReadBuffer(cl_frame_buffer_, CL_TRUE, 0, buffer.size() * sizeof(uint32_t), buffer.data());


	glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
	glFinish();
}

} // namespace GPUGame
