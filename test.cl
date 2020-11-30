int fib(int x )
{
/*
	if( x <= 1 )
		return 1;
	return fib( x - 1 ) + fib( x - 2 );
*/
	return 5;
}

void SetBlack( int& x ){ x= 0; }

kernel void entry( global int* frame_buffer, int width, int height, float time_s )
{
	SetBlack( frame_buffer[0] );
	for( int y= 0 ; y < height; ++y )
	for( int x= 0 ; x < width ; ++x )
		frame_buffer[ x + y * width ]= (y * 255 / height) << 8;

	for( int x= 0 ; x < width ; ++x )
	{
		float c= sin(((float)x) / 16.0f + time_s);
		int r= (int)( (c * 0.5f + 0.5f) * 255.0f );
		for( int y= height * 3 / 8 ; y < height * 5 / 8; ++y )
		{
			frame_buffer[ x + y * width ] |= r << 16;
		}
	}

	for( int i= 0; i < 60; ++i )
	{
		int x= fib(i / 4);
		frame_buffer[width * 50 + i]= x;
		frame_buffer[width * 51 + i]= x;
		frame_buffer[width * 52 + i]= x;
		frame_buffer[width * 53 + i]= x;
	}

	int b= 66;
	SetBlack( b );
	frame_buffer[width * height / 2 + height / 2]= b;
}
