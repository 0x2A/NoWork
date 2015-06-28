#pragma once

#include "Common.h"
#include "rapidjson/document.h"
#include "Texture2D.h"
#include "Mesh.h"
#include "Shader.h"
#include "Sprite.h"
#include "SpriteAnimation.h"

class SpriteSheet
{
	friend class Sprite;
	friend class SpriteAnimation;
public:
	
	NOWORK_API ~SpriteSheet();

	NOWORK_API static SpriteSheet* Load(const std::string& path);
	
	NOWORK_API Sprite* GetSprite(size_t index);
	NOWORK_API SpriteAnimation* GetAnimation(size_t index);

	NOWORK_API void SetLinearFiltering(bool b);
	NOWORK_API void SetColorKey(glm::vec3 color);
	NOWORK_API void DisableColorKey();

	NOWORK_API size_t NumAnimations() { return m_SpriteAnimations.size(); }
	NOWORK_API size_t NumSprites() { return m_Sprites.size(); }

	NOWORK_API const std::vector<SpriteAnimation*> GetAnimations() { return m_SpriteAnimations; }
	NOWORK_API const std::vector<Sprite*> GetSrpites() { return m_Sprites; }

protected:

	static Mesh *m_PlaneMesh;
	static Shader* m_SpriteShader;
	static Shader* m_SpriteKeyedShader;

private:

	static bool ParseSprites(SpriteSheet* sheet, rapidjson::Value& doc);
	static bool ParseAnimations(SpriteSheet* sheet, rapidjson::Value& doc);

	static bool GotValue(rapidjson::Value& doc, const char* name, const std::string& msg, bool warning = false, bool error = true);
	static bool GetArea(rapidjson::Value& doc, Area<int>* targetArea);
	
	Texture2D* m_SpriteTexture;
	std::vector<Sprite*> m_Sprites;
	std::vector<SpriteAnimation*> m_SpriteAnimations;
};