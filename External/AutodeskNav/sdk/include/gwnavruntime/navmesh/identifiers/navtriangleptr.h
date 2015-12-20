/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavTrianglePtr_H
#define Navigation_NavTrianglePtr_H

#include "gwnavruntime/navmesh/identifiers/navfloorptr.h"
#include "gwnavruntime/navmesh/identifiers/navtrianglerawptr.h"

namespace Kaim
{

/// Each instance of this class uniquely identifies a single NavTriangle in a NavFloor.
/// Instances of this class can be safely stored and used in subsequent frames.
class NavTrianglePtr
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	NavTrianglePtr(); ///< Creates an invalid NavTrianglePtr. 

	/// Creates a new NavTrianglePtr referring to the provided NavTriangleIdx in the provided NavFloorPtr.
	/// \param navFloorPtr    The NavFloor that contains the triangle this object should refer to.
	/// \param triangleIdx    The index of this triangle within the NavFloor. 
	NavTrianglePtr(const NavFloorPtr& navFloorPtr, NavTriangleIdx triangleIdx);

	/// Creates a new NavTrianglePtr copying information from the provided one.
	explicit NavTrianglePtr(const NavTriangleRawPtr& navTriangleRawPtr);


	/// Clears all information maintained by this object.
	/// \param navFloorPtr    The NavFloor that contains the triangle this object should refer to.
	/// \param triangleIdx    The index of this triangle within the NavFloor. 
	void Set(const NavFloorPtr& navFloorPtr, NavTriangleIdx triangleIdx);

	/// Returns true if this object refers to a valid triangle: i.e. a triangle in a validNavFloor. see NavFloorPtr::IsValid().
	bool IsValid() const;

	void Invalidate(); ///< Invalidates this object. 

	bool operator==(const NavTrianglePtr& rhs) const; ///< Returns true if this object identifies the same triangle as rhs, or if both are invalid. 
	bool operator!=(const NavTrianglePtr& rhs) const; ///< Returns true if this object identifies a different triangle from rhs. 

	NavTriangleIdx      GetTriangleIdx()  const; ///< Returns the index of this triangle within its NavFloor.
	NavFloor*           GetNavFloor()     const; ///< Returns a pointer to the NavFloor that contains this triangle. Returns KY_NULL if this object is not valid.
	const NavFloorBlob* GetNavFloorBlob() const; ///< Returns a const pointer to the NavFloorBlob that contains this triangle. Returns KY_NULL if this object is not valid.

	// ---------------------------------- Member Functions for valid instance ----------------------------------
	/// All these function should be called only if IsValid() returns true
	/// \pre    This object must be valid. Its validity is not checked. 

	/// Returns a pointer to the NavFloor that contains this triangle.
	/// \pre    This object must be valid. Its validity is not checked. 
	NavFloor* GetNavFloor_Unsafe() const;

	/// Returns a const pointer to the NavFloorBlob that contains this triangle.
	/// \pre    This object must be valid. Its validity is not checked. 
	const NavFloorBlob* GetNavFloorBlob_Unsafe() const;

	/// Constructs and returns a NavTriangleRawPtr that refers to the same NavTriangle
	/// \pre    This object must be valid. Its validity is not checked. 
	NavTriangleRawPtr GetRawPtr() const;

	 ///< Returns a const reference to the NavTag associated to this navTriangle.
	/// \pre    This object must be valid. Its validity is not checked.
	const NavTag& GetNavTag() const;

	/// Returns a const reference to the CellPos that indicates the position of the cell that contains this triangle
	/// \pre    This object must be valid. Its validity is not checked. 
	const CellPos& GetCellPos() const;

	/// Updates the parameters to identify the vertices in this triangle.
	/// \param[out] v0Pos3f    The 3D position of the first vertex in the triangle.
	/// \param[out] v1Pos3f    The 3D position of the second vertex in the triangle.
	/// \param[out] v2Pos3f    The 3D position of the third vertex in the triangle.
	/// \pre    This object must be valid. Its validity is not checked. 
	void GetVerticesPos3f(Vec3f& v0Pos3f, Vec3f& v1Pos3f, Vec3f& v2Pos3f) const;

	/// Updates the parameters to identify the vertices in this triangle.
	/// \param[out] triangle3f    The 3 3D positions of the vertices in the triangle.
	/// \pre    This object must be valid. Its validity is not checked. 
	void GetVerticesPos3f(Triangle3f& triangle3f) const;

	/// Updates the parameters to identify the vertices in this triangle.
	/// \param[out] v0CoordPos64    The 2D integer position of the first vertex in the triangle.
	/// \param[out] v1CoordPos64    The 2D integer position of the second vertex in the triangle.
	/// \param[out] v2CoordPos64    The 2D integer position of the third vertex in the triangle.
	/// \pre    This object must be valid. Its validity is not checked. 
	void GetVerticesCoordPos64(CoordPos64& v0CoordPos64, CoordPos64& v1CoordPos64, CoordPos64& v2CoordPos64) const;

	/// Updates the parameters to identify the vertices in this triangle.
	/// \param[out] v0CoordPos    The 2D integer position in cell of the first vertex in the triangle.
	/// \param[out] v1CoordPos    The 2D integer position in cell of the second vertex in the triangle.
	/// \param[out] v2CoordPos    The 2D integer position in cell of the third vertex in the triangle.
	/// \pre    This object must be valid. Its validity is not checked. 
	void GetVerticesCoordPos(CoordPos& v0CoordPos, CoordPos& v1CoordPos, CoordPos& v2CoordPos) const;

