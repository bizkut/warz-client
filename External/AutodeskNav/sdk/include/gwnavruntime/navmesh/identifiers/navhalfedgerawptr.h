/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavHalfEdgeRawPtr_H
#define Navigation_NavHalfEdgeRawPtr_H

#include "gwnavruntime/navmesh/identifiers/navfloorptr.h"
#include "gwnavruntime/navmesh/identifiers/navtrianglerawptr.h"
#include "gwnavruntime/navmesh/traverselogic.h"

namespace Kaim
{

///////////////////////////////////////////////////////////////////////////////////////////
// NavHalfEdgeRawPtr
///////////////////////////////////////////////////////////////////////////////////////////

/// Each instance of this class uniquely identifies a singleNavHalfEdge in a NavFloor.
/// This pointer is guaranteed to be valid only in the frame in which it was retrieved.
/// Never store a NavHalfEdgeRawPtr for use in subsequent frames, because it has no protection against data
/// streaming. Use NavHalfEdgePtr instead.
/// Actually, this class is used internally for some performance and working memory usage friendliness reason
/// (no ref-counting increment/decrement, no need to call constructor/destructor), but unless you really know what you do
/// prefer NavHalfEdgePtr which is safer.
class NavHalfEdgeRawPtr
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)

public:
	/// Creates an invalid NavHalfEdgeRawPtr. 
	NavHalfEdgeRawPtr();

	/// Creates a new NavHalfEdgePtr referring to the provided NavHalfEdgeIdx in the provided NavFloorRawPtr.
	/// \param navFloorRawPtr    The NavFloor that contains the edge this object should refer to. 
	/// \param halfEdgeIdx       The index of this edge within the NavFloor. 
	NavHalfEdgeRawPtr(const NavFloorRawPtr& navFloorRawPtr, NavHalfEdgeIdx halfEdgeIdx);

	/// Creates a new NavHalfEdgePtr referring to the provided NavHalfEdgeIdx in the provided NavFloor.
	/// \param navFloor       The NavFloor that contains the edge this object should refer to. 
	/// \param halfEdgeIdx    The index of this edge within the NavFloor. 
	NavHalfEdgeRawPtr(NavFloor* navFloor, NavHalfEdgeIdx halfEdgeIdx);


	/// Clears all information maintained by this object.
	/// \param navFloorRawPtr    The NavFloor that contains the edge this object should refer to. 
	/// \param halfEdgeIdx       The index of this edge within the NavFloor. 
	void Set(const NavFloorRawPtr& navFloorRawPtr, NavHalfEdgeIdx halfEdgeIdx);

	/// Clears all information maintained by this object.
	/// \param navFloor       The NavFloor that contains the edge this object should refer to. 
	/// \param halfEdgeIdx    The index of this edge within the NavFloor. 
	void Set(NavFloor* navFloor, NavHalfEdgeIdx halfEdgeIdx);

	/// Returns true if this object refers to a valid edge: i.e. an edge in a validNavFloor. see NavFloorPtr::IsValid()
	bool IsValid() const;

	void Invalidate(); ///< Invalidates this object.

	bool operator==(const NavHalfEdgeRawPtr& rhs) const; ///< Returns true if this object identifies the same edge as rhs, or if both are invalid. 
	bool operator!=(const NavHalfEdgeRawPtr& rhs) const; ///< Returns true if this object identifies a different edge from rhs. 
	
	NavHalfEdgeIdx GetHalfEdgeIdx() const;  ///< Retrieves the index of this NavHalfEdge within its NavFloor.

	// ---------------------------------- Member Functions for valid instance ----------------------------------
	/// All these function should be called only if IsValid() returns true
	/// \pre    This object must be valid. Its validity is not checked. 

	/// Returns a reference to the NavFloor that contains this hedge.
	/// \pre    This object must be valid. Its validity is not checked. 
	NavFloor* GetNavFloor() const;

	/// Returns a reference to the NavFloorBlob that contains this edge.
	/// \pre    This object must be valid. Its validity is not checked. 
	const NavFloorBlob* GetNavFloorBlob() const;

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

	/// Computes and returns the coordinates of the middle position of this NavHalfEdge
	/// \pre    This object must be valid. Its validity is not checked.
	Vec3f GetMiddlePos3fOfNavHalfEdge() const;

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
	template<class TraverseLogic>
	bool IsHalfEdgeCrossable(void* traverseLogicUserData) const;

	/// Returns true if this edge is not on an external boundary of the NavMesh and not a internal
	/// edge that is not correctly stitched.
	/// \pre    This object must be valid. Its validity is not checked.
	bool IsHalfEdgeCrossable() const;

	/// Returns true if this edge can be traversed according the TraverseLogic, and updates resultRawPtr.
	/// \param[out] resultRawPtr    The edge in the next adjacent triangle that borders this edge.
	/// \param traverseLogicUserData    A pointer to a traverseLogicUserData
	/// \pre    This object must be valid. Its validity is not checked.
	template<class TraverseLogic>
	bool IsHalfEdgeCrossable(void* traverseLogicUserData, NavHalfEdgeRawPtr& resultRawPtr) const;

	/// Returns true if this edge is not on an external boundary of the NavMesh and not a internal
	/// edge that is not correctly stitched, and updates resultRawPtr.
	/// \param[out] resultRawPtr    The edge in the next adjacent triangle that borders this edge.
	/// \pre    This object must be valid. Its validity is not checked.
	bool IsHalfEdgeCrossable(NavHalfEdgeRawPtr& resultRawPtr) const;

	/// Returns true if all the edges around the start vertex of this edge can be traversed accordingly
	/// to the TraverseLogic.
	/// \param traverseLogicUserData    A pointer to a traverseLogicUserData
	/// \pre    This object must be valid. Its validity is not checked.
	template<class TraverseLogic>
	bool IsStartVertexOnBorder(void* traverseLogicUserData) const;

	/// Returns true if all the edges around the start vertex of this edge are neither an external boundary of
	/// the NavMesh nor an internal edge that is not correctly stitched.
	/// \pre    This object must be valid. Its validity is not checked.
	bool IsStartVertexOnBorder() const;

	/// Returns true if all the edges around the end vertex of this edge can be traversed accordingly
	/// to the TraverseLogic.
	/// \param traverseLogicUserData    A pointer to a traverseLogicUserData
	/// \pre    This object must be valid. Its validity is not checked.
	template<class TraverseLogic>
	bool IsEndVertexOnBorder(void* traverseLogicUserData) const;

	/// Returns true if all the edges around the end vertex of this edge are neither an external boundary of
	/// the NavMesh nor an internal edge that is not correctly stitched.
	/// \pre    This object must be valid. Its validity is not checked.
	bool IsEndVertexOnBorder() const;

	/// Updates resultRawPtr to identify the edge in the next adjacent triangle that borders this edge.
	/// \param[out] resultRawPtr    Updated to store the next adjacent triangle.
	/// \pre    This object must be valid. Its validity is not checked.
	/// \pre    The type of the edge returned by a call to NavHalfEdge::GetHalfEdgeType() must not be #EDGETYPE_OBSTACLE.
	void GetPairHalfEdgeRawPtr(NavHalfEdgeRawPtr& resultRawPtr) const;

	/// Updates resultRawPtr to identify the next edge in the triangle.
	/// \pre    This object must be valid. Its validity is not checked.
	void GetNextHalfEdgeRawPtr(NavHalfEdgeRawPtr& resultRawPtr) const;

	/// Updates resultRawPtr to identify the previous edge in the triangle.
	/// \pre    This object must be valid. Its validity is not checked. 
	void GetPrevHalfEdgeRawPtr(NavHalfEdgeRawPtr& resultRawPtr) const;

	/// Returns the next edge along the border of the NavMesh according to TraverseLogic.
	/// \param traverseLogicUserData    A pointer to a traverseLogicUserData
	/// \pre    This object must be valid. Its validity is not checked.
	/// \pre    This object must be on the border of an area of the NavMesh according to TraverseLogic. It may not be paired with
	///         another triangle with a traversable NavTag. That means we must have IsHalfEdgeCrossable<TraverseLogic>() == false.
	/// \post    The returned edge is not paired with another triangle with a traversable NavTag (IsHalfEdgeCrossable<TraverseLogic>() == false).
	template<class TraverseLogic>
	NavHalfEdgeRawPtr GetNextNavHalfEdgeRawPtrAlongBorder(void* traverseLogicUserData) const;

	/// Returns the previous edge along the border of the given NavTag.
	/// \param traverseLogicUserData    A pointer to a traverseLogicUserData
	/// \pre    This object must be valid. Its validity is not checked.
	/// \pre    This object must be on the border of an area of the NavMesh according to TraverseLogic. It may not be paired with
	/// 		another triangle with a traversable NavTagv. That means we must have IsHalfEdgeCrossable<TraverseLogic>() == false.
	/// \post	The returned edge is not paired with another triangle with a traversable NavTag (IsHalfEdgeCrossable<TraverseLogic>() == false).
	template<class TraverseLogic>
	NavHalfEdgeRawPtr GetPrevNavHalfEdgeRawPtrAlongBorder(void* traverseLogicUserData) const;

