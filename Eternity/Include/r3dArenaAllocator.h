//=========================================================================
//	Module: r3dArenaAllocator.h
//	Created by Roman E. Marchenko <roman.marchenko@gmail.com>
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

#ifdef USE_R3D_MEMORY_ALLOCATOR

#include "windows.h"

//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
//#define ENABLE_ADDITIONAL_VALIDATION
#endif

//////////////////////////////////////////////////////////////////////////

class CSHolder
{
public:
	CRITICAL_SECTION * cs;
	CSHolder(CRITICAL_SECTION * in_cs) : cs(in_cs)
	{
		EnterCriticalSection(cs);
	}
	~CSHolder()
	{
		LeaveCriticalSection(cs);
	}
};

//////////////////////////////////////////////////////////////////////////

const int PAGE_SIZE = 0x1000;
const int NUM_PAGES_IN_ARENA = 2500;

class MemoryArena
{
	struct MemBlock
	{
		/**	Data for entry blocks double-linked list */
		MemBlock *nextEntry;
		MemBlock *prevEntry;
		int count;

		/** Single linked list of free blocks. */
		MemBlock *nextFree;
	};

	/**	Memory arena address always aligned on page boundary (4096). */
	void *pages;

	ptrdiff_t pageTable[NUM_PAGES_IN_ARENA];
	MemBlock *freeListTable[8];

	ptrdiff_t anchor, lastUsed;

	r3dAllocatorMemoryStats memStats;

	/**	Commit new page, and return entry address. */
	MemBlock * CommitNewPage(size_t blockSize, int freeListIdx);

	/**	Special code path for full page requests. */
	MemBlock * FullPageRequest();

	/**	Get new free page. Return page table index. */
	ptrdiff_t GetFreePage();

	/**	Mark page as free. */
	void MarkPageAsFree(ptrdiff_t pageTableIdx);

	void ValidateFreeOperation(MemBlock *toDel, ptrdiff_t pageTableIdx);
	void ValidateEntireArena();
	void MarkUnusedSpaceOfBlock(void *mem, size_t memSize, size_t blockSize);

public:
	MemoryArena();
	~MemoryArena();

	void * AllocateMemory(size_t size, size_t alignment);
	bool FreeMemory(void *mem);
	inline bool IsMemoryAvailable() const { return pages != 0; }

	const r3dAllocatorMemoryStats & GetMemStats() const;
};

//////////////////////////////////////////////////////////////////////////

const int MAX_NUM_ARENAS = 20;

class r3dArenaAllocator
{
	/**	Multithreading guard. */
	CRITICAL_SECTION *guard;

	int lastArenaIdx;

	MemoryArena *arenas[MAX_NUM_ARENAS];

	r3dAllocatorMemoryStats memStats;

	MemoryArena * CreateNewArena();

public:
	r3dArenaAllocator(CRITICAL_SECTION *cs);
	~r3dArenaAllocator();

	void * Allocate(size_t size, size_t alignment = 1);
	/**	If memory not belong to any arena, use _aligned_free to deallocate it. */
	void Deallocate(void *ptr, size_t alignment);
	/**	Deallocate memory only if it belongs to some arena. REturn true if deallocation was succeeded. */
	bool DeallocateOnlyOwnMemory(void *ptr);

	/**	Get allocator memory statistics. */
	r3dAllocatorMemoryStats GetMemStats() const;
};

#endif //USE_R3D_MEMORY_ALLOCATOR

//////////////////////////////////////////////////////////////////////////
