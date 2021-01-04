R"(
#version 130
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexUV;
in vec4 vertexColor;
in vec3 vertexTangent;

out vec2 texCoord;
out vec4 vertColor;

void main( void )
{
    texCoord = vertexUV;
    vertColor = vertexColor;
    gl_Position = vec4(vertexPosition.xy, 0, 1);
}
)"