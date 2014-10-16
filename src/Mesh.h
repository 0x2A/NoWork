#pragma once

#include "Common.h"


class Vertex
{
public:
	Vertex(glm::vec3 _position, glm::vec3 _normal, glm::vec2 _texCoord) :
		position(_position), normal(_normal), texCoord(_texCoord)
	{
		if (glm::length(normal) != 0)
			glm::normalize(normal);
	}

	Vertex(glm::vec3 _position, glm::vec3 _normal) : Vertex(_position, _normal, glm::vec2())
	{}

	Vertex(glm::vec3 _position) : Vertex(_position, glm::vec3())
	{}

	glm::vec3 position; //Position of the vertex
	glm::vec3 normal; //Normal of the vertex
	glm::vec2 texCoord; //Texture coordinate of the vertex

private:
};


class Mesh
{
public:

	NOWORK_API ~Mesh();
	NOWORK_API static Mesh* Create(const std::vector<Vertex>& vertices);
	

protected:
	Mesh();

private:
	bool CreateVBO();


	std::vector<Vertex> m_Vertices;
};