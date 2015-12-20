/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_IGeneratorGlue_H
#define GwNavGen_IGeneratorGlue_H


#include "gwnavruntime/base/types.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/base/memory.h"


namespace Kaim
{

class IParallelForInterface;
class ITlsAlloc;
class ICollisionInterface;

/// Provides an abstract base interface for a class that the Generator can invoke in order to retrieve objects that
/// implement specific interfaces.
/// You cannot use this class as-is; you must write your own implementation, create an instance of your class, and
/// pass that instance in the constructor for your Generator instance. Any time the Generator needs to call one of the
/// interfaces managed by the IGeneratorGlue class, it calls the methods in your class instance to retrieve a pointer.
/// Using this class is an alternative to providing pointers to the interfaces directly in the constructor for the
/// Generator.
class IGeneratorGlue : public RefCountBaseV<IGeneratorGlue, MemStat_NavDataGen>
{
public:
	virtual ~IGeneratorGlue() {}

	/// This method is expected to return a pointer to an object that the Generator will use to parallelize computations
	/// over multiple processors, or KY_NULL in order to perform all computations sequentially on a single processor.
	/// This object is not copied; you must ensure that you do not destroy it until after you are finished using the
	/// Generator.
	virtual IParallelForInterface*   GetParallelFor()   = 0;

	/// This method is expected to return a pointer to an object that the Generator will use to allocate and free memory
	/// in secondary threads, when configured to use parallel generation. This object is not copied; you must ensure that
	/// you do not destroy it until after you are finished using the Generator.
	virtual ITlsAlloc*               GetTlsAlloc()      = 0;

	virtual ICollisionInterface*     GetVisInterface()     = 0;
};

}


#endif
