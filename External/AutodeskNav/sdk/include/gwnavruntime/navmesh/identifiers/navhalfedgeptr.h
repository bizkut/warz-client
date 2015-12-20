/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavHalfEdgePtr_H
#define Navigation_NavHalfEdgePtr_H

#include "gwnavruntime/navmesh/identifiers/navhalfedgerawptr.h"

namespace Kaim
{

/// Each instance of this class uniquely identifies a single NavHalfEdge in a NavFloor.
/// Instances of this class can be safely stored and used in subsequent frames.
class NavHalfEdgePtr
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	NavHalfEdgePtr(); ///< Creates an invalid NavHalfEdgePtr. 

	/// Creates a new NavHalfEdgePtr referring to the provided NavHalfEdgeIdx in the provided NavFloorPtr.
	/// \param navFloorPtr    The NavFloor that contains the edge this object should refer to.
	/// \param halfEdgeIdx    The index of this edge within the NavFloor.
	NavHalfEdgePtr(const NavFloorPtr& navFloorPtr, NavHalfEdgeIdx halfEdgeIdx);

	/// Creates a new NavHalfEdgePtr copying information from the provided one.
	explicit NavHalfEdgePtr(const NavHalfEdgeRawPtr& navHalfEdgeRawPtr);

	/// Clears all information maintained by this object.
	/// \param navFloorPtr    The NavFloor that contains the edge this object should refer to.
	/// \param halfEdgeIdx    The index of this edge within the NavFloor. 
	void Set(const NavFloorPtr& navFloorPtr, NavHalfEdgeIdx halfEdgeIdx);

	/// Returns true if this object refers to a valid edge: i.e. an edge  in a valid NavFloor. see NavFloorPtr::IsValid()
	bool IsValid() const;

	void Invalidate(); ///< Invalidates this object.

	bool operator==(const NavHalfEdgePtr& rhs) const; ///< Returns true if this object identifies the same edge as rhs, or if both are invalid. 
	bool operator!=(const NavHalfEdgePtr& rhs) const; ///< Returns true if this object identifies a different edge from rhs. 

	NavHalfEdgeIdx      GetHalfEdgeIdx()  const; ///< Returns the index of this edge within its NavFloor. 
	NavFloor*           GetNavFloor()     const; ///< Returns a pointer to the NavFloor that contains this edge. Returns KY_NULL if this object is not valid.
	const NavFloorBlob* GetNavFloorBlob() const; ///< Returns a const pointer to the NavFloorBlob that contains this edge. Returns KY_NULL if this object is not valid.


	// ---------------------------------- Member Functions for valid instance ----------------------------------
	/// All these function should be called only if IsValid() returns true
	/// \pre    This object must be valid. Its validity is not checked. 

	/// Returns a pointer to the NavFloor that contains this edge.
	/// \pre    This object must be valid. Its validity is not checked. 
	NavFloor*GetNavFloor_Unsafe() const;

	/// Returns a const pointer to the NavFloorBlob that contains this edge.
	/// \pre    This object must be valid. Its validity is not checked. 
	const NavFloorBlob* GetNavFloorBlob_Unsafe() const;

	/// Constructs and returns a NavHalfEdgeRawPtr that refers to the same NavHalfEdge
	/// \pre    This object must be valid. Its validity is not checked. 
	NavHalfEdgeRawPtr GetRawPtr() const;

	/// Returns a const reference to the CellPos that indicates the position of the cell that contains this edge
	/// within the grid of NavData cells.
	/// \pre    This object must be valid. Its validity is not checked. 
	const CellPos& GetCellPos() const;

	/// Retrieves the NavTag associated to the NavFloor that contains this halfEdge.
	/// \pre    This object must be valid. Its validity is not checked. 
	const NavTag& GetNavTag() const;

	/// Updates the parameters to identify the vertices in the specified triangle.
	/// \param[out] v0Pos3f    The 3D position of the start vertex of the edge.
	/// \param[out] v1Pos3f    The 3D position of the end vertex in the edge.
	/// \param[out] v2Pos3f    The 3D position of the third vertex in the triangle.
	/// \pre    This object must be valid. Its validity is not checked. 
	void GetTriangleVerticesPos3f(Vec3f& v0Pos3f, Vec3f& v1Pos3f, Vec3f& v2Pos3f) const;

	/// Updates the parameters to identify the start and end vertices of the specified edge.
	/// \param[out] v0Pos3f    The 3D position of the start vertex of the edge.
	/// \param[out] v1Pos3f    The 3D position of the end vertex in the edge.
	/// \pre    This object must be valid. Its validity is not checked. 
	void GetVerticesPos3f(Vec3f& v0Pos3f, Vec3f& v1Pos3f) const;

