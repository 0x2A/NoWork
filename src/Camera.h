#pragma once

#include "Common.h"
#include "Transform.h"

class Renderer;
class Camera
{
public:


	enum class ClearFlags
	{
		CF_SOLID_COLOR,
		CF_DEPTH_ONLY,
		CF_NONE

	};

	NOWORK_API Camera(Renderer* renderer, float fov = 60.0f, float clipNear = 0.5f, float clipFar = 1000.0f, bool isOrthographic = false);

	NOWORK_API glm::mat4 GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	NOWORK_API glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }
	NOWORK_API void SetProjectionMatrix(glm::mat4 val) { m_ProjectionMatrix = val; }
	NOWORK_API glm::mat4 GetViewMatrix() const { return m_ViewMatrix; }
	NOWORK_API void SetViewMatrix(glm::mat4 val) { m_ViewMatrix = val; }

	NOWORK_API float GetFov() const;
	NOWORK_API void SetFov(float);
	NOWORK_API Transform* GetTransform() { return &m_Transform; }

	NOWORK_API float GetClipNear() const;
	NOWORK_API void SetClipNear(float);

	NOWORK_API float GetClipFar() const;
	NOWORK_API void SetClipFar(float);

	NOWORK_API bool IsOrthographic() const;
	NOWORK_API void SetOrthographic(bool);


	void Update();
	void Render();

private:
	void BuildProjectionMatrix();


	glm::mat4 m_ViewMatrix, m_ProjectionMatrix, m_ViewProjectionMatrix;
	float m_Fov, m_ClipNear, m_ClipFar;

	bool m_IsOrthographic;
	float m_TargetPoint;

	glm::vec3 m_ClearColor;

	ClearFlags m_ClearFlag;

	Renderer* m_Renderer;

	Transform m_Transform;
	
};
