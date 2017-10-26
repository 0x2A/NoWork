
#pragma once

#include "nowork/Common.h"
#include "nowork/Quaternion.h"


#define RadToDeg 57.2957795130823f
#define DegToRad 0.01745329251994f

class Transform
{

public:
	NOWORK_API Transform();
	NOWORK_API virtual void Translate(float x, float y, float z, bool world = false);
	NOWORK_API virtual void Translate(const glm::vec3 &dir, bool world = false);
	NOWORK_API virtual void Transformate(const glm::mat4& mat, bool world = false);
	NOWORK_API virtual void SetPosition(const glm::vec3 &pos);
	NOWORK_API virtual void SetYPosition(const float &pos);
	NOWORK_API virtual void Rotate(float x, float y, float z, bool world = false);
	NOWORK_API virtual void Rotate(const glm::vec3 &dir, bool world = false);
	NOWORK_API virtual void Rotate(Quaternion &q, bool world = false);
	NOWORK_API virtual void Rotate(glm::quat &q, bool world = false);
	NOWORK_API virtual void ResetRotation();
	NOWORK_API virtual void Scale(const glm::vec3 &scale);
	NOWORK_API virtual void Scale(float x, float y, float z);
	NOWORK_API virtual void SetScale(float x, float y, float z);
	NOWORK_API virtual void SetScale(const glm::vec3 &scale);
	NOWORK_API virtual glm::vec3 GetScale();

	NOWORK_API static glm::vec4 ScreenSpaceToWorldSpacePoint(glm::ivec2 ssPpoint, glm::mat4 ViewProjectionMatrix, glm::ivec2 viewportSize);


	NOWORK_API void Reset();

	NOWORK_API virtual glm::vec3 WorldPosition();
	NOWORK_API glm::vec3 GetRotation();
	NOWORK_API void LookAt(glm::vec3 &targetPosition);
	
	NOWORK_API virtual glm::vec3 Up();
	NOWORK_API virtual glm::vec3 Right();
	NOWORK_API virtual glm::vec3 Forward();
	
	NOWORK_API glm::mat4 operator*(glm::mat4 const&);
	NOWORK_API glm::mat4 operator*(Transform &);
	
	NOWORK_API virtual glm::mat4 GetModelMatrix();

	//overrides the model matrix (useful for custom matrix calculations)
	NOWORK_API void SetModelMatrix(glm::mat4&);

protected:

	glm::mat4 m_Transform = glm::mat4(1.0f), m_Scale = glm::mat4(1.0);
	std::vector<Transform*> m_ChildTransforms;
		
};

class ViewTransform : public Transform
{

public:
	ViewTransform();
	NOWORK_API virtual void Translate(float x, float y, float z, bool world = false) override;
	NOWORK_API virtual void Translate(const glm::vec3 &dir, bool world = false) override;
	NOWORK_API virtual void Transformate(const glm::mat4& mat, bool world = false) override;
	NOWORK_API virtual void Rotate(float x, float y, float z, bool world = false) override;
	NOWORK_API virtual void Rotate(const glm::vec3 &dir, bool world = false) override;

	NOWORK_API virtual glm::mat4 GetModelMatrix() override;
	NOWORK_API glm::mat4 GetViewMatrix();

	NOWORK_API virtual glm::vec3 ViewUp();
	NOWORK_API virtual glm::vec3 ViewRight();
	NOWORK_API virtual glm::vec3 ViewForward();
};