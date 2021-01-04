R"(
#version 130
uniform vec4 DiffuseColor;
uniform sampler2D Texture;
uniform vec3 LightPos;
		
in vec2 texCoord;
in vec4 vertColor;
in vec3 worldNormal;
		
out vec4 colorOut;
		
void main(void)
{
	float ndotl = clamp(dot(normalize(worldNormal), normalize(LightPos)), 0, 1);
	colorOut = DiffuseColor * vertColor * ndotl;
}
)"