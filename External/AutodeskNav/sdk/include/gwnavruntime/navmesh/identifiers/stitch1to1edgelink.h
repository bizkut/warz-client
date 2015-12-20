/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_Stitch1To1EdgeLink_H
#define Navigation_Stitch1To1EdgeLink_H

#include "gwnavruntime/navmesh/identifiers/navfloorptr.h"

namespace Kaim
{

///////////////////////////////////////////////////////////////////////////////////////////
// Stitch1To1EdgeLink
///////////////////////////////////////////////////////////////////////////////////////////

class Stitch1To1EdgeLink
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)

public:
	/// Creates an invalid Stitch1To1EdgeLink. 
	Stitch1To1EdgeLink();

	/// Creates a new NavHalfEdgePtr referring to the provided NavHalfEdgeIdx in the provided NavFloor.
	/// \param navFloor       The NavFloor that contains the edge this object should refer to. 
	/// \param stitch1To1EdgeIdx    The index of this edge within the NavFloor. 
	Stitch1To1EdgeLink(NavFloor* navFloor, KyUInt32 stitch1To1EdgeIdx);

	/// Clears all information maintained by this object.
	/// \param navFloor       The NavFloor that contains the edge this object should refer to. 
	/// \param stitch1To1EdgeIdx    The index of this edge within the NavFloor. 
	void Set(NavFloor* navFloor, KyUInt32 stitch1To1EdgeIdx);

	/// Returns true if this object refers to a valid edge: i.e. an edge in a validNavFloor. see NavFloorPtr::IsValid()
	bool IsValid() const;

	void Invalidate(); ///< Invalidates this object.

	bool operator==(const Stitch1To1EdgeLink& rhs) const; ///< Returns true if this object identifies the same edge as rhs, or if both are invalid. 
	bool operator!=(const Stitch1To1EdgeLink& rhs) const; ///< Returns true if this object identifies a different edge from rhs. 
	
	// ---------------------------------- Member Functions for valid instance ----------------------------------
	/// All these function should be called only if IsValid() returns true
	/// \pre    This object must be valid. Its validity is not checked. 

	/// Returns a reference to the NavFloor that contains this hedge.
	/// \pre    This object must be valid. Its validity is not checked. 
	NavFloor* GetNavFloor() const;

	/// Returns a reference to the NavFloorBlob that contains this edge.
	/// \pre    This object must be valid. Its validity is not checked. 
	const NavFloorBlob* GetNavFloorBlob() const;

public:
	NavFloor* m_navFloor;         ///< The NavFloor coming from the generation corresponding to the NavFloor1To1StitchData
	KyUInt16 m_stitch1To1EdgeIdx; ///< The index of this edge within its NavFloor1To1StitchData.
	KyUInt16 m_correspondingOfStitch1To1Idx; ///< Use internally to speed up stitching. Do not modify.
};



KY_INLINE Stitch1To1EdgeLink::Stitch1To1EdgeLink() : m_navFloor(), m_stitch1To1EdgeIdx(KyUInt16MAXVAL), m_correspondingOfStitch1To1Idx(KyUInt16MAXVAL) {}

KY_INLINE Stitch1To1EdgeLink::Stitch1To1EdgeLink(NavFloor* navFloor, NavHalfEdgeIdx stitch1To1EdgeIdx) : 
m_navFloor(navFloor), m_stitch1To1EdgeIdx((CompactNavHalfEdgeIdx)stitch1To1EdgeIdx), m_correspondingOfStitch1To1Idx(KyUInt16MAXVAL) {}

KY_INLINE void Stitch1To1EdgeLink::Set(NavFloor* navFloor, NavHalfEdgeIdx stitch1To1EdgeIdx)
{
	m_navFloor = navFloor;
	m_stitch1To1EdgeIdx = (CompactNavHalfEdgeIdx)stitch1To1EdgeIdx;
	m_correspondingOfStitch1To1Idx = KyUInt16MAXVAL;
}

KY_INLINE bool Stitch1To1EdgeLink::IsValid() const { return m_navFloor != KY_NULL && m_stitch1To1EdgeIdx != KyUInt16MAXVAL; }
KY_INLINE void Stitch1To1EdgeLink::Invalidate() { m_navFloor = KY_NULL; m_stitch1To1EdgeIdx = KyUInt16MAXVAL; m_correspondingOfStitch1To1Idx = KyUInt16MAXVAL; }

KY_INLINE bool Stitch1To1EdgeLink::operator==(const Stitch1To1EdgeLink& rhs) const { return m_navFloor == rhs.m_navFloor && m_stitch1To1EdgeIdx == rhs.m_stitch1To1EdgeIdx;}
KY_INLINE bool Stitch1To1EdgeLink::operator!=(const Stitch1To1EdgeLink& rhs) const { return !(*this == rhs); }

KY_INLINE NavFloor*           Stitch1To1EdgeLink::GetNavFloor()     const { return m_navFloor;                    }
KY_INLINE const NavFloorBlob* Stitch1To1EdgeLink::GetNavFloorBlob() const { return m_navFloor->GetNavFloorBlob(); }
}

#endif //Navigation_Stitch1To1EdgeLink_H