	/// Returns the coordinates of the starting vertex of this NavHalfEdge
	/// \pre    This object must be valid. Its validity is not checked.
	Vec3f GetStartVertexPos3f() const;

	/// Returns the coordinates of the ending vertex of this NavHalfEdge
	/// \pre    This object must be valid. Its validity is not checked.
	Vec3f GetEndVertexPos3f() const;

	/// Returns the x,y integer coordinates of the starting vertex of this NavHalfEdge
	/// \pre    This object must be valid. Its validity is not checked.
	CoordPos64 GetStartVertexCoordPos64() const;

	/// Returns the x,y integer coordinates of the ending vertex of this NavHalfEdge
	/// \pre    This object must be valid. Its validity is not checked.
	CoordPos64 GetEndVertexCoordPos64() const;

	/// Returns the altitude (z coordinate) of the starting vertex of this NavHalfEdge
	/// \pre    This object must be valid. Its validity is not checked 
	KyFloat32 GetStartVertexAltitude() const;

	/// Returns the altitude (z coordinate) of the ending vertex of this NavHalfEdge
	/// \pre    This object must be valid. Its validity is not checked 
	KyFloat32 GetEndVertexAltitude() const;

	/// Returns the 2D integer coordinates of the starting vertex of this edge in its NavCell.
	/// \pre    This object must be valid. Its validity is not checked. 
	CoordPos GetStartVertexCoordPosInCell() const;

	/// Returns the 2D integer coordinates of the ending vertex of this edge in its NavCell.
	/// \pre    This object must be valid. Its validity is not checked. 
	CoordPos GetEndVertexCoordPosInCell() const;

	/// Returns true if this edge can be traversed according the TraverseLogic.
	/// \param traverseLogicUserData    A pointer to a traverseLogicUserData
	/// \pre    This object must be valid. Its validity is not checked.
	template <class TraverseLogic>
	bool IsHalfEdgeCrossable(void* traverseLogicUserData) const;

	/// Returns true if this edge is not on an external boundary of the NavMesh and not a internal
	/// edge that is not correctly stitched.
	/// \pre    This object must be valid. Its validity is not checked.
	bool IsHalfEdgeCrossable() const;
	 
	/// Returns true if this edge can be traversed according the TraverseLogic
	///  and updates resultPtr.
	/// \param[out] resultPtr    The edge in the next adjacent triangle that borders this edge.
	/// \param traverseLogicUserData         A pointer to a traverseLogicUserData
	/// \pre    This object must be valid. Its validity is not checked.
	template <class TraverseLogic>
	bool IsHalfEdgeCrossable(NavHalfEdgePtr& resultPtr, void* traverseLogicUserData) const;

	/// Returns true if this edge is not on an external boundary of the NavMesh and not a internal
	///  edge that is not correctly stitched, and updates resultPtr.
	/// \param[out] resultPtr    The edge in the next adjacent triangle that borders this edge.
	/// \pre    This object must be valid. Its validity is not checked.
	bool IsHalfEdgeCrossable(NavHalfEdgePtr& resultPtr) const;

	/// Updates resultPtr to identify the edge in the next adjacent triangle that borders this edge.
	/// \pre    This object must be valid. Its validity is not checked.
	/// \pre    The type of the edge returned by a call to NavHalfEdge::GetHalfEdgeType() must not be #EDGETYPE_OBSTACLE.
	void GetPairHalfEdgePtr(NavHalfEdgePtr& resultPtr) const;

	/// Updates resultPtr to identify the next edge in the triangle.
	/// \pre    This object must be valid. Its validity is not checked.
	void GetNextHalfEdgePtr(NavHalfEdgePtr& resultPtr) const;

	/// Updates resultPtr to identify the previous edge in the triangle.
	/// \pre    This object must be valid. Its validity is not checked. 
	void GetPrevHalfEdgePtr(NavHalfEdgePtr& resultPtr) const;

	/// Returns the next edge along the border of the NavMesh according to TraverseLogic.
	/// \param traverseLogicUserData    A pointer to a traverseLogicUserData
	/// \pre    This object must be valid. Its validity is not checked.
	/// \pre    This object must be on the border of an area of the NavMesh according to TraverseLogic. It may not be paired with
	///          another triangle with a traversable NavTag. That means we must have <code>IsHalfEdgeCrossable<TraverseLogic>() == false</code>.
	/// \post    The returned edge is not paired with another triangle with a traversable NavTag.
	template<class TraverseLogic>
	NavHalfEdgePtr GetNextNavHalfEdgePtrAlongBorder(void* traverseLogicUserData) const;

