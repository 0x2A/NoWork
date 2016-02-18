#include "nowork/SpriteAnimation.h"
#include "nowork/SpriteSheet.h"


SpriteAnimation::SpriteAnimation(std::vector<Area<int>> frames, int frameRate, std::string name, Texture2DPtr spriteTexture)
{
	m_SpriteTexture = spriteTexture;

	m_Frames = frames;
	m_EndFrame = frames.size() - 1;
	m_startFrame = 0;
	m_CurrentFrame = m_startFrame;
	m_FrameRate = frameRate;

	m_Finished = false;
	m_Loop = true;

	m_FrameTime = 1.0f / (float)m_FrameRate;

	m_LastFrameTime = 0;
	m_Name = name;

	//calculate OpenGL scaled texture params
	glm::ivec2 texSize = spriteTexture->GetSize();
	m_glFrames.reserve(m_Frames.size());
	for (int i = 0; i < m_Frames.size(); i++)
	{
		m_glFrames.push_back(Area<float>(m_Frames[i].X1() / (float)texSize.x, m_Frames[i].Y1() / (float)texSize.y, m_Frames[i].X2() / (float)texSize.x, m_Frames[i].Y2() / (float)texSize.y));
	}

	//calculate aspect ratio
	//TODO: calculate this for every frame, so change in aspect ratio during animation is possible
	m_AspectRatio = m_Frames[0].Width() / (float)m_Frames[0].Height();

	m_colorKeyParams.x = 0.01f;
	m_colorKeyParams.y = 0.0f;
}

void SpriteAnimation::Render(glm::mat4& modelMatrix)
{
	bool useColorKey = !(m_ColorKey.x == -1 && m_ColorKey.y == -1 && m_ColorKey.z == -1); //do we use color keying?
	ShaderPtr shader = useColorKey ? SpriteSheet::m_SpriteKeyedShader : SpriteSheet::m_SpriteShader; //use normal shader

	Area<float> area = m_glFrames[m_CurrentFrame];
	shader->Use();
	shader->SetParameterVec4("texCoordParams", glm::vec4(area.X1(), area.Y1(), area.Width(), area.Height()));
	shader->SetTexture(m_SpriteTexture);
	shader->SetDiffuseColor(glm::vec4(1, 1, 1, 1));
	if (useColorKey)
	{
		shader->SetParameterVec3("colorKey", m_ColorKey);
		shader->SetParameterVec2("colorKeyParams", m_colorKeyParams);
	}

	MeshPtr mesh = SpriteSheet::m_PlaneMesh;
	mesh->GetTransform()->SetModelMatrix(modelMatrix);
	mesh->GetTransform()->SetScale(glm::vec3(m_AspectRatio, 1, 1));
	mesh->Render(shader);
}

void SpriteAnimation::Update(float deltaTime)
{
	if (m_Finished) //if animation is finished there is nothing to do
		return;

	//head to the next frame if passed time is higher or equal to frame time (framerate)
	static float currentFrameTime;
	currentFrameTime += deltaTime;
	if (currentFrameTime - m_LastFrameTime >= m_FrameTime)
	{
		m_CurrentFrame++;
		if (m_CurrentFrame > m_EndFrame)
		{
			//jump back to start frame if loop is enabled
			if (m_Loop)
			{
				m_CurrentFrame = m_startFrame;
			}
			else //otherwise set the finished flag
			{
				m_CurrentFrame = m_EndFrame;
				m_Finished = true;
			}
		}
		m_LastFrameTime = currentFrameTime;
	}
}


glm::vec2 SpriteAnimation::GetSizePixels()
{
	return glm::vec2(m_Frames[0].Width(), m_Frames[0].Height());
}

void SpriteAnimation::SetFrameRate(int framerate)
{
	m_FrameRate = framerate;
	m_FrameTime = 1000.0f / (float)m_FrameRate;
}

