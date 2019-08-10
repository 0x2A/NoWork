#include "nowork/Camera.h"

#include "nowork/Renderer.h"


Camera::Camera(Renderer* renderer, float fov /*= 60.0f*/, float clipNear /*= 0.5f*/, float clipFar /*= 1000.0f*/, bool isOrthographic /*= false*/) :
m_Renderer(renderer), m_Fov(fov), m_ClipNear(clipNear), m_ClipFar(clipFar), m_IsOrthographic(isOrthographic), m_ClearFlag(ClearFlags::CF_SOLID_COLOR), m_ClearColor(0, 0, 0)
{
	
	glClear(GL_COLOR_BUFFER_BIT);
	m_ViewMatrix = m_ViewProjectionMatrix = glm::mat4();

	BuildProjectionMatrix();
}

void Camera::BuildProjectionMatrix()
{
	
	if (m_IsOrthographic)
	{
		//glm::vec3 pos = m_Transform.WorldPosition();
		//float t = glm::max(glm::length(pos),1.0f);

		int screenWidth, screenHeight;
		m_Renderer->GetFramebufferSize(screenWidth, screenHeight);
		float aspect = screenWidth / (float)screenHeight;
		m_ProjectionMatrix = glm::ortho(-aspect, aspect, -1.0f, 1.0f, m_ClipNear, m_ClipFar);
	}
	else
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_Renderer->GetAspectRatio(), m_ClipNear, m_ClipFar);
	}
}

glm::vec3 Camera::WorldPosition()
{
	return glm::vec3(glm::inverse(m_ViewMatrix) * glm::vec4(0, 0, 0, 1));
}

void Camera::Update()
{
	// The right vector will always be parallel to the xz-plane.
	glm::vec3 pos = m_Transform.WorldPosition();
	glm::vec3 forward = m_Transform.Forward();

	m_ViewMatrix = m_Transform.GetModelMatrix();

	BuildProjectionMatrix();
	UpdateViewProjectionMatrix();

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

void Camera::Render()
{

	//TODO: Check culling mask etc.
	GLbitfield clearbf = GL_NONE;
	switch (m_ClearFlag)
	{
	case ClearFlags::CF_DEPTH_ONLY:
		clearbf = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
		break;
	case ClearFlags::CF_SOLID_COLOR:
	default:
		clearbf = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
		break;

	}
	glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0f);
	m_Renderer->ClearScreen(clearbf);

}

NOWORK_API void Camera::ClearScreen(ClearFlags clearFlag)
{
	GLbitfield clearbf = GL_NONE;
	switch (m_ClearFlag)
	{
	case ClearFlags::CF_DEPTH_ONLY:
		clearbf = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
		break;
	case ClearFlags::CF_SOLID_COLOR:
	default:
		clearbf = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
		break;
	}

	glClear(clearbf);
}

NOWORK_API void Camera::SetColorMask(bool r, bool g, bool b, bool a)
{
	glColorMask(r, g, b, a);
}

NOWORK_API void Camera::SetFaceCulling(FaceCullingType type /*= FaceCullingType::BACK*/)
{
	glCullFace(type);
}
