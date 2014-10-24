#include <stdio.h>
#include "MemoryManager.h"
#include "MutexLock.h"
#include "DebugUtility.h"
#include "AABB2.hpp"
#include "Renderer.h"
#include "Rgba.h"
#include "Vector4.h"

namespace
{
	const size_t SIZE_OF_MEMORY_POOL = 500 * 1024 * 1024;	//500 MB

	const float INVERSE_SIZE_OF_MEMORY = 1.f / SIZE_OF_MEMORY_POOL;
}

namespace gh
{
	MemoryManager& MemoryManager::getInstance()
	{
		int x = 1;
		if( !s_instanceMemManager )
		{
			CreateMemoryManager();
		}

		return *s_instanceMemManager;
	}

	void* MemoryManager::allocateMemory( size_t numByte, const char* fileName, int lineNumber )
	{
		MutexLock functionMutexLock( m_mutex );

		void* allocationAddress = nullptr;

		++m_numberOfAllocations;
		m_memoryAccumulatedOverLifeOfProgram += numByte;

		if( numByte > m_largestAllocation )
		{
			m_largestAllocation = numByte;
		}

		if( m_usedMemoryListStart )
		{
			//look for best fit open space
			allocationAddress = lookForBestFit( numByte, fileName, lineNumber );
		}
		else
		{
			//no memory has been allocated
			//create a new memory block
			m_usedMemoryListStart = static_cast< MemoryBlock* >( m_memPoolStart );
			m_usedMemoryListStart->initializeMemoryBlock( numByte
				, fileName
				, lineNumber
				, nullptr
				, nullptr );
			allocationAddress = m_usedMemoryListStart->m_dataPtr;
		}

		return allocationAddress;
	}

	void MemoryManager::deallocate( void* addressToRelease, const char* fileName, int lineNum )
	{
		char* byteAddress = ( char* )( addressToRelease ) - sizeof( MemoryBlock );
		removeMemoryAllocation( byteAddress );
	}

	MemoryManager::MemoryManager()
		:	m_memPoolStart( nullptr )
		,	m_memPoolEnd( nullptr )
		,	m_usedMemoryListStart( nullptr )
		,	m_usedMemoryListEnd( nullptr )
		,	m_numberOfAllocations( 0 )
		,	m_largestAllocation( 0 )
		,	m_memoryAccumulatedOverLifeOfProgram( 0 )
		,	m_mutex( Mutex() )
	{
		//allocate the memory pool that we are going to use
		m_memPoolStart = malloc( SIZE_OF_MEMORY_POOL );
		assert( m_memPoolStart );
		m_memPoolEnd = static_cast< char* >( m_memPoolStart ) + SIZE_OF_MEMORY_POOL;
	}

	void MemoryManager::CreateMemoryManager()
	{
		void* memoryManagerAddress = malloc( sizeof( MemoryManager ) );
		assert( memoryManagerAddress );

		s_instanceMemManager = new ( memoryManagerAddress ) MemoryManager();
	}

	void MemoryManager::removeMemoryAllocation( void* addressOfBlockToRemove )
	{
		MutexLock functionMutexLock( m_mutex );

		if( m_usedMemoryListStart )
		{
			MemoryBlock* currentMemoryBlock = m_usedMemoryListStart;

			while( currentMemoryBlock )
			{
				if( static_cast< char* >( ( void*)( currentMemoryBlock ) ) > (char*)( addressOfBlockToRemove) )
				{
					//this means that we have not found the block of memory the user desires to delete
					int x = 1;
					int y = 2;
					break;
				}
				else if( addressOfBlockToRemove == (void*)( currentMemoryBlock ) )
				{
					MemoryBlock* previousBlock = currentMemoryBlock->m_prevMemoryBlock;
					MemoryBlock* nextBlock = currentMemoryBlock->m_nextMemoryBlock;

					if( previousBlock )
					{
						previousBlock->m_nextMemoryBlock = nextBlock;
					}
					else
					{
						m_usedMemoryListStart = nextBlock;
					}

					if( nextBlock )
					{
						nextBlock->m_prevMemoryBlock = previousBlock;
					}

					break;
				}
				else
				{
					currentMemoryBlock = currentMemoryBlock->m_nextMemoryBlock;
				}
			}
		}
	}

