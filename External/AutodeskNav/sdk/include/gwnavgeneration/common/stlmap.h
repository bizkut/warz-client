/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_StlMap_H
#define GwNavGen_StlMap_H


#include "gwnavgeneration/common/stlallocator.h"
#include <map>

#ifndef KY_STL_USED
#define KY_STL_USED
#endif

namespace Kaim
{


template< typename Key, typename Type, typename Traits = std::less<Key>, typename A = Kaim::GlobalStlAllocator< std::pair <const Key, Type> > >
class StlMap : public std::map<Key, Type, Traits, A> 
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
};

template< typename Key, typename Type, typename Traits = std::less<Key>, typename A = Kaim::TlsStlAllocator< std::pair <const Key, Type> > >
class TlsStlMap : public std::map<Key, Type, Traits, A> 
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
};

}


#endif
