/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JUBA - secondary contact: LASI
#ifndef Navigation_HalfEdgeIntersection_H
#define Navigation_HalfEdgeIntersection_H

#include "gwnavruntime/navmesh/identifiers/navhalfedgeptr.h"


namespace Kaim
{

// This class is a "volatile" version of the HalfEdgeIntersection class. It used internally during computation 
///  and then finally stored as NavTagSubSegment in the QueryDynamicOutput. see HalfEdgeIntersection for detai
class RawHalfEdgeIntersection
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)

public:
	RawHalfEdgeIntersection() : m_intersectionPos3f(KyFloat32MAXVAL, KyFloat32MAXVAL, KyFloat32MAXVAL) {}
	RawHalfEdgeIntersection(const NavHalfEdgeRawPtr& navHalfEdgeRawPtr, const Vec3f& intersectionPos3f) :
		m_navHalfEdgeRawPtr(navHalfEdgeRawPtr), m_intersectionPos3f(intersectionPos3f) {}

public:
	NavHalfEdgeRawPtr m_navHalfEdgeRawPtr; ///< Records the NavHalfEdge crossed by this sub-segment.
	Vec3f m_intersectionPos3f;             ///< Records the intersections between the ray and the HalfEdge.
};

/// This class represents an intersection between an NavHalfEdge and a ray (within a RayCastQuery or a RayCanGoQuery,
///  carried out against the NavMesh).
/// When you conduct a RayCastQuery or a RayCanGoQuery against a NavMesh, you can set up the QueryDynamicOutputMode
///  for the query to record a HalfEdgeIntersection for each different NavHalfEdge crossed by the ray.
/// You can then iterate through these HalfEdgeIntersection to make different types of calculations based on theses different NavTag
/// As an example, their are used to clamp the path on the NavMesh.we retrieve the HalfEdgeIntersection along the Path by performing queries
/// along each PathEdge and we process them.
class HalfEdgeIntersection
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)

public:
	HalfEdgeIntersection() : m_intersectionPos3f(KyFloat32MAXVAL, KyFloat32MAXVAL, KyFloat32MAXVAL) {}
	HalfEdgeIntersection(const RawHalfEdgeIntersection& rawHalfEdgeIntersection) :
		m_navHalfEdgePtr(rawHalfEdgeIntersection.m_navHalfEdgeRawPtr.GetNavFloor(), rawHalfEdgeIntersection.m_navHalfEdgeRawPtr.GetHalfEdgeIdx()),
		m_intersectionPos3f(rawHalfEdgeIntersection.m_intersectionPos3f) {}

public:
	NavHalfEdgePtr m_navHalfEdgePtr; ///< Records the NavHalfEdge crossed by this sub-segment.
	Vec3f m_intersectionPos3f;       ///< Records the intersections Between the ray and the halfEdge
};

}


#endif //Navigation_HalfEdgeIntersection_H


