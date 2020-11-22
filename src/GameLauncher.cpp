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

kernel void entry( global int* frame_buffer )
{
	for( int y= 0 ; y < 200; ++y )
	for( int x= 0 ; x < 320; ++x )
		frame_buffer[ x + y * 320 ]= y << 8;

	for( int i= 0; i < 60; ++i )
	{
		int x= fib(i / 4);
		frame_buffer[320 * 10 + i]= x;
		frame_buffer[320 * 11 + i]= x;
		frame_buffer[320 * 12 + i]= x;
		frame_buffer[320 * 13 + i]= x;
	}
}

)";

GameLauncher::GameLauncher()
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	if (platforms.empty())
		Log::FatalError("OpenCL platforms not found.");

	std::vector<cl::Device> devices;
	platforms.front().getDevices(CL_DEVICE_TYPE_GPU, &devices);

	if(devices.empty())
		Log::FatalError("No devices");
	for(const cl::Device& device : devices)
		Log::Info("Device: ", device.getInfo<CL_DEVICE_NAME>());

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

void GameLauncher::RunFrame()
{
	cl::Kernel func(cl_program_, "entry");
	func.setArg(0, cl_frame_buffer_);

	cl_queue_.enqueueNDRangeKernel(func, cl::NullRange, 1, cl::NullRange);

	std::vector<uint32_t> buffer(320 * 200);
	cl_queue_.enqueueReadBuffer(cl_frame_buffer_, CL_TRUE, 0, buffer.size() * sizeof(uint32_t), buffer.data());


	glDrawPixels(320, 200, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
}

} // namespace GPUGame
