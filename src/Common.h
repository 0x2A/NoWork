#pragma once


#define NOWORK_VERSION	"1.0"

//Either export or import dll functions
#ifdef _NOWORK_INTERNAL_
#define NOWORK_API   __declspec( dllexport ) 
#else
#define NOWORK_API   __declspec( dllimport ) 
#endif

#include <string>
#include <unordered_map>
#include <map>
#include <iostream>
#include <sstream>

//online include gl3w and glfw3 if we are in NoWork context
#ifdef _NOWORK_INTERNAL_
#include <GL/gl3w.h>
#include <glfw/glfw3.h>
#endif