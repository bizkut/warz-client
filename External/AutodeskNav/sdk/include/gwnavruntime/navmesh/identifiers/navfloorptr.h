/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavFloorPtr_H
#define Navigation_NavFloorPtr_H

#include "gwnavruntime/navmesh/identifiers/navfloorrawptr.h"

namespace Kaim
{

/// Each instance of this class uniquely identifies a single NavFloor.
/// Instances of this class can be safely stored and used in subsequent frames.
class NavFloorPtr
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	// ---------------------------------- Main API Functions ----------------------------------

	/// Creates an invalid NavFloorPtr. 
	NavFloorPtr(NavFloor* navFloor = KY_NULL);

	/// Creates a new NavFloorPtr copying information from the provided one.
	explicit NavFloorPtr(const NavFloorRawPtr& navFloorRawPtr);

	/// Returns true if this object refers to a valid NavFloor, which means a NavFloor:
	/// -# that exists,
	/// -# whose underlying static data (NavFloorBlob) has not been removed from the Database and
	/// -# which is active (in term of overlap and dynamic version).
	bool IsValid() const; 

	void Invalidate(); ///< Invalidates this object.

	bool operator==(const NavFloorPtr& rhs) const; ///< Returns true if this object identifies the same NavFloor as rhs, or if both are invalid. 
	bool operator!=(const NavFloorPtr& rhs) const; ///< Returns true if this object identifies a different NavFloor from rhs. 

	NavFloor*           GetNavFloor()     const; ///< Returns a pointer to the NavFloor identified by this object. Returns KY_NULL if this object is not valid. 
	const NavFloorBlob* GetNavFloorBlob() const; ///< Returns a const pointer to the NavFloorBlob identified by this object. Returns KY_NULL if this object is not valid. 

	// ---------------------------------- Member Functions for valid instance ----------------------------------
	/// \pre All these function should be called only if IsValid() returns true.

	/// Returns a pointer to the NavFloor identified by this object.
	/// \pre    This object must be valid. Its validity is not checked.
	NavFloor* GetNavFloor_Unsafe() const;

	/// Returns a const pointer to the NavFloorBlob identified by this object.
	/// \pre    This object must be valid. Its validity is not checked.
	const NavFloorBlob* GetNavFloorBlob_Unsafe() const;

	/// Constructs and returns a NavFloorRawPtr that refers to the same NavFloor.
	/// \pre    This object must be valid. Its validity is not checked.
	NavFloorRawPtr GetRawPtr() const;

	/// Returns a const reference to the CellPos that indicates the position of the cell that contains this NavFloor.
	/// \pre    This object must be valid. Its validity is not checked.
	const CellPos& GetCellPos() const;

private:
	mutable Ptr<NavFloor> m_navFloorPtr;
};

KY_INLINE NavFloorPtr::NavFloorPtr(NavFloor* navFloor) : m_navFloorPtr(navFloor) {}
KY_INLINE NavFloorPtr::NavFloorPtr(const NavFloorRawPtr& navFloorRawPtr) : m_navFloorPtr(navFloorRawPtr.GetNavFloor()) {}
KY_INLINE void NavFloorPtr::Invalidate() { m_navFloorPtr = KY_NULL; }
KY_INLINE bool NavFloorPtr::IsValid() const
{
	if(m_navFloorPtr != KY_NULL)
	{
		if(m_navFloorPtr->IsStillLoaded())
			return m_navFloorPtr->IsActive();

		m_navFloorPtr = KY_NULL;
	}

	return false;
}


KY_INLINE NavFloor*           NavFloorPtr::GetNavFloor()     const { return IsValid() ? GetNavFloor_Unsafe()     : KY_NULL; }
KY_INLINE const NavFloorBlob* NavFloorPtr::GetNavFloorBlob() const { return IsValid() ? GetNavFloorBlob_Unsafe() : KY_NULL; }

KY_INLINE NavFloor*           NavFloorPtr::GetNavFloor_Unsafe()     const { return m_navFloorPtr;                    }
KY_INLINE const NavFloorBlob* NavFloorPtr::GetNavFloorBlob_Unsafe() const { return m_navFloorPtr->GetNavFloorBlob(); }

KY_INLINE NavFloorRawPtr NavFloorPtr::GetRawPtr() const { return NavFloorRawPtr(m_navFloorPtr.GetPtr()); }

KY_INLINE bool NavFloorPtr::operator==(const NavFloorPtr& rhs) const { return m_navFloorPtr == rhs.m_navFloorPtr; }
KY_INLINE bool NavFloorPtr::operator!=(const NavFloorPtr& rhs) const { return m_navFloorPtr != rhs.m_navFloorPtr; }

KY_INLINE const CellPos& NavFloorPtr::GetCellPos() const { return GetRawPtr().GetCellPos(); }

}

#endif //Navigation_NavFloorPtr_H

