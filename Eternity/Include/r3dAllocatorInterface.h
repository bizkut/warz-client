//=========================================================================
//	Module: r3dAllocatorInterface.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

#ifdef USE_R3D_MEMORY_ALLOCATOR
#undef new

enum r3dAllocationTypes
{
	ALLOC_TYPE_GFX = 0,
	ALLOC_TYPE_GAME,
	ALLOC_TYPE_PHYSX,
	ALLOC_TYPE_SCALEFORM,
	ALLOC_TYPE_NAVIGATION,
	ALLOC_TYPE_TOTAL
};

//////////////////////////////////////////////////////////////////////////

struct r3dAllocatorMemoryStats
{
	size_t numAllocations;
	size_t numDeallocations;
	size_t totalAllocatedMemory;

	r3dAllocatorMemoryStats()
	: numAllocations(0), numDeallocations(0), totalAllocatedMemory(0)
	{}
};

//////////////////////////////////////////////////////////////////////////

void * r3dAllocateMemory(size_t size, unsigned int alignment, r3dAllocationTypes t);
void r3dDeallocateMemory(void *ptr, unsigned int alignment, r3dAllocationTypes t);
r3dAllocatorMemoryStats r3dGetAllocatorStats(r3dAllocationTypes t);

//	Pugi xml
void * r3dPugiAllocateMemory(size_t size);
void r3dPugiDeallocateMemory(void *ptr);

void * operator new(size_t size);
void operator delete(void *pMemory);
void * operator new[](size_t size);
void operator delete[](void *pMemory);

void * operator new(size_t size, r3dAllocationTypes t);
void operator delete(void * ptr, r3dAllocationTypes t);
void * operator new[](size_t size, r3dAllocationTypes t);
void operator delete[](void * ptr, r3dAllocationTypes t);

#define game_new new (ALLOC_TYPE_GAME)
#define gfx_new new (ALLOC_TYPE_GFX)

//-------------------------------------------------------------------------
//	STL
//-------------------------------------------------------------------------

#include "r3dSTLAllocators.h"

#define r3dgameVector(type) std::vector<type, r3dSTLCustomAllocator<type, ALLOC_TYPE_GAME> >
#define r3dgameList(type) std::list<type, r3dSTLCustomAllocator<type, ALLOC_TYPE_GAME> >
#define r3dgameSet(type) std::set<type, std::less<type>, r3dSTLCustomAllocator<type, ALLOC_TYPE_GAME> >
#define r3dgameMap(key, value) std::map<key, value, std::less<key>, r3dSTLCustomAllocator<std::pair<const key, value>, ALLOC_TYPE_GAME> >
#define r3dgameMultiMap(key, value) std::multimap<key, value, std::less<key>, r3dSTLCustomAllocator<std::pair<const key, value>, ALLOC_TYPE_GAME> >
#define r3dgameMultiMapCmp(key, value, cmp) std::multimap<key, value, cmp, r3dSTLCustomAllocator<std::pair<const key, value>, ALLOC_TYPE_GAME> >
#define r3dgameHashMap(key, value) stdext::hash_map<key, value, stdext::hash_compare<key, std::less<key> >, r3dSTLCustomAllocator<std::pair<const key, value>, ALLOC_TYPE_GAME> >
#define r3dgameUnorderedMap(key, value) std::tr1::unordered_map<key, value, std::tr1::hash<key>, std::equal_to<key>, r3dSTLCustomAllocator<std::pair<const key, value>, ALLOC_TYPE_GAME > >


#define r3dgfxVector(type) std::vector<type, r3dSTLCustomAllocator<type, ALLOC_TYPE_GFX> >
#define r3dgfxList(type) std::list<type, r3dSTLCustomAllocator<type, ALLOC_TYPE_GFX> >
#define r3dgfxSet(type) std::set<type, std::less<type>, r3dSTLCustomAllocator<type, ALLOC_TYPE_GFX> >
#define r3dgfxMap(key, value) std::map<key, value, std::less<key>, r3dSTLCustomAllocator<std::pair<const key, value>, ALLOC_TYPE_GFX> >
#define r3dgfxMultiMap(key, value) std::multimap<key, value, std::less<key>, r3dSTLCustomAllocator<std::pair<const key, value>, ALLOC_TYPE_GFX> >
#define r3dgfxHashMap(key, value) stdext::hash_map<key, value, stdext::hash_compare<key, std::less<key> >, r3dSTLCustomAllocator<std::pair<const key, value>, ALLOC_TYPE_GFX> >
#define r3dgfxUnorderedMap(key, value) std::tr1::unordered_map<key, value, std::tr1::hash<key>, std::equal_to<key>, r3dSTLCustomAllocator<std::pair<const key, value>, ALLOC_TYPE_GFX > >

typedef std::basic_string<char, std::char_traits<char>, r3dSTLCustomAllocator<char, ALLOC_TYPE_GAME> > r3dSTLString;

#else

#define r3dgameVector(type) std::vector<type >
#define r3dgameList(type) std::list<type >
#define r3dgameSet(type) std::set<type >
#define r3dgameMap(key, value) std::map<key, value >
#define r3dgameMultiMap(key, value) std::multimap<key, value >
#define r3dgameMultiMapCmp(key, value, cmp) std::multimap<key, value, cmp >
#define r3dgameHashMap(key, value) stdext::hash_map<key, value >
#define r3dgameUnorderedMap(key, value) std::tr1::unordered_map<key, value >

#define r3dgfxVector(type) std::vector<type >
#define r3dgfxList(type) std::list<type >
#define r3dgfxSet(type) std::set<type >
#define r3dgfxMap(key, value) std::map<key, value >
#define r3dgfxMultiMap(key, value) std::multimap<key, value >
#define r3dgfxHashMap(key, value) stdext::hash_map<key, value >
#define r3dgfxUnorderedMap(key, value) std::tr1::unordered_map<key, value >

typedef std::basic_string<char, std::char_traits<char>, std::allocator<char> > r3dSTLString;

#define game_new new
#define gfx_new new

#endif
