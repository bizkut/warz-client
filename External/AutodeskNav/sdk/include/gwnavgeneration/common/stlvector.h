/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_StlVector_H
#define GwNavGen_StlVector_H


#include "gwnavgeneration/common/stlallocator.h"


#if defined(KY_CC_MSVC)
# pragma warning (push)
# pragma warning (disable:4548) //  expression before comma has no effect; expected expression with side-effect
#include <vector>
# pragma warning (pop)
#else
#include <vector>
#endif

#ifndef KY_STL_USED
#define KY_STL_USED
#endif

namespace Kaim
{


template< typename T, typename A = Kaim::TlsStlAllocator<T> >
class TlsStlVector : public std::vector<T, A> 
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	TlsStlVector() : std::vector<T, A>() {}
	TlsStlVector(size_t count) : std::vector<T, A>(count) {}
};


}


#endif
