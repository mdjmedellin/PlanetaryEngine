#ifndef INCLUDE_GL_H
#define INCLUDE_GL_H
#pragma once

#ifdef _WIN32
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX
#	include <Windows.h>
#endif

#include <glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#pragma comment (lib, "glew32.lib" )
#pragma comment (lib, "opengl32.lib" )
#pragma comment (lib, "glu32.lib" )

#endif