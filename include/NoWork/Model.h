#pragma once

#include "NoWork/Common.h"
#include "NoWork/SceneObject.h"
#include "NoWork/Texture2D.h"
#include "NoWork/Mesh.h"

class Model : public SceneObject
{

public:

	NOWORK_API ~Model();
	NOWORK_API static Model* Load(const std::string path, Mesh::DataUsage usage = Mesh::DataUsage::STATIC_DRAW);

	NOWORK_API Mesh* GetSubmesh(size_t index);
	
	NOWORK_API void Render(Shader *shader);

protected:

	Model();

private:

	std::vector<Mesh* > m_Meshes;
	std::vector<Texture2D*> m_Textures;
	std::vector<int> m_MaterialIndices;
	int m_TotalVertices;
};