R"(
#version 130
in vec4 vertexPosition;
in vec2 vertexUV;

out vec2 texCoord;

void main(void)
{
	texCoord = vertexUV;
	gl_Position = vertexPosition;
};
)"