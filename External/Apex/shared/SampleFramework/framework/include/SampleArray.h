#ifndef SAMPLE_ARRAY
#define SAMPLE_ARRAY

#include "PsArray.h"
#include "PsInlineArray.h"
#include "PsHashMap.h"
#include "PsAllocator.h"

namespace SampleFramework
{
	template<typename T>
	class Array : public physx::shdfnd::Array<T, physx::shdfnd::RawAllocator>
	{
	public:
		PX_INLINE explicit Array() : physx::shdfnd::Array<T, physx::shdfnd::RawAllocator>() {}		
		PX_INLINE explicit Array(physx::PxU32 size, const T& a = T()) : physx::shdfnd::Array<T, physx::shdfnd::RawAllocator>(size, a) {}		
	};
}


#endif
