#version 450

layout(binding= 0, std430) buffer frame_buffer
{
	int texels[];
};

void main()
{
	const int width= 640;
	const int height= 480;
	for( int y= 0; y < height; ++y )
	for( int x= 0; x < width; ++x )
	{
		int r= x * 255 / width;
		int g= y * 255 / height;
		int b= 0;
		int a= 0;
		int rgba= (a<<24) | (r<<16) | (g<<8) | b;
		texels[ x + y * width ]= rgba;
	}
}