	void* MemoryManager::lookForBestFit( size_t numByte, const char* fileName, int lineNum )
	{
		assert( m_usedMemoryListStart );

		size_t memoryRequestSize = numByte + sizeof( MemoryBlock );

		//start from the beginning of the list and move down
		size_t memorySpan = 0;
		void* newMemoryBlockStart = nullptr;

		char* startOfCurrentMemoryBlock = ( char* )( m_usedMemoryListStart );
		char* startOfNextMemoryBlock = nullptr;

		MemoryBlock* currentMemoryBlock = m_usedMemoryListStart;
		MemoryBlock* nextMemoryBlock = m_usedMemoryListStart->m_nextMemoryBlock;

		MemoryBlock* next = nullptr;
		MemoryBlock* prev = nullptr;

		memorySpan = startOfCurrentMemoryBlock - ( char* )( m_memPoolStart );
		if( memorySpan > memoryRequestSize )
		{
			newMemoryBlockStart = m_memPoolStart;
			next = currentMemoryBlock;


			currentMemoryBlock->m_prevMemoryBlock = static_cast< MemoryBlock* >( newMemoryBlockStart);
			currentMemoryBlock->m_prevMemoryBlock->initializeMemoryBlock( numByte
				, fileName
				, lineNum
				, prev
				, next );


			m_usedMemoryListStart = currentMemoryBlock->m_prevMemoryBlock;
			currentMemoryBlock = m_usedMemoryListStart;
		}
		else
		{
			bool continueLooking = true;

			while( continueLooking )
			{
				if( nextMemoryBlock != nullptr )
				{
					startOfNextMemoryBlock = ( char* )( static_cast< void* >( nextMemoryBlock ) );

					memorySpan = startOfNextMemoryBlock - startOfCurrentMemoryBlock;
					memorySpan -= sizeof( MemoryBlock );
					memorySpan -= currentMemoryBlock->m_dataSize;

					if( memorySpan > memoryRequestSize )
					{
						// Rearrange the linked list
						newMemoryBlockStart = static_cast< void* >( startOfCurrentMemoryBlock 
							+ sizeof( MemoryBlock ) 
							+ currentMemoryBlock->m_dataSize );

						next = currentMemoryBlock->m_nextMemoryBlock;
						prev = nextMemoryBlock->m_prevMemoryBlock;


						currentMemoryBlock->m_nextMemoryBlock = static_cast< MemoryBlock* >( newMemoryBlockStart );
						currentMemoryBlock->m_nextMemoryBlock->initializeMemoryBlock( numByte
							, fileName
							, lineNum
							, prev
							, next );

						nextMemoryBlock->m_prevMemoryBlock = currentMemoryBlock->m_nextMemoryBlock;

						continueLooking = false;
					}
				}
				else
				{
					newMemoryBlockStart = ( void* )( startOfCurrentMemoryBlock + sizeof( MemoryBlock ) 
						+ currentMemoryBlock->m_dataSize );

					if( static_cast< char* >( m_memPoolEnd ) - static_cast< char* >( newMemoryBlockStart ) > sizeof( MemoryBlock ) + numByte )
					{
						prev = currentMemoryBlock;


						currentMemoryBlock->m_nextMemoryBlock = static_cast< MemoryBlock* >( newMemoryBlockStart );
						currentMemoryBlock->m_nextMemoryBlock->initializeMemoryBlock( numByte
							, fileName
							, lineNum
							, prev
							, nullptr );

						continueLooking = false;
					}
				}

				//advance to the next node in the list
				currentMemoryBlock = currentMemoryBlock->m_nextMemoryBlock;
				startOfCurrentMemoryBlock = ( char* )( static_cast< void* >( currentMemoryBlock ) );
				nextMemoryBlock = currentMemoryBlock->m_nextMemoryBlock;
			}
		}

		//at this point we should have the new allocated memory block in the currentMemoryBlock variable
		void* returningMemoryAddress = ( currentMemoryBlock->m_dataPtr );
		return returningMemoryAddress;
	}

