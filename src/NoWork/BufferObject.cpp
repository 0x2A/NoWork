#include "NoWork/BufferObject.h"
#include "NoWork/Framework.h"


struct createStuct
{
	const void* data;
	size_t size;
};


BufferObject::BufferObject()
{}

 BufferObject::~BufferObject()
{
	 glDeleteBuffers(1, &m_UBO);
}


 UniformbufferPtr BufferObject::Create(const void* data, size_t size)
{
	 UniformbufferPtr ub = UniformbufferPtr(new BufferObject);
	 if (!NoWork::IsMainThread())
	 {
		 createStuct* t = new createStuct;
		 t->data = data;
		 t->size = size;
		 ub->AddToGLQueue(0, t);
		 return ub;
	 }
	 glCreateBuffers(1, &ub->m_UBO);
	 glNamedBufferStorage(ub->m_UBO, size, data, GL_DYNAMIC_STORAGE_BIT);
	 return ub;
}

void BufferObject::CreateBuffer(const void* data, size_t size)
{
	
}

void BufferObject::DoAsyncWork(int mode, void *params)
{
	if (mode == 0)
	{
		createStuct* t = static_cast<createStuct*>(params);
		glCreateBuffers(1, &m_UBO);
		glNamedBufferStorage(m_UBO, t->size, t->data, GL_DYNAMIC_STORAGE_BIT);
		delete t;
	}
}



void BufferObject::Bind(uint slot, BindType bindType)
{
	glBindBufferBase(bindType, slot, m_UBO);
}