	/// Returns the previous edge along the border of the NavMesh according to TraverseLogic.
	/// \param traverseLogicUserData    A pointer to a traverseLogicUserData
	/// \pre    This object must be valid. Its validity is not checked.
	/// \pre    This object must be on the border of an area of the NavMesh according to TraverseLogic. It may not be paired with
	///          another triangle with a traversable NavTag. That means we must have <code>IsHalfEdgeCrossable<TraverseLogic>() == false</code>.
	/// \post    The returned edge is not paired with another triangle with a traversable NavTag.
	template<class TraverseLogic>
	NavHalfEdgePtr GetPrevNavHalfEdgePtrAlongBorder(void* traverseLogicUserData) const;

public:
	NavFloorPtr m_navFloorPtr;           ///< The NavFloor that contains this edge.
	CompactNavHalfEdgeIdx m_halfEdgeIdx; ///< The index of this edge within its NavFloor.
};

KY_INLINE NavHalfEdgePtr::NavHalfEdgePtr() : m_halfEdgeIdx(CompactNavHalfEdgeIdx_MAXVAL) {}

KY_INLINE NavHalfEdgePtr::NavHalfEdgePtr(const NavFloorPtr& navFloorPtr, NavHalfEdgeIdx halfEdgeIdx) :
	m_navFloorPtr(navFloorPtr), m_halfEdgeIdx((CompactNavHalfEdgeIdx)halfEdgeIdx) {}

KY_INLINE NavHalfEdgePtr::NavHalfEdgePtr(const NavHalfEdgeRawPtr& navHalfEdgeRawPtr) :
	m_navFloorPtr(navHalfEdgeRawPtr.GetNavFloor()), m_halfEdgeIdx(navHalfEdgeRawPtr.m_halfEdgeIdx) {}

KY_INLINE void NavHalfEdgePtr::Set(const NavFloorPtr& navFloorPtr, NavHalfEdgeIdx halfEdgeIdx)
{
	m_navFloorPtr = navFloorPtr;
	m_halfEdgeIdx = (CompactNavHalfEdgeIdx)halfEdgeIdx;
}

KY_INLINE bool NavHalfEdgePtr::IsValid() const { return m_navFloorPtr.IsValid() && m_halfEdgeIdx != CompactNavHalfEdgeIdx_MAXVAL; }

KY_INLINE void NavHalfEdgePtr::Invalidate() { m_navFloorPtr.Invalidate(); m_halfEdgeIdx = CompactNavHalfEdgeIdx_MAXVAL; }

KY_INLINE bool NavHalfEdgePtr::operator==(const NavHalfEdgePtr& rhs) const { return m_navFloorPtr == rhs.m_navFloorPtr && m_halfEdgeIdx == rhs.m_halfEdgeIdx; }
KY_INLINE bool NavHalfEdgePtr::operator!=(const NavHalfEdgePtr& rhs) const { return !(*this == rhs); }

KY_INLINE NavHalfEdgeIdx NavHalfEdgePtr::GetHalfEdgeIdx() const { return (NavHalfEdgeIdx)m_halfEdgeIdx; }

KY_INLINE NavFloor*           NavHalfEdgePtr::GetNavFloor()            const { return m_navFloorPtr.GetNavFloor();            }
KY_INLINE const NavFloorBlob* NavHalfEdgePtr::GetNavFloorBlob()        const { return m_navFloorPtr.GetNavFloorBlob();        }
KY_INLINE NavFloor*           NavHalfEdgePtr::GetNavFloor_Unsafe()     const { return m_navFloorPtr.GetNavFloor_Unsafe();     }
KY_INLINE const NavFloorBlob* NavHalfEdgePtr::GetNavFloorBlob_Unsafe() const { return m_navFloorPtr.GetNavFloorBlob_Unsafe(); }

KY_INLINE NavHalfEdgeRawPtr NavHalfEdgePtr::GetRawPtr() const { return NavHalfEdgeRawPtr(m_navFloorPtr.GetRawPtr(), m_halfEdgeIdx); }

