/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JUBA - secondary contact: LASI
#ifndef Navigation_NavTagSubSegment_H
#define Navigation_NavTagSubSegment_H

#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"


namespace Kaim
{


/// This class is a "volatile" version of the NavTagSubSegment class. It used internally during computation 
///  and then finally stored as NavTagSubSegment in the QueryDynamicOutput. see NavTagSubSegment for details
class RawNavTagSubSegment
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)

public:
	RawNavTagSubSegment() :
		m_entrancePos3f(KyFloat32MAXVAL, KyFloat32MAXVAL, KyFloat32MAXVAL),
		m_exitPos3f(KyFloat32MAXVAL, KyFloat32MAXVAL, KyFloat32MAXVAL)
	{}

	RawNavTagSubSegment(const NavFloorRawPtr& navFloorRawPtr, NavTriangleIdx entranceTriangleIdx, const Vec3f& entranceCoordPos3f, const Vec3f& exitCoordPos3f) :
		m_navTriangleRawPtr(navFloorRawPtr, entranceTriangleIdx), m_entrancePos3f(entranceCoordPos3f), m_exitPos3f(exitCoordPos3f)
	{}

public:
	// last Connex encountered during the building of the subSegment
	NavTriangleRawPtr m_navTriangleRawPtr; ///< Any triangle crossed by the segment, to enable access the navTag
	
	Vec3f m_entrancePos3f; ///< Records the starting point of the sub-segment. 
	Vec3f m_exitPos3f; ///< Records the ending point of the sub-segment. 
};

/// This class represents a NavTag sub-segment of a ray (within a RayCastQuery or a RayCanGoQuery, carried out against the NavMesh).
/// When you conduct a RayCastQuery or a RayCanGoQuery against a NavMesh, you can set up the QueryDynamicOutputMode
///  for the query to record a NavTagSubSegment for each different NavTag crossed by the ray.
/// You can then iterate through these NavTagSubSegment to make different types of calculations based on theses different NavTag
/// As an example, their are used to build the PathEventList : we retrieve the NavTagSubSegment along the Path by performing queries
/// along each PathEdge and we process them.
class NavTagSubSegment
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)

public:
	NavTagSubSegment() :
		m_entrancePos3f(KyFloat32MAXVAL, KyFloat32MAXVAL, KyFloat32MAXVAL),
		m_exitPos3f(KyFloat32MAXVAL, KyFloat32MAXVAL, KyFloat32MAXVAL) {}

	NavTagSubSegment(const RawNavTagSubSegment& rawNavTagSubSegment) :
		m_navTrianglePtr(rawNavTagSubSegment.m_navTriangleRawPtr),
		m_entrancePos3f(rawNavTagSubSegment.m_entrancePos3f),
		m_exitPos3f(rawNavTagSubSegment.m_exitPos3f) {}

public:
	// accessing the navTag
	NavTrianglePtr m_navTrianglePtr; ///< Records one NavTrianglePtr referencing any triangle crossed by the segment, to enable access the navTag
	Vec3f m_entrancePos3f;           ///< Records the starting point of the sub-segment. 
	Vec3f m_exitPos3f;               ///< Records the ending point of the sub-segment. 
};
}


#endif //Navigation_NavTagSubSegment_H


