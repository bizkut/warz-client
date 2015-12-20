/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavGraphEdgeRawPtr_H
#define Navigation_NavGraphEdgeRawPtr_H

#include "gwnavruntime/navgraph/identifiers/navgraphvertexrawptr.h"

namespace Kaim
{

class NavTag;

/// Each instance of this class uniquely identifies a single NavGraphEdge in a NavGraph.
/// This pointer is guaranteed to be valid only in the frame in which it was retrieved.
/// Never store a NavGraphEdgeRawPtr for use in subsequent frames, because it has no protection against data
/// streaming. Use NavGraphEdgePtr instead.
/// Actually, this class is used internally for some performance and working memory usage friendliness reason
/// (no ref-counting increment/decrement, no need to call constructor/destructor), but unless you really know what you do
/// prefer NavGraphEdgePtr which is safer.
/// Within a NavGraph, a NavGraphEdge is identified by a NavGraphVertexIdx (corresponding to its starting vertex) and the
///  index of a Neighbor vertex of this starting vertex among all its neighbor vertices (this neighbor vertex corresponds 
///  to its starting vertex).
class NavGraphEdgeRawPtr
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)

public:
	NavGraphEdgeRawPtr(); ///< Constructs a new instance of this class. Creates an invalid NavGraphEdgeRawPtr. 
	NavGraphEdgeRawPtr(const NavGraphRawPtr& navGraphRawPtr, NavGraphEdgeSmartIdx edgeSmartIdx);
	NavGraphEdgeRawPtr(const NavGraphVertexRawPtr& navGraphVertexRawPtr, KyUInt32 neighborIndex);

	/// Returns true if this object refers to a valid NavGraphEdge: i.e. a NavGraphEdge in a valid NavGraph. see NavGraph::IsValid().
	bool IsValid() const;

	void Invalidate(); ///< Invalidates this object. 

	bool operator ==(const NavGraphEdgeRawPtr& rhs) const; ///< Returns true if this object identifies the same edge as rhs, or if both are invalid. 
	bool operator !=(const NavGraphEdgeRawPtr& rhs) const; ///< Returns true if this object identifies an edge different from the one identified by rhs, or if only one of them is invalid. 
	bool operator < (const NavGraphEdgeRawPtr& rhs) const;
	bool operator <=(const NavGraphEdgeRawPtr& rhs) const;

	// ---------------------------------- Member Functions for valid instance ----------------------------------
	/// All these function should be called only if IsValid() returns true.
	/// \pre    This object must be valid. Its validity is not checked.

	/// Returns the index of the edge around its starting vertex. It is the index of the ending vertex within the neighbor vertices of the starting vertex.
	/// \pre    This object must be valid. Its validity is not checked.
	KyUInt32 GetEdgeNumberAroundStartVertex() const;

	/// Return the opposite NavGraphEdgeRawPtr, if any exists.
	/// If no opposite NavGraphEdgeRawPtr exists (the edge is not "bi-directional"), an invalid NavGraphEdgePtr is returned
	/// \pre    This object must be valid. Its validity is not checked. 
	NavGraphEdgeRawPtr GetOppositeNavGraphEdgeRawPtr() const;

	NavGraph*             GetNavGraph()                    const; ///< Returns a reference to the NavGraph that contains this NavGraphEdge. \pre This object must be valid. Its validity is not checked.
	const NavGraphBlob*   GetNavGraphBlob()                const; ///< Returns a reference to the NavGraphBlob that contains this NavGraphEdge. \pre This object must be valid. Its validity is not checked.
	NavGraphVertexIdx     GetStartVertexIdx()              const; ///< Returns the index of the starting NavGraphVertex of current instance within its NavGraph. \pre This object must be valid. Its validity is not checked.
	NavGraphVertexIdx     GetEndNavGraphVertexIdx()        const; ///< Returns the index of the ending NavGraphVertex of current instance within its NavGraph. \pre This object must be valid. Its validity is not checked.
	NavGraphVertexRawPtr  GetStartNavGraphVertexRawPtr()   const; ///< Returns a NavGraphVertexRawPtr that identifies the starting NavGraphVertex of current instance. \pre This object must be valid. Its validity is not checked.
	NavGraphVertexRawPtr  GetEndNavGraphVertexRawPtr()     const; ///< Returns a NavGraphVertexRawPtr that identifies the ending NavGraphVertex of current instance. \pre This object must be valid. Its validity is not checked.
	const NavGraphVertex& GetStartNavGraphVertex()         const; ///< Returns the starting NavGraphVertex of current instance. \pre This object must be valid. Its validity is not checked.
	const NavGraphVertex& GetEndNavGraphVertex()           const; ///< Returns the ending NavGraphVertex of current instance. \pre This object must be valid. Its validity is not checked.
	const Vec3f&          GetStartNavGraphVertexPosition() const; ///< Returns the position of the starting NavGraphVertex of current instance. \pre This object must be valid. Its validity is not checked.
	const Vec3f&          GetEndNavGraphVertexPosition()   const; ///< Returns the position of the ending NavGraphVertex of current instance. \pre This object must be valid. Its validity is not checked.
	const NavTag&         GetNavTag()                      const; ///< Returns the NavTag associated to this NavGraphEdge. \pre This object must be valid. Its validity is not checked.

	template<class TraverseLogic>
	bool CanBeTraversed(void* traverseLogicUserData, KyFloat32* costMultiplier = KY_NULL) const;

public: // internal
	template<class TraverseLogic>
	bool CanBeTraversed(void* traverseLogicUserData, KyFloat32* costMultiplier, const LogicWithoutCostMultipler&) const;
	template<class TraverseLogic>
	bool CanBeTraversed(void* traverseLogicUserData, KyFloat32* costMultiplier, const LogicWithCostMultiplerPerNavTag&) const;
	template<class TraverseLogic>
	bool CanBeTraversed(void* traverseLogicUserData, KyFloat32* costMultiplier, const LogicWithCostMultiplerPerTriangle&) const;

public:
	NavGraphRawPtr m_navGraphRawPtr;
	NavGraphEdgeSmartIdx m_edgeSmartIdx;
};

}

#include "gwnavruntime/navgraph/identifiers/navgraphedgerawptr.inl"

#endif //Navigation_NavGraphEdgeRawPtr_H

