#include "Camera.h"

#include "Renderer.h"


Camera::Camera(Renderer* renderer, float fov /*= 60.0f*/, float clipNear /*= 0.5f*/, float clipFar /*= 1000.0f*/, bool isOrthographic /*= false*/) :
m_Renderer(renderer), m_Fov(fov), m_ClipNear(clipNear), m_ClipFar(clipFar), m_IsOrthographic(isOrthographic), m_ClearFlag(ClearFlags::CF_SOLID_COLOR), m_ClearColor(0, 0, 0)
{
	
	glClear(GL_COLOR_BUFFER_BIT);
	m_ViewMatrix = m_ViewProjectionMatrix = glm::mat4();

	BuildProjectionMatrix();
}

void Camera::BuildProjectionMatrix()
{
	int screenWidth, screenHeight;
	m_Renderer->GetFramebufferSize(screenWidth, screenHeight);
	m_ProjectionMatrix = m_IsOrthographic ? glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f) : glm::perspective(m_Fov, m_Renderer->GetAspectRatio(), m_ClipNear, m_ClipFar);
}

void Camera::Update()
{
	// The right vector will always be parallel to the xz-plane.
	glm::vec3 pos = m_Transform.WorldPosition();
	glm::vec3 forward = m_Transform.Forward();

	m_ViewMatrix = m_Transform.GetModelMatrix();

	BuildProjectionMatrix();
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

}

 float Camera::GetFov() const
{
	 return m_Fov;
}

 void Camera::SetFov(float fov)
{
	 m_Fov = fov;
}

 float Camera::GetClipNear() const
{
	 return m_ClipNear;
}

 void Camera::SetClipNear(float cn)
{
	 m_ClipNear = cn;
}

 float Camera::GetClipFar() const
{
	 return m_ClipFar;
}

 void Camera::SetClipFar(float cf)
{
	 m_ClipFar = cf;
}

 bool Camera::IsOrthographic() const
{
	 return m_IsOrthographic;
}

 void Camera::SetOrthographic(bool o)
{
	 m_IsOrthographic = o;
}

