/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JUBA - secondary contact: LAPA

#ifndef Navigation_NavGraphLink_H
#define Navigation_NavGraphLink_H

#include "gwnavruntime/navgraph/identifiers/navgraphvertexrawptr.h"
#include "gwnavruntime/navmesh/identifiers/navtrianglerawptr.h"
#include "gwnavruntime/containers/collection.h"

namespace Kaim
{

class NavGraphLink
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)

public:
	NavGraphLink(const NavGraphVertexRawPtr& navGraphVertexRawPtr);

	void Set(const NavTriangleRawPtr& triangleRawPtr);

	bool IsValid() const { return GetIndexInCollection() != KyUInt16MAXVAL; }

	KyUInt32 GetIndexInCollection() const;
	void SetIndexInCollection(KyUInt32 indexInCollection);

public:
	KyUInt16 m_indexInFloorCollection; // for remove O(1) in the collection of the NavFloor

	NavGraphVertexRawPtr m_navGraphVertexRawPtr;
	NavTriangleRawPtr m_navTriangleRawPtr;
};

KY_INLINE NavGraphLink::NavGraphLink(const NavGraphVertexRawPtr& navGraphVertexRawPtr) :
	m_indexInFloorCollection(KyUInt16MAXVAL),
	m_navGraphVertexRawPtr(navGraphVertexRawPtr)
{}

KY_INLINE void NavGraphLink::Set(const NavTriangleRawPtr& triangleRawPtr) { m_navTriangleRawPtr = triangleRawPtr; }

KY_INLINE KyUInt32 NavGraphLink::GetIndexInCollection() const { return m_indexInFloorCollection; }
KY_INLINE void NavGraphLink::SetIndexInCollection(KyUInt32 indexInCollection) { m_indexInFloorCollection = (KyUInt16)indexInCollection; }

}

#endif // Navigation_NavGraphLink_H