	/// Returns the altitude of the point on the plane defined by this triangle that has the same (X,Y) coordinates as
	/// the specified position. 
	/// \param pos    The 3D position of the point
	/// \pre    This object must be valid. Its validity is not checked. 
	KyFloat32 GetAltitudeOfPointInTriangle(const Vec3f& pos) const;
public:
	NavFloorPtr m_navFloorPtr; ///< The NavFloor that contains this triangle. 
	CompactNavTriangleIdx m_triangleIdx; ///< The index of this triangle within its NavFloor. 
};

KY_INLINE NavTrianglePtr::NavTrianglePtr() : m_triangleIdx(CompactNavTriangleIdx_MAXVAL) {}
KY_INLINE NavTrianglePtr::NavTrianglePtr(const NavFloorPtr& navFloorPtr, NavTriangleIdx triangleIdx) :
	m_navFloorPtr(navFloorPtr), m_triangleIdx((CompactNavTriangleIdx)triangleIdx) {}
KY_INLINE NavTrianglePtr::NavTrianglePtr(const NavTriangleRawPtr& navTriangleRawPtr) :
	m_navFloorPtr(navTriangleRawPtr.GetNavFloor()), m_triangleIdx((CompactNavTriangleIdx)navTriangleRawPtr.GetTriangleIdx()) {}

KY_INLINE void NavTrianglePtr::Set(const NavFloorPtr& navFloorPtr, NavTriangleIdx triangleIdx)
{
	m_navFloorPtr = navFloorPtr;
	m_triangleIdx = (CompactNavTriangleIdx)triangleIdx;
}

KY_INLINE bool NavTrianglePtr::IsValid() const { return m_navFloorPtr.IsValid() && m_triangleIdx != CompactNavTriangleIdx_MAXVAL; }

KY_INLINE void NavTrianglePtr::Invalidate() { m_navFloorPtr.Invalidate(); m_triangleIdx = CompactNavTriangleIdx_MAXVAL; }

KY_INLINE bool NavTrianglePtr::operator==(const NavTrianglePtr& rhs) const { return m_navFloorPtr == rhs.m_navFloorPtr && m_triangleIdx == rhs.m_triangleIdx; }
KY_INLINE bool NavTrianglePtr::operator!=(const NavTrianglePtr& rhs) const { return !(*this == rhs); }

KY_INLINE NavTriangleIdx NavTrianglePtr::GetTriangleIdx() const { return (NavTriangleIdx)m_triangleIdx; }

KY_INLINE NavFloor*           NavTrianglePtr::GetNavFloor()            const { return m_navFloorPtr.GetNavFloor();            }
KY_INLINE const NavFloorBlob* NavTrianglePtr::GetNavFloorBlob()        const { return m_navFloorPtr.GetNavFloorBlob();        }
KY_INLINE NavFloor*           NavTrianglePtr::GetNavFloor_Unsafe()     const { return m_navFloorPtr.GetNavFloor_Unsafe();     }
KY_INLINE const NavFloorBlob* NavTrianglePtr::GetNavFloorBlob_Unsafe() const { return m_navFloorPtr.GetNavFloorBlob_Unsafe(); }

KY_INLINE NavTriangleRawPtr NavTrianglePtr::GetRawPtr() const { return NavTriangleRawPtr(m_navFloorPtr.GetRawPtr(), m_triangleIdx); }

KY_INLINE const CellPos& NavTrianglePtr::GetCellPos()                                   const { return GetRawPtr().GetCellPos();                             }
KY_INLINE const NavTag&  NavTrianglePtr::GetNavTag()                                    const { return GetRawPtr().GetNavTag();                              }
KY_INLINE void           NavTrianglePtr::GetVerticesPos3f(Triangle3f& triangle3f)       const { return GetRawPtr().GetVerticesPos3f(triangle3f);             }
KY_INLINE KyFloat32      NavTrianglePtr::GetAltitudeOfPointInTriangle(const Vec3f& pos) const { return GetRawPtr().GetAltitudeOfPointInTriangle(pos);        }
KY_INLINE void NavTrianglePtr::GetVerticesPos3f(Vec3f& v0Pos3f, Vec3f& v1Pos3f, Vec3f& v2Pos3f) const
{
	return GetRawPtr().GetVerticesPos3f(v0Pos3f, v1Pos3f, v2Pos3f) ;
}
KY_INLINE void NavTrianglePtr::GetVerticesCoordPos64(CoordPos64& v0CoordPos64, CoordPos64& v1CoordPos64, CoordPos64& v2CoordPos64) const
{
	return GetRawPtr().GetVerticesCoordPos64(v0CoordPos64, v1CoordPos64, v2CoordPos64);
}
KY_INLINE void NavTrianglePtr::GetVerticesCoordPos(CoordPos& v0CoordPos, CoordPos& v1CoordPos, CoordPos& v2CoordPos) const
{
	return GetRawPtr().GetVerticesCoordPos(v0CoordPos, v1CoordPos, v2CoordPos);
}
}

#endif //Navigation_NavTrianglePtr_H

