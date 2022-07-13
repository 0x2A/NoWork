#include "NoWork/BufferObject.h"
#include "NoWork/Framework.h"


struct createStuct
{
	unsigned int usageType;
	const void* data;
	size_t size;
};


BufferObject::BufferObject()
{}

 BufferObject::~BufferObject()
{
	 glDeleteBuffers(1, &m_UBO);
}


 BufferObjectPtr BufferObject::Create(unsigned int usageType, const void* data, size_t size)
{
	 BufferObjectPtr ub = BufferObjectPtr(new BufferObject);
	 ub->m_DataLength = size;
	 ub->m_Usage = usageType;
	 if (!NoWork::IsMainThread())
	 {
		 createStuct* t = new createStuct;
		 t->data = data;
		 t->size = size;
		 t->usageType = usageType;
		 ub->AddToGLQueue(0, t);
		 return ub;
	 }
	 
	 glCreateBuffers(1, &ub->m_UBO);
	 glNamedBufferStorage(ub->m_UBO, size, data, usageType);
	 return ub;
}


void BufferObject::DoAsyncWork(int mode, void *params)
{
	if (mode == 0)
	{
		createStuct* t = static_cast<createStuct*>(params);
		glCreateBuffers(1, &m_UBO);
		glNamedBufferStorage(m_UBO, t->size, t->data, t->usageType);
		delete t;
	}
}



void BufferObject::Bind(uint slot, BindType bindType)
{
	glBindBufferBase(bindType, slot, m_UBO);
}

void BufferObject::Unmap()
{
	if (!m_Mapped)
	{
		LOG_WARNING("Tried to unmap a not mapped bufferObject.");
		return;
	}

	glUnmapNamedBuffer(m_UBO);
	m_Mapped = false;
}
