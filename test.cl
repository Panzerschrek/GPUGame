#include <stdint.h>

uint32_t fib(uint32_t x )
{
/*
	if( x <= 1u )
		return 1u;
	return fib( x - 1u ) + fib( x - 2u );
*/
	return 5u;
}

void SetBlack( uint32_t& x ){ x= 0u; }

kernel void entry( global uint32_t* frame_buffer, uint32_t width, uint32_t height, float time_s )
{
	SetBlack( frame_buffer[0] );
	for( uint32_t y= 0 ; y < height; ++y )
	for( uint32_t x= 0 ; x < width ; ++x )
		frame_buffer[ x + y * width ]= (y * 255u / height) << 8;

	for( uint32_t x= 0 ; x < width ; ++x )
	{
		const auto c= sin(float(x) / 16.0f + time_s);
		const auto r= uint32_t( (c * 0.5f + 0.5f) * 255.0f );
		for( uint32_t y= height * 3 / 8 ; y < height * 5 / 8; ++y )
		{
			frame_buffer[ x + y * width ] |= r << 16;
		}
	}

	for( uint32_t i= 0; i < 60; ++i )
	{
		uint32_t x= fib(i / 4);
		frame_buffer[width * 50 + i]= x;
		frame_buffer[width * 51 + i]= x;
		frame_buffer[width * 52 + i]= x;
		frame_buffer[width * 53 + i]= x;
	}

	uint32_t b= 66;
	SetBlack( b );
	frame_buffer[width * height / 2 + height / 2]= b;
}
