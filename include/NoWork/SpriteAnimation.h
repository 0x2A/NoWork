#pragma once

#include "nowork/Common.h"
#include "nowork/Texture2D.h"
#include "nowork/Area.h"

class SpriteAnimation
{
public:

	SpriteAnimation(std::vector<Area<int>> frames, int frameRate, std::string name, Texture2D* spriteTexture);

	NOWORK_API void Render(glm::mat4& modelMatrix);
	NOWORK_API void Update(float deltaTime);

	NOWORK_API void SetColorKey(glm::vec3 colorKey) { m_ColorKey = colorKey; }
	NOWORK_API void SetKeyThreshold(float val) { m_colorKeyParams.x = val; }
	NOWORK_API void SetKeySmoothing(float val) { m_colorKeyParams.y = val; }
	NOWORK_API void DisableColorKey() { m_ColorKey = glm::vec3(-1, -1, -1); }

	NOWORK_API glm::vec2 GetSizePixels();
	
	NOWORK_API int GetCurrentFrame() { return m_CurrentFrame; }

	NOWORK_API void SetStartFrame(int i) { m_startFrame = i; }
	NOWORK_API int GetStartFrame() { return m_startFrame; }
	NOWORK_API void SetEndFrame(int i) { m_EndFrame = i; }
	NOWORK_API int GetEndFrame() { return m_EndFrame; }

	NOWORK_API void SetFrameRate(int framerate);
	NOWORK_API int GetFrameRate() { return m_FrameRate; }

	NOWORK_API const std::string& GetName() { return m_Name; }

	NOWORK_API bool Finished() const { return m_Finished; }

	NOWORK_API bool IsLooping() const { return m_Loop; }
	NOWORK_API void Loop(bool val) { m_Loop = val; m_Finished = false; }

	NOWORK_API void Reset() { m_CurrentFrame = m_startFrame; }

private:

	int m_CurrentFrame;
	int m_startFrame;
	int m_EndFrame;
	int m_FrameRate;

	bool m_Loop;
	
	bool m_Finished;
	
	float m_FrameTime;
	float m_LastFrameTime;

	std::string m_Name;

	std::vector<Area<int>> m_Frames;
	std::vector<Area<float>> m_glFrames;
	
	float m_AspectRatio;
	
	Texture2D* m_SpriteTexture;

	glm::vec3 m_ColorKey;
	glm::vec2 m_colorKeyParams;
	
};