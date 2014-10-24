#pragma once
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

//=======================INCLUDES==================================================
#include <crtdbg.h>
#include <assert.h>
#include <new>
#include "Mutex.h"
//=================================================================================

//====================================FORWARD DECLARE========================================
class AABB2;
//===========================================================================================

namespace gh
{
	//====================================FORWARD DECLARE========================================
	class MatrixStack;
	//===========================================================================================

	//=================================MEMORY BLOCK================================
	class MemoryBlock
	{
	public:
		MemoryBlock* m_prevMemoryBlock;
		MemoryBlock* m_nextMemoryBlock;
		const char* m_file;
		int m_line;
		void* m_dataPtr;
		size_t m_dataSize;

		void initializeMemoryBlock( size_t dataByteSize, const char* file, int line, 
			MemoryBlock* prevMemBlock, MemoryBlock* nextMemBlock );
	};
	//==============================================================================


	//===============================MEMORY MANAGER======================================
	class MemoryManager
	{
	public:
		static MemoryManager& getInstance();
		static void dumpMemoryLeaks();
		static void dumpStatisticalAnalysisInformation();
		static void renderVisualQuad( AABB2& renderQuad, MatrixStack& currentMatrixStack );
		void* allocateMemory( size_t numByte, const char* fileName, int lineNumber );
		void deallocate( void* addressToRelease, const char* fileName, int lineNum );
		~MemoryManager();

	private:
		static MemoryManager* s_instanceMemManager;
		void* m_memPoolStart;
		void* m_memPoolEnd;
		MemoryBlock* m_usedMemoryListStart;
		MemoryBlock* m_usedMemoryListEnd;
		unsigned long m_numberOfAllocations;
		size_t m_largestAllocation;
		unsigned long m_memoryAccumulatedOverLifeOfProgram;
		Mutex m_mutex;

		MemoryManager();
		MemoryManager( const MemoryManager& other );
		MemoryManager& operator=( const MemoryManager& other );

		static void CreateMemoryManager();
		void removeMemoryAllocation( void* addressOfBlockToRemove );
		void* lookForBestFit( size_t numByte, const char* fileName, int lineNum );
		void dumpMemoryLeaksInternal();
		void dumpStatisticalAnalysisInternal();
		void visualizeMemoryPool( AABB2& renderQuad, MatrixStack& currentMatrixStack );
	};
	//===================================================================================
}

extern void* operator new( size_t numBytes, char* fileName, int lineNum );
extern void* operator new[]( size_t numBytes, char* fileName, int lineNum );
extern void* operator new( size_t numBytes );
extern void* operator new[]( size_t numBytes );

extern void operator delete( void* addressOfMemoryToRelease, char* fileName, int lineNum );
extern void operator delete[]( void* addressOfMemoryToRelease, char* fileName, int lineNum );
extern void operator delete( void* addressOfMemoryToRelease );
extern void operator delete[]( void* addressOfMemoryToRelease );

#endif