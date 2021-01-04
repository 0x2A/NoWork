R"(
#version 130
uniform sampler2D texture0;

in vec2 texCoord;
out vec4 colorOut;

void main(void)
{
	colorOut = vec4(texture(texture0,texCoord).xyz, 1);
};
)"