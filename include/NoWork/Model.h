#pragma once

#include "NoWork/Common.h"
#include "NoWork/SceneObject.h"
#include "NoWork/Texture2D.h"
#include "NoWork/Mesh.h"
#include <memory>

class Model : public SceneObject
{

public:

	NOWORK_API ~Model();
	NOWORK_API static Model* Load(const std::string path, Mesh::DataUsage usage = Mesh::DataUsage::STATIC_DRAW);

	NOWORK_API Mesh* GetSubmesh(size_t index);

	NOWORK_API int NumSubmeshes() { return m_Meshes.size(); }

	NOWORK_API int SubmeshMaterialIndex(int meshId){ return m_MaterialIndices[meshId]; }

	NOWORK_API std::shared_ptr<Texture2D> GetMeshTexture(int meshId);
	
	NOWORK_API void Render(Shader *shader);

	NOWORK_API void ReplaceTexture(int id, std::shared_ptr<Texture2D> tex);

protected:

	Model();

private:

	std::vector<Mesh* > m_Meshes;
	std::vector<std::shared_ptr<Texture2D>> m_Textures;
	std::vector<int> m_MaterialIndices;
	int m_TotalVertices;
};