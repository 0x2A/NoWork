#pragma once


#define NOWORK_VERSION	"1.7"

//Either export or import dll functions
#ifdef NOWORK_DLL
#ifdef _NOWORK_INTERNAL_
#define NOWORK_API   __declspec( dllexport ) 
#else
#define NOWORK_API   __declspec( dllimport ) 
#endif
#else
#define NOWORK_API	
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
#include <memory>

#include <NoWork/GL/gl3w.h>
//only include glfw3 if we are in NoWork context
#ifdef _NOWORK_INTERNAL_
#include <glfw/glfw3.h>
#endif


#include <NoWork/glm/glm.hpp>
#include <NoWork/glm/gtc/matrix_transform.hpp>
#include <NoWork/glm/gtx/transform.hpp>
#include <NoWork/glm/gtc/type_ptr.hpp>
#include <NoWork/glm/gtx/quaternion.hpp>
#include <NoWork/glm/gtx/string_cast.hpp>
#include <NoWork/glm/gtx/rotate_vector.hpp>

using namespace glm;

#define GRADTORAD(x) (( x * M_PI ) / 180.0f)
#define RADTOGRAD(x) (( x * 180.0f ) / M_PI)

template<class T>
inline void DelPtr(T* ptr)
{
	delete ptr;
	ptr = nullptr;
}

template<class T>
inline void DelArr(T* ptr)
{
	delete []ptr;
	ptr = nullptr;
}


