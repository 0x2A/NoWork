#pragma once

#include "nowork/Common.h"
#include "nowork/Texture2D.h"
#include "nowork/Mesh.h"
#include "nowork/Shader.h"
#include "nowork/Sprite.h"
#include "nowork/SpriteAnimation.h"

class SpriteSheet;
typedef std::shared_ptr<SpriteSheet> SpriteSheetPtr;

class SpriteSheet
{
	friend class Sprite;
	friend class SpriteAnimation;
public:
	
	NOWORK_API ~SpriteSheet();

	NOWORK_API static SpriteSheetPtr Load(const char* path);
	
	NOWORK_API SpritePtr GetSprite(size_t index);
	NOWORK_API SpriteAnimationPtr GetAnimation(size_t index);

	NOWORK_API void SetLinearFiltering(bool b);
	NOWORK_API void SetColorKey(glm::vec3 color);
	NOWORK_API void DisableColorKey();

	NOWORK_API size_t NumAnimations() { return m_SpriteAnimations.size(); }
	NOWORK_API size_t NumSprites() { return m_Sprites.size(); }

	NOWORK_API const std::vector<SpriteAnimationPtr>& GetAnimations() { return m_SpriteAnimations; }
	NOWORK_API const std::vector<SpritePtr>& GetSrpites() { return m_Sprites; }

protected:

	static MeshPtr m_PlaneMesh;
	static ShaderPtr m_SpriteShader;
	static ShaderPtr m_SpriteKeyedShader;

private:

	
	
	Texture2DPtr m_SpriteTexture;
	std::vector<SpritePtr> m_Sprites;
	std::vector<SpriteAnimationPtr> m_SpriteAnimations;
};