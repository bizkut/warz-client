#ifndef PLATFORM_CONFIG_H

#define PLATFORM_CONFIG_H

// Modify this header file to make the HACD data types be compatible with your
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <new>

#include "foundation/PxSimpleTypes.h"
#include "PsUserAllocated.h"

// This header file provides a brief compatibility layer between the PhysX and APEX SDK foundation header files.
// Modify this header file to your own data types and memory allocation routines and do a global find/replace if necessary
namespace physx
{
	using namespace shdfnd;
};

//using namespace physx;

namespace hacd
{

	typedef physx::PxI64		HaI64;
	typedef physx::PxI32		HaI32;
	typedef physx::PxI16		HaI16;
	typedef physx::PxI8		HaI8;

	typedef physx::PxU64		HaU64;
	typedef physx::PxU32		HaU32;
	typedef physx::PxU16		HaU16;
	typedef physx::PxU8		HaU8;

	typedef physx::PxF32		HaF32;
	typedef physx::PxF64		HaF64;

}; // end HACD namespace

#define USE_STL 0 // set to 1 to use the standard template library for all code; if off it uses high performance custom containers which trap all memory allocations.
#define USE_CONSTRAINT_BUILDER 0
#define UANS physx::shdfnd

#if USE_STL

#include <map>
#include <set>
#include <vector>
#define STDNAME std

#else

#include "PxVector.h"
#include "PxMapSet.h"

#define STDNAME physx

#endif

#endif
