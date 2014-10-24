// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <memory>
#include <tchar.h>
#include <cassert>
#include <string>

// TODO: reference additional headers your program requires here
#include <glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

#include <zlib.h>
#pragma comment (lib, "zdll.lib")