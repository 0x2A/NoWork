R"(
#version 130
uniform vec4 DiffuseColor;
uniform sampler2D Texture;
	
in vec2 texCoord;
in vec4 vertColor;
	
out vec4 colorOut;
	
void main( void )
{
		vec4 col = texture(Texture, texCoord);
	colorOut = DiffuseColor * vertColor * col;
}
)"