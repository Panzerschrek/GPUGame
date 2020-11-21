#version 450

layout(binding= 0, std430) buffer frame_buffer
{
	int texels[];
};

layout(push_constant) uniform UniformsBlock
{
	int viewport_size[4];
	float time_s;
} uniforms;

void main()
{
	int b= int((sin(uniforms.time_s) + 1.0) * 127.0);
	for( int y= 0; y < uniforms.viewport_size[1]; ++y )
	for( int x= 0; x < uniforms.viewport_size[0]; ++x )
	{
		int r= x * 255 / uniforms.viewport_size[0];
		int g= y * 255 / uniforms.viewport_size[1];
		int a= 0;
		int rgba= (a<<24) | (r<<16) | (g<<8) | b;
		texels[ x + y * uniforms.viewport_size[0] ]= rgba;
	}
}