	void MemoryManager::dumpMemoryLeaksInternal()
	{
		MutexLock functionMutexLock( m_mutex );

		MemoryBlock* currentMemoryBlock = m_usedMemoryListStart;

		char memoryLeakOutputLine[ 1024 ];

		sprintf_s( memoryLeakOutputLine, "MEMORY LEAKS\n" ); 
		traceString( memoryLeakOutputLine );

		while( currentMemoryBlock )
		{

			sprintf_s( memoryLeakOutputLine, ">\t%s(%i): \tsize = %i bytes\n", 
				currentMemoryBlock->m_file,
				currentMemoryBlock->m_line,
				currentMemoryBlock->m_dataSize );

			traceString( memoryLeakOutputLine );
			
			currentMemoryBlock = currentMemoryBlock->m_nextMemoryBlock;
		}
	}

	void MemoryManager::dumpMemoryLeaks()
	{
		getInstance().dumpMemoryLeaksInternal();
	}

	void MemoryManager::dumpStatisticalAnalysisInformation()
	{
		getInstance().dumpStatisticalAnalysisInternal();
	}

	void MemoryManager::dumpStatisticalAnalysisInternal()
	{
		MutexLock functionMutexLock( m_mutex );

		char statisticalOutputLine[1024];

		sprintf_s( statisticalOutputLine,
			"MEMORY STATISTICAL ANALYSIS\nLargest Allocation = %i bytes\nNumber of Allocations = %lu\nTotal Memory Allocated Over the Course of the Program = %lu\nAverage Size of Memory Allocation = %f\n",
			m_largestAllocation,
			m_numberOfAllocations,
			m_memoryAccumulatedOverLifeOfProgram,
			( m_memoryAccumulatedOverLifeOfProgram / ( double )( m_numberOfAllocations ) ) );
		
		traceString( statisticalOutputLine );
	}

	MemoryManager::~MemoryManager()
	{
		dumpStatisticalAnalysisInformation();
	}

	void MemoryManager::renderVisualQuad( AABB2& renderQuad, MatrixStack& currentMatrixStack )
	{
		getInstance().visualizeMemoryPool( renderQuad, currentMatrixStack );
	}

