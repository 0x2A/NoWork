
#pragma once

#include "Common.h"

#define RadToDeg 57.2957795130823
#define DegToRad 0.01745329251994

class Transform
{

public:
	Transform();
	void Translate(float x, float y, float z, bool world = false);
	void Translate(const glm::vec3 &dir, bool world = false);
	void SetPosition(const glm::vec3 &pos);
	void SetYPosition(const float &pos);
	void Rotate(float x, float y, float z, bool world = false);
	void Rotate(const glm::vec3 &dir, bool world = false);
	void Scale(const glm::vec3 &scale);
	void Scale(float x, float y, float z);
	glm::vec3 GetScale();

	void Reset();

	glm::vec3 LocalPosition();
	glm::vec3 WorldPosition();
	glm::vec3 GetRotation();
	void LookAt(glm::vec3 targetPosition);

	glm::mat4 GetWorldMatrix();

	glm::vec3 Up();
	glm::vec3 Right();
	glm::vec3 Forward();
	
	glm::mat4 operator*(glm::mat4 const&);
	glm::mat4 operator*(Transform &);
	
		
protected:
	glm::mat4 GetLocalMatrix();
	
private:

	glm::mat4 m_Transform = glm::mat4(), m_Scale;
	Transform* m_ParentTransform = NULL;
	std::vector<Transform*> m_ChildTransforms;
		
};

inline Transform::Transform()
{}

inline void Transform::SetPosition(const glm::vec3 &pos)
{
	glm::vec4 oPos = m_Transform * glm::vec4(0,0,0,1);
	m_Transform *= glm::translate(glm::vec3(-oPos));
	m_Transform *= glm::translate(glm::vec3(pos));
}

inline void Transform::SetYPosition(const float &pos)
{
	glm::vec4 oPos = m_Transform * glm::vec4(0,0,0,1);
	oPos.x = oPos.z = 0;
	m_Transform *= glm::translate(glm::vec3(-oPos));
	m_Transform *= glm::translate(glm::vec3(0,pos,0));
}

inline void Transform::Reset()
{
	m_Transform = glm::mat4();
}

inline void Transform::Translate(float x, float y, float z, bool world)
{
	glm::mat4 transMatrix = glm::translate(glm::vec3(x,y,z));
	if(world)
		m_Transform = m_Transform * transMatrix;
			
	else
		m_Transform = transMatrix * m_Transform;
}

inline void Transform::Translate(const glm::vec3 &dir, bool world)
{
	glm::mat4 transMatrix = glm::translate(dir);
	if(world)
		m_Transform = m_Transform * transMatrix;
			
	else
		m_Transform = transMatrix * m_Transform;
}

inline void Transform::Rotate(float x, float y, float z, bool world)
{
	glm::mat4 rotMatrix = glm::rotate( x, glm::vec3(1,0,0));
	rotMatrix *= glm::rotate( y, glm::vec3(0,1,0));
	rotMatrix *= glm::rotate( z, glm::vec3(0,0,1));

	if(world)
		m_Transform = m_Transform * rotMatrix;
	else
		m_Transform = rotMatrix * m_Transform;
			
}

inline void Transform::Rotate(const glm::vec3 &rot,bool world)
{
	glm::mat4 rotMatrix = glm::rotate( rot.x, glm::vec3(1,0,0));
	rotMatrix *= glm::rotate( rot.y, glm::vec3(0,1,0));
	rotMatrix *= glm::rotate( rot.z, glm::vec3(0,0,1));

	if(world)
		m_Transform = m_Transform * rotMatrix;
			
	else
		m_Transform = rotMatrix * m_Transform;
}


inline void Transform::Scale(const glm::vec3 &scale)
{
	m_Scale *= glm::scale(scale);
}

inline void Transform::Scale(float x, float y, float z)
{
	m_Scale *= glm::scale(glm::vec3(x,y,z));
}

inline  glm::vec3 Transform::GetScale()
{
	return glm::vec3(m_Scale * glm::vec4(1,1,1,0));
}


inline glm::vec3 Transform::LocalPosition()
{
	return glm::vec3(GetLocalMatrix() * glm::vec4(0,0,0,1));
}

inline glm::vec3 Transform::WorldPosition()
{
	return glm::vec3(GetWorldMatrix() * glm::vec4(0,0,0,1));;
}

inline glm::vec3 Transform::Forward() 
{
	glm::vec4 forward = GetWorldMatrix() * glm::vec4(0,0,-1,0);
	return glm::vec3(forward);
}

inline glm::vec3 Transform::Right() 
{
	glm::vec4 right = GetWorldMatrix() * glm::vec4(1,0,0,0);
	return glm::vec3(right);
}

inline glm::vec3 Transform::Up()
{
	glm::vec4 up = GetWorldMatrix() * glm::vec4(0,1,0,0);
	return glm::vec3(up);
}

inline glm::vec3 Transform::GetRotation()
{
	float thetaX, thetaY, thetaZ;
	//Decomposing rotation matrix, upper left part from transform matrix
	if(m_Transform[0][0] == 1.0f)
	{
		thetaY = atan2f(m_Transform[0][2],m_Transform[2][3]);
		thetaX = thetaZ = 0;
	}
	else if (m_Transform[0][0] == -1.0f)
	{
		thetaY = atan2f(m_Transform[0][2],m_Transform[2][3]);
		thetaX = thetaZ = 0;
	}
	else
	{
		thetaY = atan2(-m_Transform[2][0],m_Transform[0][0]);
		thetaZ = asin(m_Transform[1][0]);
		thetaX = atan2(-m_Transform[1][2],m_Transform[1][1]);
	}

	thetaX *= RadToDeg;
	thetaY *= RadToDeg;
	thetaZ *= RadToDeg;
	if(thetaX < 0.0f) thetaX += 360.0f;
	if(thetaY < 0.0f) thetaY += 360.0f;
	if(thetaZ < 0.0f) thetaZ += 360.0f;
	return glm::vec3(thetaX,thetaY,thetaZ);
}

inline glm::mat4 Transform::GetLocalMatrix()
{
	return m_Transform * m_Scale;
}

inline glm::mat4 Transform::GetWorldMatrix()
{
	glm::mat4 m = m_Transform * m_Scale;
	if(m_ParentTransform)
	{
		m = m * m_ParentTransform->GetWorldMatrix();
	}

	return m;
}

inline void Transform::LookAt(glm::vec3 targetPosition)
{
	m_Transform = glm::lookAt(LocalPosition(),targetPosition,glm::vec3(0,1,0));
}

inline glm::mat4 Transform::operator*(glm::mat4 const& m)
{
	return GetLocalMatrix() * m;
}

inline glm::mat4 Transform::operator*(Transform &m)
{
	return GetLocalMatrix() * m.GetLocalMatrix();
}
