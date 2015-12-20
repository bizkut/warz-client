//=========================================================================
//	Module: r3dArenaAllocator.cpp
//	Created by Roman E. Marchenko <roman.marchenko@gmail.com>
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"

#ifdef USE_R3D_MEMORY_ALLOCATOR

#include "r3dArenaAllocator.h"
#include "r3dAssert.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////////////

#ifndef ENABLE_ADDITIONAL_VALIDATION
#pragma optimize("tpg", on)
#endif

//////////////////////////////////////////////////////////////////////////

namespace
{
	inline size_t NextPow2(size_t n)
	{
		n--;
		n |= n >> 1;  
		n |= n >> 2;  
		n |= n >> 4;
		n |= n >> 8;
		n |= n >> 16;
		n++;
		return n;
	}

//////////////////////////////////////////////////////////////////////////

	__declspec (naked) size_t uintLog2(size_t val)
	{
		// prepare stack
		__asm
		{
			push ebp
			mov ebp, esp
			sub esp, __LOCAL_SIZE
		}

		__asm
		{
			mov eax,[val]
			bsr eax, eax
		}

		// Cleanup function
		__asm  
		{
			mov esp, ebp
			pop ebp
			ret
		}
	}
}

//-------------------------------------------------------------------------
//	Class MemoryArena
//-------------------------------------------------------------------------

