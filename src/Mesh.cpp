#include "Mesh.h"

#include "Log.h"

Mesh::Mesh()
{}

Mesh::~Mesh()
{
	m_Vertices.clear();

	unsigned int buffers[] = { m_IndexBuffer, m_VertexBuffer, m_NormalBuffer, m_TexCoordBuffer };
	glDeleteBuffers(4, buffers);
	glDeleteVertexArrays(1, &m_VertexArrayObject);
}

Mesh* Mesh::Create(Renderer* renderer, const std::vector<Vertex>& vertices, const std::vector<Face>& faces, bool calculateNormals, DataUsage usage)
{
	LOG_DEBUG("Creating mesh with " << vertices.size() << " vertices");

	Mesh* mesh = new Mesh;
	mesh->m_DataUsage = usage;
	mesh->m_Vertices = vertices;
	mesh->m_Renderer = renderer;
	mesh->m_Faces = faces;
	mesh->m_NumIndices = (unsigned int)faces.size() * 3;
	mesh->m_NumVertices = (unsigned int)vertices.size();

	if (mesh->m_Faces.size() == 0 && calculateNormals || mesh->m_Faces.size() > 0)
		mesh->CalculateNormals();

	if (!mesh->CreateVBO())
	{
		delete mesh;
		return NULL;
	}
	return mesh;
}

NOWORK_API Mesh* Mesh::Create(Renderer* renderer, const std::vector<Vertex>& vertices, DataUsage usage /*= DataUsage::STATIC_DRAW*/)
{
	LOG_DEBUG("Creating mesh with " << vertices.size() << " vertices");

	Mesh* mesh = new Mesh;
	mesh->m_DataUsage = usage;
	mesh->m_Vertices = vertices;
	mesh->m_Renderer = renderer;
	mesh->m_NumIndices = 0;
	mesh->m_NumVertices = (unsigned int)vertices.size();

	if (!mesh->CreateVBO())
	{
		delete mesh;
		return NULL;
	}
	return mesh;
}


void Mesh::CalculateNormals()
{
	if (m_Faces.size() == 0)
	{
		if (m_Vertices.size() < 3)
		{
			LOG_ERROR("Not enough vertices to calculate normals (min 3 needed)");
			return;
		}

		glm::vec3 normal;
		for (int i = 0; i < m_Vertices.size(); i++)
		{
			normal = glm::cross(m_Vertices[i + 1].position - m_Vertices[i].position, m_Vertices[i + 2].position - m_Vertices[i].position);
			normal = glm::normalize(normal);
			m_Vertices[i].normal = m_Vertices[i + 1].normal = m_Vertices[i + 2].normal = normal;
			i += 2;
		}
	}
}

bool Mesh::CreateVBO()
{
	if (m_Vertices.size() == 0)
	{
		LOG_ERROR("No vertices defined. Please add some vertices",__FUNCTION__);
		return false;
	}
	if (m_NumIndices == 0)
	{
		LOG_WARNING("No indices defined. Rendering in array mode (slower)", __FUNCTION__);
	}
	
	//generate vertex buffer object
	glGenVertexArrays(1, &m_VertexArrayObject);
	glBindVertexArray(m_VertexArrayObject);

	// buffer for indices
	if (m_NumIndices > 0)
	{
		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Face) * m_Faces.size(), &m_Faces[0], m_DataUsage);
	}

	//buffer for vertices
	glGenBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_Vertices.size(), &m_Vertices[0], m_DataUsage);

	
	//setup data locations

	// Positions (location = 0)
	glEnableVertexAttribArray(MODEL_VERTEX_LOCATION);
	glVertexAttribPointer(MODEL_VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	//Normals (location = 1)
	glEnableVertexAttribArray(MODEL_NORMAL_LOCATION);
	glVertexAttribPointer(MODEL_NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));

	//texcoords (location = 2)
	glEnableVertexAttribArray(MODEL_TEXCOORD_LOCATION);
	glVertexAttribPointer(MODEL_TEXCOORD_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)*2));


	// unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

void Mesh::Render(Shader* shader, RenderMode mode)
{
	if (!shader)
	{
		LOG_ERROR("Invalid shader! Cant render!");
		return;
	}
	glBindVertexArray(m_VertexArrayObject);

	glm::mat4 modelMatrix = m_Transform.GetModelMatrix();

	shader->Use();
	shader->SetParameterMat4("ModelMatrix", modelMatrix);
	shader->SetParameterMat4("MVPMatrix", m_Renderer->GetCamera()->GetViewProjectionMatrix()*modelMatrix);
	shader->SetParameterMat4("ModelViewMatrix", m_Renderer->GetCamera()->GetViewMatrix()*modelMatrix);

	if (m_NumIndices > 0)
		glDrawElements(mode, m_NumIndices, GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(mode, 0, m_NumVertices);

	glBindVertexArray(0);
}