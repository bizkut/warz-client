/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavFloorRawPtr_H
#define Navigation_NavFloorRawPtr_H

#include "gwnavruntime/navmesh/navfloor.h"

namespace Kaim
{

class NavFloorBlob;

/// Each instance of this class uniquely identifies a single NavFloor.
/// This pointer is guaranteed to be valid only in the frame in which it was retrieved.
/// Never store a NavFloorRawPtr for use in subsequent frames, because it has no protection against data
/// streaming. Use NavFloorPtr instead.
/// Actually, this class is used internally for some performance and working memory usage friendliness reason
/// (no ref-counting increment/decrement, no need to call constructor/destructor), but unless you really know what you do
/// prefer NavFloorPtr which is safer.
class NavFloorRawPtr
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	NavFloorRawPtr(); ///< Creates an invalid NavFloorRawPtr. 

	/// Creates a new NavFloorRawPtr pointing on the provided NavFloor.
	/// \param navFloor    A pointer to the NavFloor this object should refer to. 
	explicit NavFloorRawPtr(NavFloor* navFloor);

	/// Sets the information maintained by this object.
	/// \param navFloor    A pointer to the NavFloor we want the NavFloorRawPtr refers to. 
	void Set(NavFloor* navFloor);

	/// Returns true if this object refers to a valid NavFloor, which means a NavFloor:
	/// -# that exists,
	/// -# whose underlying static data (NavFloorBlob) has not been removed from the Database and
	/// -# which is active (in term of overlap and dynamic version).
	bool IsValid() const;

	/// Invalidates this object. 
	void Invalidate();

	bool operator==(const NavFloorRawPtr& rhs) const; ///< Returns true if this object identifies the same NavFloor as rhs, or if both are invalid. 
	bool operator!=(const NavFloorRawPtr& rhs) const; ///< Returns true if this object identifies a different NavFloor from rhs. 

	/// Returns a reference to the NavFloor identified by this object.
	NavFloor* GetNavFloor() const;

	/// Returns a reference to the NavFloor identified by this object.
	/// \pre    This object must be valid. Its validity is not checked. 
	const NavFloorBlob* GetNavFloorBlob() const;

	/// Returns a const reference to the CellPos that indicates the position of the cell that contains this NavFloor
	/// within the grid of NavData cells.
	/// \pre    This object must be valid. Its validity is not checked. 
	const CellPos& GetCellPos() const;

public:
	NavFloor* m_navFloor;
};

KY_INLINE NavFloorRawPtr::NavFloorRawPtr() : m_navFloor(KY_NULL) {}
KY_INLINE NavFloorRawPtr::NavFloorRawPtr(NavFloor* navFloor) : m_navFloor(navFloor) {}
KY_INLINE void NavFloorRawPtr::Set(NavFloor* navFloor) { m_navFloor = navFloor; }

KY_INLINE bool NavFloorRawPtr::IsValid() const { return m_navFloor != KY_NULL && m_navFloor->IsStillLoaded() && m_navFloor->IsActive(); }
KY_INLINE void NavFloorRawPtr::Invalidate() { m_navFloor = KY_NULL; }

KY_INLINE bool NavFloorRawPtr::operator==(const NavFloorRawPtr& rhs) const { return m_navFloor == rhs.m_navFloor; }
KY_INLINE bool NavFloorRawPtr::operator!=(const NavFloorRawPtr& rhs) const { return !(*this == rhs); }

KY_INLINE NavFloor*           NavFloorRawPtr::GetNavFloor()     const { return m_navFloor;                       }
KY_INLINE const NavFloorBlob* NavFloorRawPtr::GetNavFloorBlob() const { return GetNavFloor()->GetNavFloorBlob(); }
KY_INLINE const CellPos&      NavFloorRawPtr::GetCellPos()      const { return GetNavFloor()->GetCellPos();      }

}

#endif //Navigation_NavFloorRawPtr_H