	void MemoryManager::visualizeMemoryPool( AABB2& renderQuad, MatrixStack& currentMatrixStack )
	{
		float lastHeightOffset = 0.f;
		Vector2 widthAndHeight = renderQuad.getWidthAndHeight();

		Vector2 mins;
		Vector2 maxs;
		AABB2 memoryQuad;

		MutexLock functionMutexLock( m_mutex );

		char* previouslyOccupiedLocationEnd = (char*)m_memPoolStart;

		MemoryBlock* currentMemoryBlock = m_usedMemoryListStart;

		if( currentMemoryBlock )
		{
			char* currentMemoryBlockLocation;
			char* lastOccupatedSectionStart = static_cast< char* >( ( void* )( m_usedMemoryListStart ) );


			float occupiedBlockSize = 0.f;
			float freeBlockSize = 0.f;
			float scaledSize = 0.f;


			while( currentMemoryBlock )
			{
				currentMemoryBlockLocation = static_cast< char* >( ( void* )( currentMemoryBlock ) );

				if( currentMemoryBlockLocation != previouslyOccupiedLocationEnd )
				{
					//at this point we know there is a gap

					//calculate the height of the occupied memory
					occupiedBlockSize = previouslyOccupiedLocationEnd - lastOccupatedSectionStart;
					scaledSize = occupiedBlockSize * INVERSE_SIZE_OF_MEMORY;

					mins.SetXY( 0.f, lastHeightOffset );
					maxs.SetXY( widthAndHeight.x, widthAndHeight.y * scaledSize + lastHeightOffset );

					memoryQuad.setMins( mins );
					memoryQuad.setMaxs( maxs );

					theRenderer.renderQuad( memoryQuad, Rgba( Rgba::RED ).getVector4(), nullptr );

					lastHeightOffset = maxs.y;

					//calculate the height of the sliver of free memory
					freeBlockSize = currentMemoryBlockLocation - previouslyOccupiedLocationEnd;
					scaledSize = freeBlockSize * INVERSE_SIZE_OF_MEMORY;

					mins.SetXY( 0.f, lastHeightOffset );
					maxs.SetXY( widthAndHeight.x, widthAndHeight.y * scaledSize + lastHeightOffset );

					memoryQuad.setMins( mins );
					memoryQuad.setMaxs( maxs );

					theRenderer.renderQuad( memoryQuad, Rgba( Rgba::GREEN ).getVector4(), nullptr );

					lastHeightOffset = maxs.y;

					lastOccupatedSectionStart = currentMemoryBlockLocation;
				}

				previouslyOccupiedLocationEnd = currentMemoryBlockLocation + sizeof( MemoryBlock ) 
					+ currentMemoryBlock->m_dataSize;

				currentMemoryBlock = currentMemoryBlock->m_nextMemoryBlock;
			}
		}

		//at this point the rest of the memory is free
		mins.SetXY( 0.f, lastHeightOffset );
		maxs.SetXY( widthAndHeight.x, widthAndHeight.y );

		memoryQuad.setMins( mins );
		memoryQuad.setMaxs( maxs );

		theRenderer.renderQuad( memoryQuad, Rgba( Rgba::GREEN ).getVector4(), nullptr );
	}

	MemoryManager* MemoryManager::s_instanceMemManager;

	void MemoryBlock::initializeMemoryBlock( size_t dataByteSize, const char* file, int line, 
		MemoryBlock* prevMemBlock, MemoryBlock* nextMemBlock )
	{
		m_file = file;
		m_line = line;
		m_dataSize = dataByteSize;
		m_prevMemoryBlock = prevMemBlock;
		m_nextMemoryBlock = nextMemBlock;
		m_dataPtr = ( static_cast< char* >( ( void* )( this ) ) + sizeof( MemoryBlock ) );
	}
}


void* operator new( size_t numBytes, char* fileName, int lineNum )
{
	return gh::MemoryManager::getInstance().allocateMemory( numBytes, fileName, lineNum );
}

void* operator new( size_t numBytes )
{
	return gh::MemoryManager::getInstance().allocateMemory( numBytes, nullptr, 0 );
}

void* operator new[]( size_t numBytes, char* fileName, int lineNum )
{
	return gh::MemoryManager::getInstance().allocateMemory( numBytes, fileName, lineNum );
}

void* operator new[]( size_t numBytes )
{
	return gh::MemoryManager::getInstance().allocateMemory( numBytes, nullptr, 0 );
}


void operator delete( void* addressOfMemoryToRelease, char* fileName, int lineNum )
{
	gh::MemoryManager::getInstance().deallocate( addressOfMemoryToRelease, fileName, lineNum );
}

void operator delete( void* addressOfMemoryToRelease )
{
	gh::MemoryManager::getInstance().deallocate( addressOfMemoryToRelease, nullptr, 0 );
}

void operator delete[]( void* addressOfMemoryToRelease, char* fileName, int lineNum )
{
	gh::MemoryManager::getInstance().deallocate( addressOfMemoryToRelease, fileName, lineNum );
}

void operator delete[]( void* addressOfMemoryToRelease )
{
	gh::MemoryManager::getInstance().deallocate( addressOfMemoryToRelease, nullptr, 0 );
}