MemoryArena::MemoryArena()
: pages(0)
, anchor(-1)
, lastUsed(-1)
{
	ZeroMemory(pageTable, _countof(pageTable) * sizeof(pageTable[0]));
	ZeroMemory(freeListTable, _countof(freeListTable) * sizeof(freeListTable[0]));
	pages = VirtualAlloc(0, PAGE_SIZE * NUM_PAGES_IN_ARENA, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

//////////////////////////////////////////////////////////////////////////

MemoryArena::~MemoryArena()
{
	if (pages)
		VirtualFree(pages, 0, MEM_RELEASE);
}

//////////////////////////////////////////////////////////////////////////

void * MemoryArena::AllocateMemory(size_t size, size_t alignment)
{
	//	Cannot handle allocations more than page size
	if (size > PAGE_SIZE || !pages)
		return 0;

	if (size == 0)
		size = 1;

	//	Determine block size for this allocation
	size_t blockSize = (std::max)(alignment, NextPow2(size));
           blockSize = (std::max)(blockSize, 16u);
	
#ifndef FINAL_BUILD
	memStats.numAllocations++;
	memStats.totalAllocatedMemory += blockSize;
#endif

	if (blockSize == PAGE_SIZE)
	{
		void *mem = FullPageRequest();
#ifdef ENABLE_ADDITIONAL_VALIDATION
		MarkUnusedSpaceOfBlock(mem, size - sizeof(size_t), blockSize);
#endif
		return mem;
	}

	//	Check if there are available page with needed block size?
	int freeListIdx = uintLog2(blockSize) - 4;

	MemBlock *entry = freeListTable[freeListIdx];
	if (!entry)
	{
		entry = CommitNewPage(blockSize, freeListIdx);
		if (!entry)
			return 0;
	}

	//	Get block from free list and return it to application
	MemBlock *rv = 0;
	if (entry->count > 1)
	{
		rv = entry->nextFree;
		entry->nextFree = rv->nextFree;
		--entry->count;
	}
	else
	{
		rv = entry;
		MemBlock *& firstEntry = freeListTable[freeListIdx];
		if (firstEntry->nextEntry)
		{
			firstEntry->nextEntry->prevEntry = 0;
			firstEntry = firstEntry->nextEntry;
		}
		else
		{
			firstEntry = 0;
		}

		byte *pagesBase = reinterpret_cast<byte*>(pages);
		byte *addr = reinterpret_cast<byte*>(entry);

		int pageTableEntryIdx = (addr - pagesBase) / PAGE_SIZE;
		pageTable[pageTableEntryIdx] = freeListIdx;
	}

#ifdef ENABLE_ADDITIONAL_VALIDATION
	MarkUnusedSpaceOfBlock(rv, size - sizeof(size_t), blockSize);

	ValidateEntireArena();
#endif
	return rv;
}

//////////////////////////////////////////////////////////////////////////

void MemoryArena::MarkUnusedSpaceOfBlock(void *mem, size_t memSize, size_t blockSize)
{
	byte *rvBytes = reinterpret_cast<byte*>(mem);
	memset(rvBytes + memSize, 0xcd, blockSize - memSize);
}

//////////////////////////////////////////////////////////////////////////

MemoryArena::MemBlock * MemoryArena::CommitNewPage(size_t blockSize, int freeListIdx)
{
	//	Check anchor first
	ptrdiff_t pageTableIdx = GetFreePage();
	//	Out of memory
	if (pageTableIdx == -1)
		return 0;

	byte *pagesBytePtr = reinterpret_cast<byte*>(pages);

	byte *newPage = pagesBytePtr + pageTableIdx * PAGE_SIZE;
	byte *memBlocks = newPage;

	size_t numBlocks = PAGE_SIZE / blockSize;

	//	Now create single linked list of page blocks
	MemBlock *nextBlock = 0;
	for (size_t i = 0; i < numBlocks - 1; ++i)
	{
		MemBlock * mb = reinterpret_cast<MemBlock*>(memBlocks);
		nextBlock = reinterpret_cast<MemBlock*>(memBlocks + blockSize);
		mb->nextFree = nextBlock;
		memBlocks += blockSize;
	}
	nextBlock->nextFree = 0;

	//	First list should contain free blocks count
	MemBlock * first = reinterpret_cast<MemBlock*>(newPage);
	first->count = numBlocks;

	//	Adjust page table
	pageTable[pageTableIdx] = reinterpret_cast<ptrdiff_t>(newPage) | freeListIdx;

	//	Add entry block to double linked list of entry blocks
	MemBlock *&oldHead = freeListTable[freeListIdx];
	if (!oldHead)
	{
		oldHead = first;
		first->nextEntry = 0;
		first->prevEntry = 0;
	}
	else
	{
		oldHead->prevEntry = first;
		first->nextEntry = oldHead;
		first->prevEntry = 0;
	}

	return first;
}

//////////////////////////////////////////////////////////////////////////

bool MemoryArena::FreeMemory(void *mem)
{
	byte *byteMem = reinterpret_cast<byte*>(mem);
	byte *bytePages = reinterpret_cast<byte*>(pages);

	if (pages == 0)
		return false;

	//	If memory is outside pages area, assume that memory from other pool (or event it is CRT mem)
	if (byteMem < bytePages || byteMem > bytePages + PAGE_SIZE * NUM_PAGES_IN_ARENA)
		return false;

	MemBlock *newFreeBlock = reinterpret_cast<MemBlock*>(mem);

	//	Calculate page table index
	ptrdiff_t pageTableIdx = (byteMem - bytePages) / PAGE_SIZE;
	ptrdiff_t pageTableEntry = pageTable[pageTableIdx];

	size_t freeListIdx = pageTableEntry & 0xf;
	size_t blockSize = 16u << freeListIdx;
	size_t numBlocks = PAGE_SIZE / blockSize;

#ifndef FINAL_BUILD
	memStats.numDeallocations++;
	memStats.totalAllocatedMemory -= blockSize;
#endif

#ifdef ENABLE_ADDITIONAL_VALIDATION

	r3d_assert(reinterpret_cast<ptrdiff_t>(byteMem) % blockSize == 0);

	size_t *st = reinterpret_cast<size_t*>(byteMem + blockSize - sizeof(size_t));
	r3d_assert(*st == 0xcdcdcdcd);

	memset(mem, 0xba, blockSize);
#endif

	//	Handle full page requests
	if (blockSize == PAGE_SIZE)
	{
		MarkPageAsFree(pageTableIdx);
		return true;
	}

#ifdef ENABLE_ADDITIONAL_VALIDATION
	ValidateFreeOperation(newFreeBlock, pageTableIdx);
#endif

	const size_t ADDR_MASK = static_cast<size_t>(-1) ^ 0xf;
	MemBlock *entry = reinterpret_cast<MemBlock*>(pageTableEntry & ADDR_MASK);

	if (entry != 0)
	{
		//	Insert block into free list
		newFreeBlock->nextFree = entry->nextFree;
		entry->nextFree = newFreeBlock;
		++entry->count;

		//	If whole page is free, mark it for reuse
		if (numBlocks == entry->count)
		{
			//	Remove entry block from free list
			if (entry == freeListTable[freeListIdx])
			{
				MemBlock * newHead = entry->nextEntry;
				freeListTable[freeListIdx] = newHead;
				if (newHead)
					newHead->prevEntry = 0;
			}
			else
			{
				if (entry->nextEntry)
					entry->nextEntry->prevEntry = entry->prevEntry;
				if (entry->prevEntry)
					entry->prevEntry->nextEntry = entry->nextEntry;
			}
			MarkPageAsFree(pageTableIdx);
		}
	}
	else
	{
		//	Mark this block as entry block
		newFreeBlock->nextFree = 0;
		newFreeBlock->count = 1;

		MemBlock *&entries = freeListTable[freeListIdx];
		if (!entries)
		{
			entries = newFreeBlock;
			newFreeBlock->nextEntry = 0;
			newFreeBlock->prevEntry = 0;
		}
		else
		{
			newFreeBlock->nextEntry = entries->nextEntry;
			newFreeBlock->prevEntry = entries;

			if (entries->nextEntry)
				entries->nextEntry->prevEntry = newFreeBlock;

			entries->nextEntry = newFreeBlock;
		}

		//	Add address into page table entry
		pageTable[pageTableIdx] = reinterpret_cast<size_t>(newFreeBlock) | freeListIdx;
	}
#ifdef ENABLE_ADDITIONAL_VALIDATION
	ValidateEntireArena();
#endif
	return true;
}

//////////////////////////////////////////////////////////////////////////

void MemoryArena::MarkPageAsFree(ptrdiff_t pageTableIdx)
{
	if (lastUsed == pageTableIdx)
	{
		--lastUsed;
	}
	else
	{
		pageTable[pageTableIdx] = anchor;
		anchor = pageTableIdx;
	}
}

//////////////////////////////////////////////////////////////////////////

MemoryArena::MemBlock * MemoryArena::FullPageRequest()
{
	ptrdiff_t pageTableIdx = GetFreePage();
	//	Out of memory
	if (pageTableIdx == -1)
		return 0;

	pageTable[pageTableIdx] = uintLog2(PAGE_SIZE) - 4;

	byte *pagesBytePtr = reinterpret_cast<byte*>(pages);
	byte *newPage = pagesBytePtr + pageTableIdx * PAGE_SIZE;

	MemBlock *mb = reinterpret_cast<MemBlock*>(newPage);
	return mb;
}

//////////////////////////////////////////////////////////////////////////

ptrdiff_t MemoryArena::GetFreePage()
{
	ptrdiff_t pageTableIdx = -1;
	if (anchor < 0)
	{
		if (lastUsed < NUM_PAGES_IN_ARENA - 1)
		{
			pageTableIdx = ++lastUsed;
		}
	}
	else
	{
		//	Relink anchor
		pageTableIdx = anchor;
		anchor = pageTable[anchor];
	}
	return pageTableIdx;
}

//////////////////////////////////////////////////////////////////////////

void MemoryArena::ValidateFreeOperation(MemBlock *toDel, ptrdiff_t pageTableIdx)
{
	ptrdiff_t pageTableEntry = pageTable[pageTableIdx];
	
	//	Check if page for block is not free already
// 	ptrdiff_t curAnchor = anchor;
// 	while (curAnchor != -1)
// 	{
// 		r3d_assert(pageTable[curAnchor] != pageTableEntry);
// 		curAnchor = pageTable[curAnchor];
// 	}

	r3d_assert(pageTableIdx <= lastUsed);

	//	Now check free lists for this block
	const size_t ADDR_MASK = static_cast<size_t>(-1) ^ 0xf;
	MemBlock *entry = reinterpret_cast<MemBlock*>(pageTableEntry & ADDR_MASK);

	while (entry)
	{
		r3d_assert(entry != toDel);
		entry = entry->nextFree;
	}
}

//////////////////////////////////////////////////////////////////////////

void MemoryArena::ValidateEntireArena()
{
	byte *pagesStart = reinterpret_cast<byte*>(pages);
	byte *pagesEnd = reinterpret_cast<byte*>(pages) + PAGE_SIZE * NUM_PAGES_IN_ARENA;

	//	Check free list
	for (int i = 0; i < _countof(freeListTable); ++i)
	{
		MemBlock *b = freeListTable[i];
		while (b)
		{
			byte *bytePtr = reinterpret_cast<byte*>(b);
			r3d_assert(pagesStart <= bytePtr && bytePtr < pagesEnd);
			b = b->nextEntry;
		}
	}

	//	Check pageTable
	const size_t ADDR_MASK = static_cast<size_t>(-1) ^ 0xf;
	for (int i = 0; i < NUM_PAGES_IN_ARENA; ++i)
	{
		ptrdiff_t pte = pageTable[i];
		MemBlock *entry = reinterpret_cast<MemBlock*>(pte & ADDR_MASK);
		byte *entryByte = reinterpret_cast<byte*>(entry);
		if (entryByte >= pagesStart && entryByte < pagesEnd)
		{
			byte *pageStart = reinterpret_cast<byte*>(pte & ~0xfff);
			byte *pageEnd = pageStart + PAGE_SIZE;

			while (entry)
			{
				entryByte = reinterpret_cast<byte*>(entry);
				r3d_assert(entryByte >= pageStart && entryByte < pageEnd);
				entry = entry->nextFree;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

const r3dAllocatorMemoryStats & MemoryArena::GetMemStats() const
{
	return memStats;
}

//-------------------------------------------------------------------------
//	class r3dArenaAllocator
//-------------------------------------------------------------------------

r3dArenaAllocator::r3dArenaAllocator(CRITICAL_SECTION *cs)
: lastArenaIdx(0)
, guard(cs)
{
	::ZeroMemory(arenas, MAX_NUM_ARENAS * sizeof(void*));
}

//////////////////////////////////////////////////////////////////////////

r3dArenaAllocator::~r3dArenaAllocator()
{
	for (int i = 0; i < MAX_NUM_ARENAS; ++i)
	{
		MemoryArena *ma = arenas[i];
		free(ma);
	}
}

//////////////////////////////////////////////////////////////////////////

MemoryArena * r3dArenaAllocator::CreateNewArena()
{
	void *mem = malloc(sizeof(MemoryArena));
	MemoryArena *ma = new (mem) MemoryArena;
	arenas[lastArenaIdx++] = ma;
	return ma;
}

//////////////////////////////////////////////////////////////////////////

void * r3dArenaAllocator::Allocate(size_t size, size_t alignment)
{
	CSHolder cs(guard);

#ifdef ENABLE_ADDITIONAL_VALIDATION
	size += sizeof(size_t); // for guard block
#endif

	//	Handle big requests separately
	if (size > PAGE_SIZE)
	{
		void *mem = _aligned_malloc(size, alignment);
#ifndef FINAL_BUILD
		if (mem)
		{
			memStats.numAllocations++;
			memStats.totalAllocatedMemory += _aligned_msize(mem, alignment, 0);;
		}
#endif
		return mem;
	}

	//	Search for arena than can handle request
	for (int i = lastArenaIdx - 1; i >= 0; --i)
	{
		MemoryArena *ma = arenas[i];

		if (ma && !ma->IsMemoryAvailable())
			return 0;

		void *mem = ma->AllocateMemory(size, alignment);
		if (mem)
			return mem;
	}

	// Create new arena
	MemoryArena *ma = CreateNewArena();

	void *mem = ma->AllocateMemory(size, alignment);
	r3d_assert(mem);
	return mem;
}

//////////////////////////////////////////////////////////////////////////

bool r3dArenaAllocator::DeallocateOnlyOwnMemory(void *ptr)
{
	if (!ptr) return false;

	CSHolder cs(guard);

	//	Search for arena than can handle request
	for (int i = lastArenaIdx - 1; i >= 0; --i)
	{
		MemoryArena *ma = arenas[i];

		if (ma->FreeMemory(ptr))
			return true;
	}
	
	return false;
}

//////////////////////////////////////////////////////////////////////////

void r3dArenaAllocator::Deallocate(void *ptr, size_t alignment)
{
	if (!ptr) return;

	if (!DeallocateOnlyOwnMemory(ptr))
	{
#ifndef FINAL_BUILD
		memStats.numDeallocations++;
		memStats.totalAllocatedMemory -= _aligned_msize(ptr, alignment, 0);
#endif
		//	Deallocation failed, assume this block is allocated using _aligned_malloc
		_aligned_free(ptr);
	}
}

//////////////////////////////////////////////////////////////////////////

r3dAllocatorMemoryStats r3dArenaAllocator::GetMemStats() const
{
	r3dAllocatorMemoryStats rv = memStats;
	//	Search for arena than can handle request
	for (int i = lastArenaIdx - 1; i >= 0; --i)
	{
		const r3dAllocatorMemoryStats &arenaStats = arenas[i]->GetMemStats();

		rv.numAllocations += arenaStats.numAllocations;
		rv.numDeallocations += arenaStats.numDeallocations;
		rv.totalAllocatedMemory += arenaStats.totalAllocatedMemory;
	}
	return rv;
}

//////////////////////////////////////////////////////////////////////////

#ifndef ENABLE_ADDITIONAL_VALIDATION
#pragma optimize("", on)
#endif

#endif // USE_R3D_MEMORY_ALLOCATOR