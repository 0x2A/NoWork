R"(
#version 130
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexUV;
in vec4 vertexColor;
in vec3 vertexTangent;
		
uniform mat4 MVPMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
		
out vec2 texCoord;
out vec4 vertColor;
out vec3 worldNormal;
		
void main( void )
{
	texCoord = vertexUV;
	vertColor = vertexColor;
	gl_Position = MVPMatrix * vec4(vertexPosition,1);
	worldNormal = normalize(vec3(ModelMatrix * vec4(vertexNormal,0)));
}
)"