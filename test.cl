#include <opencl-c.h>

int fib(int x )
{
	if( x <= 1 )
		return 1;
	return fib( x - 1 ) + fib( x - 2 );
}

void inc( int* x ) { ++(*x); }

void fill_gradient( int* frame_buffer )
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

void fill_sin( global int* frame_buffer )
{
	for( int x= 0; x < 320; ++x )
	{
		frame_buffer[x]= (int)(sin(((float)x) / 10.0f) * 127.0f + 127.0f);
	}
}

kernel void entry( global int* frame_buffer )
{
	fill_gradient( frame_buffer );
	fill_fib( frame_buffer + 320 * 10 );
	fill_sin( frame_buffer + 320 * 40 );
}