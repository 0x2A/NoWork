#pragma once

#include <memory>



class AsyncGLWorker: public std::enable_shared_from_this<AsyncGLWorker>
{
	friend class Renderer;

protected:
	void AddToGLQueue(int mode, void* params = nullptr);

	virtual void DoAsyncWork(int mode, void *params) = 0;

	static Renderer* renderer;
};


struct AsyncGLWork_t
{
	std::shared_ptr<AsyncGLWorker> worker;
	int mode;
	void *params;
};