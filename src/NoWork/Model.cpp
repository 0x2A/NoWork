#include "NoWork/Model.h"
#include "NoWork/Log.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


Model::Model()
{
	m_TotalVertices = 0;
}

std::string GetBasePath(const std::string& filename)
{
	std::string fName(filename);
	size_t pos = fName.rfind("/");
	if (pos == std::string::npos)
	{
		pos = fName.rfind("\\");
		if (pos == std::string::npos)
			return fName;
	}

	if (pos == 0)    // / is at the front.
		return fName;

	return fName.substr(0, pos);
}

std::string GetFilename(const std::string& filename)
{
	std::string fName(filename);
	size_t pos = fName.rfind("/");
	if (pos == std::string::npos)
	{
		pos = fName.rfind("\\");
		if (pos == std::string::npos)
			return fName;
	}

	if (pos == 0)    // / is at the front.
		return fName;

	return fName.substr(pos + 1, fName.size() - (pos + 1));
}

Model* Model::Load(const std::string path, Mesh::DataUsage usage)
{
	LOG_DEBUG("Loading model '" << path << "'...");

	
	double startTime = glfwGetTime();
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!scene)
	{
		LOG_ERROR("Unable to load Model '" << path << "'");
		return nullptr;
	}

	const int iVertexTotalSize = sizeof(aiVector3D) * 2 + sizeof(aiVector2D);

	int iTotalVertices = 0;

	Model* model = new Model;
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		VertexList vertices;
		FaceList faces;

		aiMesh* mesh = scene->mMeshes[i];

		//get indices/faces
		int iMeshFaces = mesh->mNumFaces;
		model->m_MaterialIndices.push_back(mesh->mMaterialIndex);
		for (int j = 0; j < iMeshFaces; j++)
		{
			const aiFace& face = mesh->mFaces[j];
			faces.push_back(Face(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
		}

		//get vertices
		vertices.reserve(mesh->mNumVertices);
		bool hasNormals = mesh->HasNormals();
		bool hasUvCoords = mesh->mNumUVComponents[0] > 0;
		for (int k = 0; k < mesh->mNumVertices; k++)
		{
			aiVector3D pos = mesh->mVertices[k];
			aiVector3D uv = hasUvCoords ? mesh->mTextureCoords[0][k] : aiVector3D(0,0,0);
			aiVector3D normal = hasNormals ? mesh->mNormals[k] : aiVector3D(1.0f, 1.0f, 1.0f);

			Vertex v = Vertex(glm::vec3(pos.x, pos.y, pos.z), glm::vec3(normal.x, normal.y, normal.z), glm::vec2(uv.x, uv.y));
			vertices.push_back(v);
		}
		Mesh* m = Mesh::Create(vertices, faces, false, usage);
		if (!m)
		{
			LOG_ERROR("Unable to load submesh '" << mesh->mName.C_Str() << "' from model '" << path << "'");
			continue;
		}
		model->m_Meshes.push_back(m);
		model->m_TotalVertices += mesh->mNumVertices;
	}

	int iNumMaterials = scene->mNumMaterials;
	std::vector<int> materialRemap(iNumMaterials);

	std::string basePath = GetBasePath(path);

	for (int i = 0; i < iNumMaterials; i++)
	{
		const aiMaterial* material = scene->mMaterials[i];
		int a = 5;
		int texIndex = 0;
		aiString texPath;  // filename

		Texture2D* tex = nullptr;
		if (material->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath) == AI_SUCCESS)
		{
			tex = Texture2D::Load(basePath + "/" + texPath.data);
		}
		model->m_Textures.push_back(tex);
	}

	double endTime = glfwGetTime();

	LOG_DEBUG("Model loaded: " << model->m_TotalVertices << " vertices total.");
	LOG_DEBUG("Loading model took " << endTime - startTime << " seconds");

	return model;
}

Model::~Model()
{
	for (auto &mesh : m_Meshes)
	{
		delete mesh;
	}
	m_Meshes.clear();

	for (auto &tex : m_Textures)
	{
		delete tex;
	}
	m_Textures.clear();
	m_MaterialIndices.clear();
}

Mesh* Model::GetSubmesh(size_t index)
{
	if (index >= m_Meshes.size()) return nullptr;
	return m_Meshes[index];
}

void Model::Render(Shader *shader)
{
	shader->Use();
	shader->SetDiffuseColor(glm::vec4(1, 1, 1, 1));

	for (int i = 0; i < m_Meshes.size(); i++)
	{
		if (m_Textures[m_MaterialIndices[i]])
			shader->SetTexture(m_Textures[m_MaterialIndices[i]]);

		m_Meshes[i]->GetTransform()->SetModelMatrix(m_Transform.GetModelMatrix());
		m_Meshes[i]->Render(shader);
	}
}
