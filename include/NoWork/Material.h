#pragma once

#include "nowork/Common.h"
#include "NoWork/Texture.h"
#include "NoWork/Texture2D.h"
#include "NoWork/Shader.h"

class Material;
typedef std::shared_ptr<Material> MaterialPtr;

class NOWORK_API  Material
{
	friend class NoWork;
public:

	Material(const char* name = "", ShaderPtr shader = nullptr);

	void SetCustomTexture(const char* name, TexturePtr texture);
	TexturePtr GetCustomTexture(const char* name);

	void SetCustomValuef(const char* name, float v);
	float GetCustomValuef(const char* name);

	void SetCustomValueVec2(const char* name, glm::vec2 v);
	glm::vec2 GetCustomValueVec2(const char* name);

	void SetCustomValueVec3(const char* name, glm::vec3 v);
	glm::vec3 GetCustomValueVec3(const char* name);

	void SetCustomValueVec4(const char* name, glm::vec4 v);
	glm::vec4 GetCustomValueVec4(const char* name);


	void SetDiffuseTexture(Texture2DPtr tex);
	Texture2DPtr GetDiffuseTexture();
	void SetDiffuseColor(glm::vec4 col);

	void SetNormalTexture(Texture2DPtr tex);
	Texture2DPtr GetNormalTexture();

	void SetMetallicTexture(Texture2DPtr tex);
	Texture2DPtr GetMetallicTexture();
	void SetMetallic(float m);

	void SetRoughnessTexture(Texture2DPtr tex);
	Texture2DPtr GetRoughnessTexture();
	void SetRoughness(float m);

	void SetShader(ShaderPtr shader);
	ShaderPtr GetShader();

	void Bind();

	const char* GetName() const { return m_Name; }
	void Name(const char* val) { m_Name = val; }


	static MaterialPtr FallbackMaterial;

protected:


	static void InitializeDefaultRessources();

private:
	Texture2DPtr m_TexDiffuse;
	Texture2DPtr m_TexNormal;
	Texture2DPtr m_TexMetallic;
	Texture2DPtr m_TexRoughness;

	glm::vec4 m_DiffuseColor;
	float m_Metallic, m_Roughness;
	std::unordered_map<const char*, TexturePtr> m_CustomTextures;
	std::unordered_map<const char*, float> m_CustomfloatValues;
	std::unordered_map<const char*, glm::vec2> m_CustomVec2Values;
	std::unordered_map<const char*, glm::vec3> m_CustomVec3Values;
	std::unordered_map<const char*, glm::vec4> m_CustomVec4Values;
	
	ShaderPtr m_Shader;

	const char* m_Name;
	
};