KY_INLINE const CellPos& NavHalfEdgePtr::GetCellPos()                   const { return GetRawPtr().GetCellPos();                   }
KY_INLINE const NavTag&  NavHalfEdgePtr::GetNavTag()                    const { return GetRawPtr().GetNavTag();                    }
KY_INLINE Vec3f          NavHalfEdgePtr::GetStartVertexPos3f()          const { return GetRawPtr().GetStartVertexPos3f();          }
KY_INLINE Vec3f          NavHalfEdgePtr::GetEndVertexPos3f()            const { return GetRawPtr().GetEndVertexPos3f();            }
KY_INLINE CoordPos64     NavHalfEdgePtr::GetStartVertexCoordPos64()     const { return GetRawPtr().GetStartVertexCoordPos64();     }
KY_INLINE CoordPos64     NavHalfEdgePtr::GetEndVertexCoordPos64()       const { return GetRawPtr().GetEndVertexCoordPos64();       }
KY_INLINE KyFloat32      NavHalfEdgePtr::GetStartVertexAltitude()       const { return GetRawPtr().GetStartVertexAltitude();       }
KY_INLINE KyFloat32      NavHalfEdgePtr::GetEndVertexAltitude()         const { return GetRawPtr().GetEndVertexAltitude();         }
KY_INLINE CoordPos       NavHalfEdgePtr::GetStartVertexCoordPosInCell() const { return GetRawPtr().GetStartVertexCoordPosInCell(); }
KY_INLINE CoordPos       NavHalfEdgePtr::GetEndVertexCoordPosInCell()   const { return GetRawPtr().GetEndVertexCoordPosInCell();   }
KY_INLINE bool           NavHalfEdgePtr::IsHalfEdgeCrossable()          const { return GetRawPtr().IsHalfEdgeCrossable();          }

KY_INLINE void NavHalfEdgePtr::GetTriangleVerticesPos3f(Vec3f& v0Pos3f, Vec3f& v1Pos3f, Vec3f& v2Pos3f) const
{
	return GetRawPtr().GetTriangleVerticesPos3f(v0Pos3f, v1Pos3f, v2Pos3f);
}
KY_INLINE void NavHalfEdgePtr::GetVerticesPos3f(Vec3f& v0Pos3f, Vec3f& v1Pos3f) const
{
	return GetRawPtr().GetVerticesPos3f(v0Pos3f, v1Pos3f);
}
template<class TraverseLogic>
KY_INLINE bool NavHalfEdgePtr::IsHalfEdgeCrossable(void* traverseLogicUserData) const
{
	return GetRawPtr().IsHalfEdgeCrossable<TraverseLogic>(traverseLogicUserData);
}

template<class TraverseLogic>
KY_INLINE NavHalfEdgePtr NavHalfEdgePtr::GetNextNavHalfEdgePtrAlongBorder(void* traverseLogicUserData) const
{
	return NavHalfEdgePtr(GetRawPtr().GetNextNavHalfEdgeRawPtrAlongBorder<TraverseLogic>(traverseLogicUserData));
}

template<class TraverseLogic>
KY_INLINE NavHalfEdgePtr NavHalfEdgePtr::GetPrevNavHalfEdgePtrAlongBorder(void* traverseLogicUserData) const
{
	return NavHalfEdgePtr(GetRawPtr().GetPrevNavHalfEdgeRawPtrAlongBorder<TraverseLogic>(traverseLogicUserData));
}

inline bool NavHalfEdgePtr::IsHalfEdgeCrossable(NavHalfEdgePtr& resultPtr) const
{
	NavHalfEdgeRawPtr resultRawPtr;
	bool storeResult = GetRawPtr().IsHalfEdgeCrossable(resultRawPtr);
	resultPtr = NavHalfEdgePtr(resultRawPtr);
	return storeResult;
}

inline void NavHalfEdgePtr::GetPairHalfEdgePtr(NavHalfEdgePtr& resultPtr) const
{
	NavHalfEdgeRawPtr resultRawPtr;
	GetRawPtr().GetPairHalfEdgeRawPtr(resultRawPtr);
	resultPtr = NavHalfEdgePtr(resultRawPtr);
}

inline void NavHalfEdgePtr::GetNextHalfEdgePtr(NavHalfEdgePtr& resultPtr) const
{
	NavHalfEdgeRawPtr resultRawPtr;
	GetRawPtr().GetNextHalfEdgeRawPtr(resultRawPtr);
	resultPtr = NavHalfEdgePtr(resultRawPtr);
}

inline void NavHalfEdgePtr::GetPrevHalfEdgePtr(NavHalfEdgePtr& resultPtr) const
{
	NavHalfEdgeRawPtr resultRawPtr;
	GetRawPtr().GetPrevHalfEdgeRawPtr(resultRawPtr);
	resultPtr = NavHalfEdgePtr(resultRawPtr);
}

template<class TraverseLogic>
inline bool NavHalfEdgePtr::IsHalfEdgeCrossable(NavHalfEdgePtr& resultPtr, void* traverseLogicUserData) const
{
	NavHalfEdgeRawPtr resultRawPtr;
	bool storeResult = GetRawPtr().IsHalfEdgeCrossable<TraverseLogic>(resultRawPtr, traverseLogicUserData);
	resultPtr = NavHalfEdgePtr(resultRawPtr);
	return storeResult;
}



}
#endif //Navigation_NavHalfEdgePtr_H

