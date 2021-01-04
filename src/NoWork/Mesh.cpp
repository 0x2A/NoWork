#include "nowork/Mesh.h"
#include "nowork/Log.h"
#include "nowork/Renderer.h"
#include "NoWork/Framework.h"

Renderer* Mesh::m_sRenderer = NULL;


Mesh::Mesh() : AsyncGLWorker()
{
	m_VertexArrayObject = 0;
}


Mesh::~Mesh()
{
	m_Vertices.clear();
	m_Faces.clear();

	unsigned int buffers[] = { m_IndexBuffer, m_VertexBuffer, m_NormalBuffer, m_TexCoordBuffer };
	
	glDeleteBuffers(4, buffers);
	glDeleteVertexArrays(1, &m_VertexArrayObject);
}

MeshPtr Mesh::Create(const Vertex *vertices, int numVertices, const Face *faces, int numFaces, bool calculateNormals, DataUsage usage)
{
	LOG_DEBUG("Creating mesh with " << numVertices << " vertices");

	MeshPtr mesh = MeshPtr(new Mesh);
	mesh->m_DataUsage = usage;
	mesh->m_Vertices.assign(vertices, vertices + numVertices);
	mesh->m_Renderer = m_sRenderer;
	mesh->m_Faces.assign(faces, faces + numFaces);
	mesh->m_NumIndices = (unsigned int)numFaces * 3;
	mesh->m_NumVertices = (unsigned int)numVertices;

	if (calculateNormals)
		mesh->CalculateNormals();

	mesh->CreateVBO();
	return mesh;
}

NOWORK_API MeshPtr Mesh::Create(const Vertex *vertices, int numVertices, DataUsage usage /*= DataUsage::STATIC_DRAW*/)
{
	LOG_DEBUG("Creating mesh with " << numVertices << " vertices");

	MeshPtr mesh = MeshPtr(new Mesh);
	mesh->m_DataUsage = usage;
	mesh->m_Vertices.assign(vertices, vertices + numVertices);
	mesh->m_Renderer = m_sRenderer;
	mesh->m_NumIndices = 0;
	mesh->m_NumVertices = (unsigned int)numVertices;

	mesh->CreateVBO();
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
	if (m_Vertices.size() == 0 && !(m_DataUsage == DYNAMIC_COPY || m_DataUsage == DYNAMIC_DRAW ||m_DataUsage == DYNAMIC_READ))
	{
		LOG_ERROR("No vertices defined. Please add some vertices",__FUNCTION__);
		return false;
	}
	
	if (!NoWork::IsMainThread())
	{
		AddToGLQueue(0);
		return true;
	}

	// buffer for indices
	if (m_NumIndices > 0 || (m_DataUsage == DYNAMIC_COPY || m_DataUsage == DYNAMIC_DRAW || m_DataUsage == DYNAMIC_READ))
	{
		glCreateBuffers(1, &m_IndexBuffer);
		
		if (m_Faces.size() != 0)
			glNamedBufferData(m_IndexBuffer, sizeof(Face) * m_Faces.size(), &m_Faces[0], m_DataUsage);
	}

	//buffer for vertices
	glCreateBuffers(1, &m_VertexBuffer);
	if (m_Vertices.size() != 0)
		glNamedBufferData(m_VertexBuffer, sizeof(Vertex) * m_Vertices.size(), &m_Vertices[0], m_DataUsage);


	//generate vertex buffer object
	glCreateVertexArrays(1, &m_VertexArrayObject);

	//assign buffers to vertex array object
	glVertexArrayVertexBuffer(m_VertexArrayObject, 0, m_VertexBuffer, 0, sizeof(Vertex));
	glVertexArrayElementBuffer(m_VertexArrayObject, m_IndexBuffer);

	//setup data locations

	// Positions (location = 0)
	glEnableVertexArrayAttrib(m_VertexArrayObject, MODEL_VERTEX_LOCATION);
	glVertexArrayAttribFormat(m_VertexArrayObject, MODEL_VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex));

	//Normals (location = 1)
	glEnableVertexArrayAttrib(m_VertexArrayObject, MODEL_NORMAL_LOCATION);
	glVertexArrayAttribFormat(m_VertexArrayObject, MODEL_NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3));

	//texcoords (location = 2)
	glEnableVertexArrayAttrib(m_VertexArrayObject, MODEL_TEXCOORD_LOCATION);
	glVertexArrayAttribFormat(m_VertexArrayObject, MODEL_TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, (sizeof(glm::vec3) * 2));

	//vertexColor (location = 3)
	glEnableVertexArrayAttrib(m_VertexArrayObject, MODEL_VERTEX_COLOR_LOCATION);
	glVertexArrayAttribFormat(m_VertexArrayObject, MODEL_VERTEX_COLOR_LOCATION, 4, GL_FLOAT, GL_FALSE, (sizeof(glm::vec3) * 2 + sizeof(glm::vec2)));

	glEnableVertexArrayAttrib(m_VertexArrayObject, MODEL_TANGENT_LOCATION);
	glVertexArrayAttribFormat(m_VertexArrayObject, MODEL_TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, (sizeof(glm::vec3) * 2 + sizeof(glm::vec2) + sizeof(glm::vec4)));
	

	//clear buffer on static draw since we dont need it in system memory anymore
	if (m_DataUsage == STATIC_DRAW)
	{
		m_Vertices.clear();
		m_Faces.clear();
	}

	return true;
}

