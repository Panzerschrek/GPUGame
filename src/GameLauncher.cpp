#include "GameLauncher.hpp"
#include "Assert.hpp"
#include "Log.hpp"
#include <SDL_opengl.h>


namespace GPUGame
{

static const char c_program_source[]=
R"(

int fib(int x )
{
	if( x <= 1 )
		return 1;
	return fib( x - 1 ) + fib( x - 2 );
}

void inc( int* x ) { ++(*x); }

void fill_gradient( global int* frame_buffer )
{
	for( int y= 0 ; y < 200; ++y )
	for( int x= 0 ; x < 320; ++x )
	{
		frame_buffer[ x + y * 320 ]= (y << 8) | (x>>1);
	}
}

void fill_fib( global int* frame_buffer )
{
	for( int i= 0; i < 60; inc(&i) )
	{
		int x= fib(i / 4);
		frame_buffer[320 * 0 + i]= x;
		frame_buffer[320 * 1 + i]= x;
		frame_buffer[320 * 2 + i]= x;
		frame_buffer[320 * 3 + i]= x;
	}
}

void fill_sin( global int* frame_buffer, float time_s )
{
	for( int x= 0; x < 320; ++x )
	{
		frame_buffer[x]= (int)(cos(((float)x) / 10.0f + time_s) * 127.0f + 127.0f);
	}
}

kernel void entry( global int* frame_buffer, float time_s )
{
	fill_gradient( frame_buffer );
	fill_fib( frame_buffer + 320 * 10 );
	fill_sin( frame_buffer + 320 * 40, time_s );
}

)";

GameLauncher::GameLauncher()
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	if (platforms.empty())
		Log::FatalError("OpenCL platforms not found.");
	for(const cl::Platform& platform : platforms)
		Log::Info( "Platform: ", platform.getInfo<CL_PLATFORM_NAME>(), " ", platform.getInfo<CL_PLATFORM_VERSION>() );

	std::vector<cl::Device> devices;
	platforms.front().getDevices(CL_DEVICE_TYPE_GPU, &devices);

	if(devices.empty())
		Log::FatalError("No devices");
	for(const cl::Device& device : devices)
		Log::Info("Device: ", device.getInfo<CL_DEVICE_NAME>(), " ", device.getInfo<CL_DEVICE_VERSION>() );

	cl::Device& device= devices.front();

	cl_context_ = cl::Context(devices);
	cl_queue_= cl::CommandQueue(cl_context_, device);

	cl_program_= cl::Program(cl_context_, std::string(c_program_source));
	const auto build_result= cl_program_.build(devices);
	if( build_result != 0)
	{
		Log::Warning("Program build error");
		Log::Info(cl_program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device));
	}

	const size_t buffer_size= 320 * 200;
	cl_frame_buffer_= cl::Buffer(cl_context_, CL_MEM_READ_WRITE, buffer_size * 4);
}

GameLauncher::~GameLauncher()
{
}

void GameLauncher::RunFrame(const float time_s)
{
	cl::Kernel func(cl_program_, "entry");
	func.setArg(0, cl_frame_buffer_);
	func.setArg(1, time_s);

	cl_queue_.enqueueNDRangeKernel(func, cl::NullRange, 1, cl::NullRange);

	const auto buffer_mapped= cl_queue_.enqueueMapBuffer( cl_frame_buffer_, CL_TRUE, CL_MAP_READ, 0, 320 * 200 * 4 );
	glDrawPixels(320, 200, GL_RGBA, GL_UNSIGNED_BYTE, buffer_mapped );
	cl_queue_.enqueueUnmapMemObject( cl_frame_buffer_, buffer_mapped );
}

} // namespace GPUGame
