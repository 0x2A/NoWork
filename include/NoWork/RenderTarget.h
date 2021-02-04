#pragma once


#include "NoWork/Common.h"


class RenderTarget;
typedef std::shared_ptr<RenderTarget> RenderTargetPtr;
class RenderTarget
{
	friend class Framebuffer;

public:

	virtual glm::ivec2 GetSize() = 0;
	virtual unsigned int GetObjectId() = 0;
protected:


	virtual void AttachToFBO(unsigned int target, unsigned int type) = 0;
};
