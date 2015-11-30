#include "NoWork/AsyncGLWorker.h"
#include "NoWork/Renderer.h"


void AsyncGLWorker::AddToGLQueue(Renderer* r, int mode, void* params)
{
	AsyncGLWork_t work;
	work.worker = this;
	work.mode = mode;
	work.params = params;
	r->RegisterAsyncGLWork(work);
}
