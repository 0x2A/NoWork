#pragma once

#include "nowork/Common.h"
#include "nowork/Texture2D.h"
#include "nowork/Area.h"
#include "nowork/SceneObject.h"

class Sprite : public SceneObject
{
public:

	Sprite(Area<int> area, Texture2D* spriteTexture);

	NOWORK_API Area<int> GetArea() { return m_Area; }

	NOWORK_API void Render();
	NOWORK_API void SetColorKey(glm::vec3 colorKey) { m_ColorKey = colorKey; }
	NOWORK_API void SetKeyThreshold(float val) { m_colorKeyParams.x = val; }
	NOWORK_API void SetKeySmoothing(float val) { m_colorKeyParams.y = val; }
	NOWORK_API void DisableColorKey() { m_ColorKey = glm::vec3(-1, -1, -1); }
	NOWORK_API glm::vec3 GetPosition();
	NOWORK_API glm::vec2 GetSizePixels();
	NOWORK_API glm::vec2 GetSize();

private:

	Area<int> m_Area;
	Area<float> m_glArea;
	Texture2D* m_SpriteTexture;
	float m_AspectRatio;
	glm::vec3 m_ColorKey;
	glm::vec2 m_colorKeyParams;
};