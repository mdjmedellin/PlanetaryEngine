#pragma once
#ifndef MEMORY_UTILITIES_H
#define MEMORY_UTILITIES_H

#include "MemoryManager.h"

#undef new
#define new new ( __FILE__, __LINE__)

#endif