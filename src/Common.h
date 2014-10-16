#pragma once


#define NOWORK_VERSION	"1.0"

//Either export or import dll functions
#ifdef _NOWORK_INTERNAL_
#define NOWORK_API   __declspec( dllexport ) 
#else
#define NOWORK_API   __declspec( dllimport ) 
#endif

#ifdef _WIN32
#define SHARED(x) \
	__pragma(data_seg(".shared"))\
	x; \
	__pragma(data_seg())
#else
//TODO
#endif

#include <string>
#include <unordered_map>
#include <map>
#include <iostream>
#include <sstream>
#include <thread>

//online include gl3w and glfw3 if we are in NoWork context
#ifdef _NOWORK_INTERNAL_
#include <GL/gl3w.h>
#include <glfw/glfw3.h>
#endif

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

template<class T>
void DelPtr(T* ptr)
{
	if (ptr)
	{
		delete(ptr);
	}
	ptr = NULL;
}