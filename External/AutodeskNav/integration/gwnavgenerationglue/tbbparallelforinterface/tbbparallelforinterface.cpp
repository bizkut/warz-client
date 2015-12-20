/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#include "tbbparallelforinterface.h"



#ifndef NDEBUG
// ----------------------------------------------- DEBUG -----------------------------------------------
// in debug we do a serial for because TBB is very slow in debug
namespace KyGlue
{

TbbParallelForInterface::TbbParallelForInterface() : m_impl(KY_NULL) {}
TbbParallelForInterface::~TbbParallelForInterface() {}
KyResult TbbParallelForInterface::Init() { return KY_SUCCESS; }
KyResult TbbParallelForInterface::Terminate() { return KY_SUCCESS; }

KyResult TbbParallelForInterface::ParallelFor(void** elements, KyUInt32 elementsCount, Kaim::IParallelElementFunctor* elementFunctor)
{
	for (KyUInt32 i = 0; i < elementsCount; ++i)
		elementFunctor->Do(elements[i]);
	return KY_SUCCESS;
}

}


#else
// ----------------------------------------------- RELEASE -----------------------------------------------
// we use TBB in RELEASE


#if defined(KY_CC_MSVC)
# pragma warning (push)
# pragma warning (disable : 4242) // '%s' : conversion from 'tbb::internal::atomic_rep<4>::word' to 'tbb::internal::int32_t', possible loss of data
#endif

#include <tbb/task_scheduler_init.h>
#include <tbb/task_scheduler_observer.h>
#include <tbb/enumerable_thread_specific.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/parallel_for.h>

#if defined(KY_CC_MSVC)
# pragma warning (pop)
#endif

namespace KyGlue
{


// Functor to match TBB interface
class TbbRangeFunctor
{
public:
	TbbRangeFunctor(void** elements, Kaim::IParallelElementFunctor* elementFunctor)
		: m_elements(elements), m_elementFunctor(elementFunctor) {}

	void operator()(const tbb::blocked_range<size_t>& range) const
	{
		for (size_t i = range.begin(); i != range.end(); ++i)
		{
			m_elementFunctor->Do(m_elements[i]);
		}
	}

private:
	void** m_elements;
	Kaim::IParallelElementFunctor* m_elementFunctor;
};


class TbbParallelForInterface_Implementation
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Kaim::Stat_Default_Mem)

public:
	TbbParallelForInterface_Implementation(TbbParallelForInterface* interf) :
		m_interf(interf),
		m_tbb_init(tbb::task_scheduler_init::deferred),
		m_elements(KY_NULL), m_elementsCount(0), m_elementFunctor(KY_NULL)
	{
	}

	KyResult Init()
	{
		m_tbb_init.initialize();
		return KY_SUCCESS;
	}

	KyResult Terminate()
	{
		m_tbb_init.terminate();
		return KY_SUCCESS;
	}

	KyResult ParallelFor(void** elements, KyUInt32 elementsCount, Kaim::IParallelElementFunctor* elementFunctor)
	{
		TbbRangeFunctor elementRangeGenerateFunctor(elements, elementFunctor);

		tbb::blocked_range<size_t> elementRange(0, elementsCount, 1);

		tbb::parallel_for(elementRange, elementRangeGenerateFunctor, tbb::simple_partitioner());

		return KY_SUCCESS;
	}

private:
	TbbParallelForInterface* m_interf;
	tbb::task_scheduler_init m_tbb_init;

	void** m_elements;
	KyUInt32 m_elementsCount;
	Kaim::IParallelElementFunctor* m_elementFunctor;
};



TbbParallelForInterface::TbbParallelForInterface() : m_impl(KY_NULL) {}

TbbParallelForInterface::~TbbParallelForInterface() {}

KyResult TbbParallelForInterface::Init()
{
	if (m_impl != KY_NULL)
		delete m_impl;
	m_impl = KY_NEW TbbParallelForInterface_Implementation(this);
	return m_impl->Init();
}

KyResult TbbParallelForInterface::Terminate()
{
	if (m_impl != KY_NULL)
		m_impl->Terminate();
	delete m_impl;
	m_impl = KY_NULL;
	return KY_SUCCESS;
}

KyResult TbbParallelForInterface::ParallelFor(void** elements, KyUInt32 elementsCount, Kaim::IParallelElementFunctor* elementFunctor)
{
	if (m_impl == KY_NULL)
		return KY_ERROR;
	return m_impl->ParallelFor(elements, elementsCount, elementFunctor);
}


}

#endif