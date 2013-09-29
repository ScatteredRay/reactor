#ifndef _GL_ALL_H_
#define _GL_ALL_H_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <GL\gl.h>
#include "glext.h"

#define GL_DECL(fun, decl) extern decl fun;
#include "glext_loader.inc"
#undef GL_DECL

#else
#include <gl.h>
#endif

#endif //_GL_ALL_H_
