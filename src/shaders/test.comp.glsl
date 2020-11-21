#version 450

layout(binding= 0, std430) buffer frame_buffer
{
	int texels[];
};

layout(push_constant) uniform uniforms_block
{
	float time_s;
} uniforms;

void main()
{
	const int width= 320;
	const int height= 200;
	int b= int((sin(uniforms.time_s) + 1.0) * 127.0);
	for( int y= 0; y < height; ++y )
	for( int x= 0; x < width; ++x )
	{
		int r= x * 255 / width;
		int g= y * 255 / height;
		int a= 0;
		int rgba= (a<<24) | (r<<16) | (g<<8) | b;
		texels[ x + y * width ]= rgba;
	}
}
