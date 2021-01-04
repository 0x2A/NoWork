R"(
	#version 130
	uniform vec4 DiffuseColor;
		
	in vec2 texCoord;
	in vec4 vertColor;
		
	out vec4 colorOut;
		
	void main( void )
	{
		colorOut = DiffuseColor * vertColor;
	}
)"