public: // internal
	NavHalfEdgeRawPtr& GetCorrespondingLink();

	template<class TraverseLogic>
	bool IsHalfEdgeCrossable(void* traverseLogicUserData, const NavFloorBlob* navFloorBlob, NavHalfEdgeRawPtr& resultRawPtr) const;
	template<class TraverseLogic>
	bool IsHalfEdgeCrossable(void* traverseLogicUserData, NavHalfEdge navHalfEdge, const NavFloorBlob* navFloorBlob,
		NavHalfEdgeRawPtr& resultRawPtr) const;
	template<class TraverseLogic>
	bool IsHalfEdgeCrossable(void* traverseLogicUserData, NavHalfEdge navHalfEdge, const NavFloorBlob* navFloorBlob,
		NavHalfEdgeRawPtr& resultRawPtr, KyFloat32* costMultiplier) const;
	template<class TraverseLogic>
	bool IsHalfEdgeOneWayCrossable(void* traverseLogicUserData, const Vec3f* position) const;
	template<class TraverseLogic>
	bool IsHalfEdgeOneWayCrossable(void* traverseLogicUserData, NavHalfEdgeRawPtr& resultRawPtr, const Vec3f* position) const;
	template<class TraverseLogic>
	bool IsHalfEdgeOneWayCrossable(void* traverseLogicUserData, const NavFloorBlob* navFloorBlob, 
		NavHalfEdgeRawPtr& resultRawPtr, const Vec3f* position) const;
	template<class TraverseLogic>
	bool IsHalfEdgeOneWayCrossable(void* traverseLogicUserData, NavHalfEdge navHalfEdge, const NavFloorBlob* navFloorBlob,
		NavHalfEdgeRawPtr& resultRawPtr, const Vec3f* position, KyFloat32* costMultiplier) const;
	template<class TraverseLogic>
	bool IsHalfEdgeOneWayCrossable(void* traverseLogicUserData, NavHalfEdge navHalfEdge, const NavFloorBlob* navFloorBlob,
		NavHalfEdgeRawPtr& resultRawPtr, KyFloat32* costMultiplier) const;

	template<class TraverseLogic>
	bool IsPairedEdgeCrossable(void* traverseLogicUserData, const NavFloorBlob* navFloorBlob, NavTriangleIdx pairTriangleIdx,
		KyFloat32* /*costMultiplier*/, const LogicWithoutCostMultipler&) const;
	template<class TraverseLogic>
	bool IsPairedEdgeCrossable(void* traverseLogicUserData, const NavFloorBlob* navFloorBlob, NavTriangleIdx pairTriangleIdx,
		KyFloat32* costMultiplier, const LogicWithCostMultiplerPerNavTag&) const;
	template<class TraverseLogic>
	bool IsPairedEdgeCrossable(void* traverseLogicUserData, const NavFloorBlob* navFloorBlob, NavTriangleIdx pairTriangleIdx,
		KyFloat32* costMultiplier, const LogicWithCostMultiplerPerTriangle&) const;

	template<class TraverseLogic>
	bool IsConnexBoundaryCrossable(void* traverseLogicUserData, const NavFloorBlob* navFloorBlob, NavTriangleIdx pairTriangleIdx,
		KyFloat32* /*costMultiplier*/, const LogicWithoutCostMultipler&) const;
	template<class TraverseLogic>
	bool IsConnexBoundaryCrossable(void* traverseLogicUserData, const NavFloorBlob* navFloorBlob, NavTriangleIdx pairTriangleIdx,
		KyFloat32* costMultiplier, const LogicWithCostMultiplerPerNavTag&) const;
	template<class TraverseLogic>
	bool IsConnexBoundaryCrossable(void* traverseLogicUserData, const NavFloorBlob* navFloorBlob, NavTriangleIdx pairTriangleIdx,
		KyFloat32* costMultiplier, const LogicWithCostMultiplerPerTriangle&) const;

	template<class TLogic>
	bool IsFloorOrCellLinkCrossable(void* traverseLogicUserData, const NavHalfEdgeRawPtr& link, KyFloat32* costMultiplier,
		const LogicWithoutCostMultipler&) const;
	template<class TLogic>
	bool IsFloorOrCellLinkCrossable(void* traverseLogicUserData, const NavHalfEdgeRawPtr& link, KyFloat32* costMultiplier,
		const LogicWithCostMultiplerPerNavTag&) const;
	template<class TLogic>
	bool IsFloorOrCellLinkCrossable(void* traverseLogicUserData, const NavHalfEdgeRawPtr& link, KyFloat32* costMultiplier,
		const LogicWithCostMultiplerPerTriangle&) const;

	template<class TraverseLogic>
	bool IsHalfEdgeOneWayCrossable(void* traverseLogicUserData, NavHalfEdge navHalfEdge, const NavFloorBlob* navFloorBlob, NavHalfEdgeRawPtr& resultRawPtr,
		const Vec3f* position, KyFloat32* costMultiplier, const LogicDoNotUseCanEnterNavTag&) const;
	template<class TraverseLogic>
	bool IsHalfEdgeOneWayCrossable(void* traverseLogicUserData, NavHalfEdge navHalfEdge, const NavFloorBlob* navFloorBlob, NavHalfEdgeRawPtr& resultRawPtr,
		const Vec3f* position, KyFloat32* costMultiplier, const LogicDoUseCanEnterNavTag&) const;

	void GetVerticesPos3f(KyFloat32 integerPrecision, const CoordPos64& cellOrigin, const NavFloorBlob* navFloorBlob, Vec3f& v0Pos3f, Vec3f& v1Pos3f) const;
public:
	NavFloorRawPtr m_navFloorRawPtr;     ///< The NavFloor that contains this edge.
	CompactNavHalfEdgeIdx m_halfEdgeIdx; ///< The index of this edge within its NavFloor.

	KyUInt16 m_boundaryEdgeIdx; ///< Use internally to speed up stitching. Do not modify.
};

}

#include "gwnavruntime/navmesh/identifiers/navhalfedgerawptr.inl"

#endif //Navigation_NavHalfEdgeRawPtr_H

