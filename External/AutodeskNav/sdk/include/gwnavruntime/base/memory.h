/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JAPA - secondary contact: GUAL

#ifndef Navigation_Memory_H
#define Navigation_Memory_H

#include "gwnavruntime/base/types.h"
#include "gwnavruntime/kernel/SF_HeapNew.h"

namespace Kaim
{

// Stat identifiers are organized into groups corresponding to different sub-systems such as World, etc.
// Every group receives its own Id range, allowing identifiers within that system to be added without collision
enum MemStatGroupStartEnum
{
	MemStatGroupStart_NonGoup      = 1 << 6, // [64 ; 128[
	MemStatGroupStart_World        = 2 << 6, // [128; 192[
	MemStatGroupStart_Data         = 3 << 6, // [192; 256[
	MemStatGroupStart_WorkingMem   = 4 << 6, // [256; 320[
	MemStatGroupStart_VisualDebug  = 5 << 6, // [320; 384[
	MemStatGroupStart_NavDataGen   = 6 << 6, // [384; 448[
	MemStatGroupStart_External     = 7 << 6, // 
};

enum MemStatEnum_NonGoup
{
	MemStat_BaseSystem = MemStatGroupStart_NonGoup
};

enum MemStatEnum_World
{
	MemStatGroup_World = MemStatGroupStart_World,
		MemStat_WorldFwk,
		MemStat_BoxObstacle,
		MemStat_TagVolume,
		MemStat_CylinderObstacle,
		MemStat_PointOfInterest,
		MemStat_Bot,
		MemStat_PathFollowing,
		MemStat_LivePath,
		MemStat_Path,
		MemStat_Channel,
		MemStat_Query,
		MemStat_QueryOutput,
		MemStat_QuerySystem,
		MemStat_DynamicNavMesh,
		MemStat_VisualDebugRegistry,
		MemStat_World_Other,
};

enum MemStatEnum_Data
{
	MemStatGroup_Data = MemStatGroupStart_Data,
		MemStat_Blob,
		MemStat_NavDataBlob,
		MemStat_NavData,
		MemStat_NavMesh,
		MemStat_NavGraph,
		MemStat_Spatialization,
		MemStat_BlobBuilder,
		MemStat_CollisionDataBlob,
		MemStat_CollisionData,
		MemStat_CostMap,
		MemStat_Data_Other,
};

enum MemStatEnum_WorkingMem
{
	MemStatGroup_WorkingMem = MemStatGroupStart_WorkingMem,
		MemStat_QueryWorkingMem,
		MemStat_DynNavMesh,
};

enum MemStatEnum_VisualDebug
{
	MemStatGroup_VisualDebug = MemStatGroupStart_VisualDebug,
		MemStat_VisualDebug,
		MemStat_VisualSystem,
		MemStat_VisualDebugMessage,
		MemStat_NavLabFrame,
		MemStat_Statistics,
		MemStat_VisualDebug_Other,
};

enum MemStatEnum_NavDataGen
{
	MemStatGroup_NavDataGen = MemStatGroupStart_NavDataGen,
		MemStat_NavDataGen,
};

enum MemStatEnum_External
{
	MemStatGroup_External = MemStatGroupStart_External,
		MemStat_LabEngine,
		MemStat_LabGame,
		MemStat_NavigationLab,
		MemStat_UnitTest,
		MemStat_Client,
};


void LinkMemoryStats();

}

//--------------------------------------------------------------------------------------------
// macros for built-in types - no constructor or destructor called
// same as KY_ALLOC but take a T parameter and automatically multiply by sizeof(T)
#define KY_MALLOC(T, count, memStat) ( (T*)KY_ALLOC((sizeof(T) * (count)), memStat) )
#define KY_MALLOC_ALIGNED(T, count, alignment, memStat) ( (T*)KY_MEMALIGN((sizeof(T) * (count)), alignment, memStat) )
#define KY_HEAP_MALLOC(heap, T, n, memStat) ( (T*)KY_HEAP_ALLOC(heap, (sizeof(T) * (n)), memStat) )


//--------------------------------------------------------------------------------------------
// Use Gameware Navigation allocation for classes that do not have KY_DEFINE_NEW_DELETE_OPERATORS
// Requires exact class for destruction
#define KY_NEW_EXTERN(T, memStat) new(KY_ALLOC(sizeof(T), memStat)) T
#define KY_DELETE_EXTERN(T, p) if (p) { p->~T(); KY_FREE(p); }


/// This macro defines new and delete operators. 
/// For internal use in the declarations of classes that do not derive from NewOverrideBase.
#define KY_DEFINE_NEW_DELETE_OPERATORS(MemStat)       \
	public:                                           \
		KY_MEMORY_REDEFINE_NEW(NavigationClass, MemStat) \
		KY_MEMORY_DEFINE_PLACEMENT_NEW                \
	private:

/// Specific to RefCounting, no inheritance involved, used to make RefCount-able classes compatible with Kaim::Ptr 
/// whose instances are created via placement new in their own preallocated buffer.
#define KY_REFCOUNT_MALLOC_FREE(ClassName)                                        \
	public:                                                                       \
		KY_INLINE void AddRef() { RefCount ++; }					              \
		KY_INLINE void Release()									              \
		{                                                                         \
			if ((RefCount.ExchangeAdd_NoSync(-1) - 1) == 0)			              \
				KY_FREE(this);                                                    \
		}                                                                         \
		int GetRefCount() const { return RefCount; }				              \
	private:                                                                      \
		ClassName();  /* Default constructor implementation mandatory */          \
		static KY_INLINE ClassName* Create(char* memoryStart)                     \
		{                                                                         \
			ClassName* instance = (::new((ClassName*)memoryStart) ClassName());   \
			if (instance)	instance->RefCount = 1;                               \
			return instance;                                                      \
		}                                                                         \
	private:                                                                      \
		mutable AtomicInt<int> RefCount;

namespace Kaim
{

// Add KY_NEW_ID to alreadt existing KY_HEAP_NEW, KY_HEAP_NEW_ID, KY_HEAP_AUTO_NEW, KY_NEW
#ifdef KY_MEMORY_ENABLE_DEBUG_INFO
# define KY_NEW_ID(id) new(Kaim::Memory::GetGlobalHeap(), id, __FILE__,__LINE__)
#else
# define KY_NEW_ID(id) new(Kaim::Memory::GetGlobalHeap(), id)
#endif 

// Simple macro for repetitive code
#define KY_DELETE_AND_SET_NULL(x)  \
{                                  \
	if (x != KY_NULL)              \
	{                              \
		delete x;                  \
		x = KY_NULL;               \
	}                              \
}                                  \


}

#endif
