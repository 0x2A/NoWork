#pragma once



class AsyncGLWorker
{
	friend class Renderer;

protected:
	void AddToGLQueue(Renderer* r, int mode, void* params = nullptr);

	virtual void DoAsyncWork(int mode, void *params) = 0;
};


struct AsyncGLWork_t
{
	AsyncGLWorker* worker;
	int mode;
	void *params;
};