/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_IParallelForInterface_H
#define GwNavGen_IParallelForInterface_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_RefCount.h"


namespace Kaim
{

/// Provides an abstract base interface for an object that treats an element of data that can be passed to an object that derives from
/// IParallelForInterface. 
/// You should not need to implement this interface yourself; specialized implementations for different kinds of data elements 
/// may be provided with Gameware Navigation. 
/// If you write a custom implementation of IParallelForInterface, your IParallelForInterface class just needs to call the Do() method of the
/// IParallelElementFunctor passed to its IParallelForInterface::ParallelFor() method for each element passed to IParallelForInterface::ParallelFor().
class IParallelElementFunctor : public NewOverrideBase<MemStat_NavDataGen>
{
public:
	virtual ~IParallelElementFunctor() {}

	/// This method is carries out whatever processing needs to be done for the specified data element. 
	virtual void Do(void* element) = 0;
};


/// Provides an abstract base interface for an object that can treat computational jobs in parallel. 
/// This class cannot be used as-is; you must use an instance of a class that derives from IParallelForInterface and that implements
/// its virtual methods. Gameware Navigation provides a default implementation, KyGlue::TbbParallelForInterface, which uses the Threaded Building Blocks
/// (TBB) libraries from Intel to manage the parallel execution of its computational jobs. 
/// You can also create your own implementation of IParallelForInterface if you need to support a different parallel computing system. For
/// an example implementation, see the code for the KyGlue::TbbParallelForInterface class, available in the integration directory.
class IParallelForInterface : public RefCountBaseV<IParallelForInterface, MemStat_NavDataGen>
{
public:
	/* For internal use only. */
	class InitObject
	{
	public:
		InitObject(IParallelForInterface* parallelFor) : m_parallelFor(parallelFor) { if (m_parallelFor) m_parallelFor->Init(); }
		~InitObject() { if (m_parallelFor) m_parallelFor->Terminate(); }
		IParallelForInterface* m_parallelFor;
	};

public:
	virtual ~IParallelForInterface() {}

	/// This method is expected to perform whatever steps are needed to initialize the parallelization mechanism. 
	/// Called once by the Generator at the beginning of the data generation process.
	virtual KyResult Init() = 0;

	/// This method is expected to perform whatever steps are needed to shut down the parallelization mechanism.
	/// Called once by the Generator at the end of the data generation process.
	virtual KyResult Terminate() = 0;

	/// This is the main method of the IParallelForInterface: it is called by Gameware Navigation to manage
	/// the parallel execution of an array of computational jobs.
	/// An implementation of this method should call the IParallelElementFunctor::Do() method for each element
	/// in the elements array.
	/// \param elements				An array of void pointers, each of which points to a computational job that needs to be carried out.
	/// \param elementsCount		The number of elements in theelements array.
	/// \param functor				A pointer to the object that should be responsible for actually treating each element in the elements
	/// 							array. 
	/// \return A #KyResult that indicates the success or failure of the parallel computations. 
	virtual KyResult ParallelFor(void** elements, KyUInt32 elementsCount, Kaim::IParallelElementFunctor* functor) = 0;
};


}


#endif
