#pragma once

#include "nowork/Common.h"
#include "nowork/Transform.h"

class Renderer;

class Camera;
typedef std::shared_ptr<Camera> CameraPtr;

class Camera
{
public:


	enum class ClearFlags
	{
		CF_SOLID_COLOR,
		CF_DEPTH_ONLY,
		CF_NONE

	};

	enum FaceCullingType
	{
		NONE = 0,
		FRONT = 0x0404,
		BACK = 0x0405,
	};

	NOWORK_API Camera(Renderer* renderer, float fov = 60.0f, float clipNear = 0.5f, float clipFar = 1000.0f, bool isOrthographic = false);

	NOWORK_API glm::mat4 GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	NOWORK_API glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }
	NOWORK_API void SetProjectionMatrix(glm::mat4 val) { m_ProjectionMatrix = val; }
	NOWORK_API glm::mat4 GetViewMatrix() const { return m_ViewMatrix; }
	NOWORK_API void SetViewMatrix(glm::mat4 val) { m_ViewMatrix = val; }

	//useful for skybox rendering, contains only camera rotation, not position
	NOWORK_API glm::mat4 GetCameraRotationMatrix() const { 
		glm::mat4 camRotMat = m_ViewMatrix;
		camRotMat[3] = glm::vec4(0, 0, 0, 1);
		return camRotMat;
	}

	NOWORK_API inline void UpdateViewProjectionMatrix()
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	NOWORK_API float GetFov() const;
	NOWORK_API void SetFov(float);
	NOWORK_API Transform* GetTransform() { return &m_Transform; }

	NOWORK_API float GetClipNear() const;
	NOWORK_API void SetClipNear(float);

	NOWORK_API float GetClipFar() const;
	NOWORK_API void SetClipFar(float);

	NOWORK_API bool IsOrthographic() const;
	NOWORK_API void SetOrthographic(bool);

	NOWORK_API void ClearScreen(ClearFlags clearFlag);
	NOWORK_API void SetColorMask(bool r, bool g, bool b, bool a);

	NOWORK_API void SetFaceCulling(FaceCullingType type = FaceCullingType::BACK);

	NOWORK_API void SetClearColor(glm::vec3 c) { m_ClearColor = c; }
	NOWORK_API glm::vec3 GetClearColor() { return m_ClearColor; }


	NOWORK_API glm::vec3 WorldPosition();

	NOWORK_API void Update();
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
