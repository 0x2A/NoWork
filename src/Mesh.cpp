#include "Mesh.h"

#include "Log.h"

Mesh::Mesh()
{}

Mesh::~Mesh()
{
	m_Vertices.clear();
}

Mesh* Mesh::Create(const std::vector<Vertex>& vertices)
{
	LOG_DEBUG("Creating mesh with " << vertices.size() << " vertices");

	Mesh* mesh = new Mesh;
	mesh->m_Vertices = vertices;
	if (!mesh->CreateVBO())
	{
		return false;
	}

	return mesh;
}


bool Mesh::CreateVBO()
{
	if (m_Vertices.size() == 0)
	{
		LOG_WARNING("No vertices defined. Please add some vertices",__FUNCTION__);
		return true;
	}


	return true;
}
