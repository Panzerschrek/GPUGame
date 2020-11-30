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

global uint32_t g_dst_pixel= 0;

kernel void entry(
	global uint32_t* const frame_buffer,
	const uint32_t width,
	const uint32_t height,
	const uint32_t fill_color,
	const float time_s )
{
	for( uint32_t y= 0 ; y < height; ++y )
	for( uint32_t x= 0 ; x < width ; ++x )
		frame_buffer[ x + y * width ]= ( (y * 255u / height) << 8 ) | ( x * 255u / width );

	for( uint32_t x= 0 ; x < width ; ++x )
	{
		const auto s= sin(float(x) / 16.0f + time_s);
		const auto r= uint32_t( (s * 0.5f + 0.5f) * 255.0f );
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

	constexpr uint32_t c_local_array_size= 16384;
	uint32_t my_local_array[c_local_array_size];
	for( uint32_t i= 0; i < c_local_array_size; ++i )
		my_local_array[i]= i * i + g_dst_pixel;

	for( uint32_t i= 0; i < c_local_array_size; ++i )
		frame_buffer[ width * height / 2 + i ]= my_local_array[c_local_array_size - 1 - i];

	frame_buffer[ g_dst_pixel ]= fill_color;
	++g_dst_pixel;

	uint32_t b= 66;
	SetBlack( b );
	frame_buffer[width * height / 2 + width / 2]= b;
}
