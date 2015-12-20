/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_StlList_H
#define GwNavGen_StlList_H


#include "gwnavruntime/base/memory.h"
#include "gwnavgeneration/common/stlallocator.h"

#if defined(KY_CC_MSVC)
#pragma warning (push)
#pragma warning (disable:4510) //  'std::_List_nod<_Ty,_Alloc>::_Node' : default constructor could not be generated
#pragma warning (disable:4548) //  expression before comma has no effect; expected expression with side-effect
#pragma warning (disable:4555) //  expression has no effect; expected expression with side-effect
#pragma warning (disable:4571) //  Informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
#pragma warning (disable:4610) //  struct 'std::_List_nod<_Ty,_Alloc>::_Node' can never be instantiated - user defined constructor required
#pragma warning (disable:4619) //  #pragma warning : there is no warning number '4284'
#include <list>
#pragma warning (pop)
#else
#include <list>
#endif


#ifndef KY_STL_USED
#define KY_STL_USED
#endif


namespace Kaim
{

template< typename T, typename A = Kaim::TlsStlAllocator<T> >
class TlsStlList : public std::list<T, A>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
};

}


#endif
