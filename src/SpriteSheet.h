#pragma once

#include "Common.h"
#include "rapidjson/document.h"
#include "Texture2D.h"
#include "Mesh.h"
#include "Shader.h"
#include "Sprite.h"

class SpriteSheet
{
	friend class Sprite;
public:
	
	NOWORK_API ~SpriteSheet();

	NOWORK_API static SpriteSheet* Load(const std::string& path);
	NOWORK_API Sprite* GetSprite(size_t index);
	NOWORK_API void SetLinearFiltering(bool b);
	NOWORK_API void SetColorKey(glm::vec3 color);
	NOWORK_API void DisableColorKey();

protected:

	static Mesh *m_PlaneMesh;
	static Shader* m_SpriteShader;
	static Shader* m_SpriteKeyedShader;

private:

	static bool ParseSprites(SpriteSheet* sheet, rapidjson::Value& doc);
	static bool ParseAnimations(SpriteSheet* sheet, rapidjson::Value& doc);

	static bool GotValue(rapidjson::Value& doc, const char* name, const std::string& msg, bool warning = false, bool error = true);

	
	std::string m_Name;
	Texture2D* m_SpriteTexture;
	std::vector<Sprite*> m_Sprites;
};