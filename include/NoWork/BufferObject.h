#pragma once
#include "nowork/Common.h"
#include "NoWork/AsyncGLWorker.h"

class BufferObject;
typedef std::shared_ptr<BufferObject> BufferObjectPtr;
class BufferObject : public AsyncGLWorker
{
public:

	enum BindType
	{
		TRANSFORM_FEEDBACK_BUFFER = 0x8C8E,
		UNIFORM_BUFFER = 0x8A11,
		ATOMIC_COUNTER_BUFFER = 0x92C0,
		SHADER_STORAGE_BUFFER = 0x90D2
	};


	NOWORK_API ~BufferObject();

	template<typename T>
	NOWORK_API static BufferObjectPtr Create(const T* data = nullptr)
	{
		return Create(data, sizeof(T));
	}
	
	template<typename T>
	NOWORK_API void Update(const T* data);

	NOWORK_API void Bind(uint slot, BindType bindType);

	NOWORK_API GLuint GetObjectID() { return m_UBO; }

	template<typename T>
	NOWORK_API void GetData(T* target)
	{
		glGetNamedBufferSubData(m_UBO, 0, sizeof(T), (void*)target);
	}

protected:
	NOWORK_API BufferObject();

	NOWORK_API virtual void DoAsyncWork(int mode, void *params) override;


private:
	NOWORK_API static BufferObjectPtr Create(const void* data, size_t size);

	GLuint m_UBO;
};



template<typename T>
NOWORK_API void BufferObject::Update(const T* data)
{
	glNamedBufferSubData(m_UBO, 0, sizeof(T), data);
}