#pragma once
#include "nowork/Common.h"
#include "NoWork/AsyncGLWorker.h"

class BufferObject;
typedef std::shared_ptr<BufferObject> BufferObjectPtr;

#define MAP_READ 0x0001 //Allows the user to read the buffer via mapping the buffer. Without this flag, attempting to map the buffer for reading will fail.
#define MAP_WRITE 0x0002 //Allows the user to map the buffer for writing. Without this flag, attempting to map the buffer for writing will fail.
#define MAP_READ_WRITE  (MAP_READ | MAP_WRITE) //Allows the user to read and write the buffer via mapping the buffer. 
#define READ_WRITE  0x0100 //Allows the user to modify the contents of the storage with *glBufferSubData*. Without this flag, attempting to call that function on this buffer will fail.
#define MAP_PERSISTENT  0x0040 //Allows the buffer object to be mapped in such a way that it can be used while it is mapped. Without this flag, attempting to perform any operation on the buffer while it is mapped will fail. You must use one of the mapping bits when using this bit.
#define MAP_COHERENT  0x0080 //Allows reads from and writes to a persistent buffer to be coherent with OpenGL, without an explicit barrier. Without this flag, you must use an explicit barrier to achieve coherency. You must use GL_PERSISTENT_BIT when using this bit.
#define CLIENT_STORAGE  0x0200 //A hint that suggests to the implementation that the storage for the buffer should come from "client" memory.

/*
* Buffer Objects are OpenGL Objects that store an array of unformatted memory allocated by the OpenGL context (AKA the GPU). 
* These can be used to store vertex data, pixel data retrieved from images or the framebuffer, and a variety of other things.
*/
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
	NOWORK_API static BufferObjectPtr Create(unsigned int usageType = READ_WRITE, const T* data = nullptr)
	{
		return Create(usageType, data, sizeof(T));
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

	template<typename T>
	T* Map();

	void Unmap();

protected:
	NOWORK_API BufferObject();

	NOWORK_API virtual void DoAsyncWork(int mode, void *params) override;


private:
	NOWORK_API static BufferObjectPtr Create(unsigned int usageType, const void* data, size_t size);

	GLuint m_UBO;
	size_t m_DataLength;
	unsigned int m_Usage;
	bool m_Mapped;
};



template<typename T>
NOWORK_API void BufferObject::Update(const T* data)
{
	glNamedBufferSubData(m_UBO, 0, sizeof(T), data);
}

template<typename T>
T* BufferObject::Map()
{
	m_Mapped = true;

	GLenum usage = GL_READ_ONLY;
	if (m_Usage & MAP_WRITE && !(m_Usage & MAP_READ))
		usage = GL_WRITE_ONLY;
	else 
		usage = GL_READ_WRITE;
	return static_cast<T*>(glMapNamedBuffer(m_UBO, usage));
}
