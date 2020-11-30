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
/*
	if( x <= 1 )
		return 1;
	return fib( x - 1 ) + fib( x - 2 );
*/
	return 5;
}

void SetBlack( int* x ){ *x= 0; }

kernel void entry( global int* frame_buffer, int width, int height, float time_s )
{
	SetBlack( frame_buffer );
	for( int y= 0 ; y < height; ++y )
	for( int x= 0 ; x < width ; ++x )
		frame_buffer[ x + y * width ]= (y * 255 / height) << 8;

	for( int x= 0 ; x < width ; ++x )
	{
		float c= sin(((float)x) / 16.0f + time_s);
		int r= (int)( (c * 0.5f + 0.5f) * 255.0f );
		for( int y= height * 3 / 8 ; y < height * 5 / 8; ++y )
		{
			frame_buffer[ x + y * width ] |= r;
		}
	}

	int b= 66;
	SetBlack( &b );
	frame_buffer[width * height / 2 + height / 2]= b;
}

)";

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
			" profile: ", device.getInfo<CL_DEVICE_PROFILE>() );

	devices.resize(1);
	cl::Device& device= devices.front();

	cl_context_ = cl::Context(devices);
	cl_queue_= cl::CommandQueue(cl_context_, device);

	try
	{
		cl_program_= cl::Program(cl_context_, std::string(c_program_source));
		if( cl_program_.build(devices, "-cl-std=CL2.0") != 0 )
		{
			Log::Warning( "Program build failed: ", cl_program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) );
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
