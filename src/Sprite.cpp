#include "Sprite.h"
#include "Shader.h"
#include "SpriteSheet.h"

Sprite::Sprite(Area<int> area, Texture2D* spriteTexture)
{
	m_SpriteTexture = spriteTexture;

	m_Area = area;

	//calculate OpenGL scaled texture params
	glm::ivec2 texSize = spriteTexture->GetSize();
	m_glArea = Area<float>(m_Area.X1() / (float)texSize.x, m_Area.Y1() / (float)texSize.y, m_Area.X2() / (float)texSize.x, m_Area.Y2() / (float)texSize.y);

	m_AspectRatio = m_Area.Width() / (float)m_Area.Height();

	m_colorKeyParams.x = 0.01f;
	m_colorKeyParams.y = 0.0f;
}

void Sprite::Render()
{
	
	bool useColorKey = !(m_ColorKey.x == -1 && m_ColorKey.y == -1 && m_ColorKey.z == -1); //do we use color keying?
	Shader* shader = useColorKey ? SpriteSheet::m_SpriteKeyedShader : SpriteSheet::m_SpriteShader; //use normal shader

	shader->Use();
	shader->SetParameterVec4("texCoordParams", glm::vec4( m_glArea.X1(), m_glArea.Y1(), m_glArea.Width(), m_glArea.Height() ));
	shader->SetTexture(m_SpriteTexture);
	shader->SetDiffuseColor(glm::vec4(1, 1, 1, 1));
	if (useColorKey)
	{
		shader->SetParameterVec3("colorKey", m_ColorKey);
		shader->SetParameterVec2("colorKeyParams", m_colorKeyParams);
	}

	Mesh* mesh = SpriteSheet::m_PlaneMesh;
	mesh->GetTransform()->SetModelMatrix(m_Transform.GetModelMatrix());
	mesh->GetTransform()->SetScale(glm::vec3(m_AspectRatio, 1, 1));
	mesh->Render(shader);	
}

glm::vec3 Sprite::GetPosition()
{
	return m_Transform.WorldPosition();
}

glm::vec2 Sprite::GetSizePixels()
{
	return glm::vec2(m_Area.Width(), m_Area.Height());
}

glm::vec2 Sprite::GetSize()
{
	auto scale = m_Transform.GetScale();
	return glm::vec2(scale);
}

