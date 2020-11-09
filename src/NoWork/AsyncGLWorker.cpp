#include "NoWork/AsyncGLWorker.h"
#include "NoWork/Renderer.h"


void AsyncGLWorker::AddToGLQueue(int mode, void* params /*= nullptr*/)
{
	AsyncGLWork_t work;
	work.worker = shared_from_this();
	work.mode = mode;
	work.params = params;
	renderer->RegisterAsyncGLWork(work);
}

Renderer* AsyncGLWorker::renderer;