void Mesh::Render(MaterialPtr material, RenderMode mode)
{
	if (!material)
	{
		LOG_ERROR("Invalid material! Cant render!");
		return;
	}

//	if (m_Vertices.size() == 0)
//		return;

	material->Bind();
	auto shader = material->GetShader();
	Render(shader);
	
}

NOWORK_API void Mesh::Render(ShaderPtr shader, RenderMode mode /*= TRIANGLES*/)
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

	Renderer::DrawCalls++;
}

NOWORK_API MeshPtr Mesh::CreatePlane(DataUsage usage /*= DataUsage::STATIC_DRAW*/)
{
	std::vector<Vertex> vertices;
	std::vector<Face> faces;

	Vertex v(glm::vec3(-1, 1, 0), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec4(1, 1, 1, 1)); vertices.push_back(v);
	v = Vertex(glm::vec3(1, 1, 0), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec2(1, 1), glm::vec4(1, 1, 1, 1)); vertices.push_back(v);
	v = Vertex(glm::vec3(1, -1, 0), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec2(1, 0), glm::vec4(1, 1, 1, 1)); vertices.push_back(v);
	v = Vertex(glm::vec3(-1, -1, 0), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec2(0, 0), glm::vec4(1, 1, 1, 1)); vertices.push_back(v);

	faces.push_back(Face(0, 3, 1));
	faces.push_back(Face(1, 3, 2));
	return Create(&vertices[0], vertices.size(), &faces[0], faces.size(), false, usage);
}

void Mesh::Init(Renderer* renderer)
{
	m_sRenderer = renderer;
}

void Mesh::UpdateBufferData(bool reallocate)
{
	if (!(m_DataUsage == DYNAMIC_DRAW || m_DataUsage == DYNAMIC_COPY))
		return;

	if (!NoWork::IsMainThread())
	{
		AddToGLQueue(1, reinterpret_cast<void*>(+reallocate));
		return;
	}

	if (m_Vertices.size() == 0) return;

	m_NumIndices = m_Faces.size()*3;
	m_NumVertices = m_Vertices.size();

	if (m_NumIndices > 0)
	{		
		if (reallocate)
			glNamedBufferData(m_IndexBuffer, sizeof(Face) * m_Faces.size(), &m_Faces[0], m_DataUsage);
		else
			glNamedBufferSubData(m_IndexBuffer, 0, sizeof(Face) * m_Faces.size(), &m_Faces[0]);
	}
	
	if (reallocate)
		glNamedBufferData(m_VertexBuffer, sizeof(Vertex) * m_Vertices.size(), &m_Vertices[0], m_DataUsage);
	else
		glNamedBufferSubData(m_VertexBuffer, 0, sizeof(Vertex) * m_Vertices.size(), &m_Vertices[0]);

	
}

void Mesh::DoAsyncWork(int mode, void *params)
{
	switch (mode)
	{
	case 0:
		CreateVBO();
		break;
	case 1:
		UpdateBufferData((bool)(params));
		break;
	}
}

