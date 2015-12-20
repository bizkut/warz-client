/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/




// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavGraphVertexRawPtr_H
#define Navigation_NavGraphVertexRawPtr_H

#include "gwnavruntime/navgraph/identifiers/navgraphrawptr.h"
#include "gwnavruntime/navgraph/navgraph.h"
#include "gwnavruntime/navgraph/navgraphtypes.h"


namespace Kaim
{

class NavGraphVertex;
class GraphVertexData;
class NavTag;

/// Each instance of this class uniquely identifies a single NavGraphVertex in a NavGraph.
/// This pointer is guaranteed to be valid only in the frame in which it was retrieved.
/// Never store a NavGraphVertexRawPtr for use in subsequent frames, because it has no protection against data
/// streaming. Use NavGraphVertexPtr instead.
/// Actually, this class is used internally for some performance and working memory usage friendliness reason
/// (no ref-counting increment/decrement, no need to call constructor/destructor), but unless you really know what you do
/// prefer NavGraphVertexPtr which is safer.
class NavGraphVertexRawPtr
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)

public:
	NavGraphVertexRawPtr(); ///< Constructs a new instance of this class. Creates an invalid NavGraphVertexRawPtr. 

	/// Construct a new instance of NavGraphVertexPtr, referring to the vertexIdx vertex of the NavGraph identified by navGraphRawPtr.
	NavGraphVertexRawPtr(const NavGraphRawPtr& navGraphRawPtr, NavGraphVertexIdx vertexIdx);

	/// Returns true if this object refers to a valid NavGraphVertex: i.e. a NavGraphVertex in a validNavGraph. see NavGraph::IsValid().
	bool IsValid() const;

	void Invalidate(); ///< Invalidates this object. 
	
	bool operator ==(const NavGraphVertexRawPtr& rhs) const; ///< Returns true if this object identifies the same vertex as rhs, or if both are invalid.
	bool operator !=(const NavGraphVertexRawPtr& rhs) const; ///< Returns true if this object identifies an vertex different from the one identified by rhs, or if only one of them is invalid.
	bool operator < (const NavGraphVertexRawPtr& rhs) const;
	bool operator <=(const NavGraphVertexRawPtr& rhs) const;
	
	// ---------------------------------- Member Functions for valid instance ----------------------------------
	/// All these function should be called only if IsValid() returns true.
	/// \pre    This object must be valid. Its validity is not checked. 

	NavGraphVertexIdx     GetNavGraphVertexIdx()   const; ///< Returns the NavGraphVertexIdx that identifies this vertex within its NavGraph. \pre This object must be valid. Its validity is not checked.
	NavGraph*             GetNavGraph()            const; ///< Returns a reference to the NavGraph that contains this NavGraphVertex. \pre This object must be valid. Its validity is not checked.
	const NavGraphBlob*   GetNavGraphBlob()        const; ///< Returns a reference to the NavGraphBlob that contains this NavGraphVertex. \pre This object must be valid. Its validity is not checked.
	KyUInt32              GetNeighborVertexCount() const; ///< Returns the number of outgoing edges (equal to the number of neighbors) of this vertex. \pre This object must be valid. Its validity is not checked.
	GraphVertexData&      GetGraphVertexData()     const; ///< Returns theGraphVertexData associated to this NavGraphVertex. For Internal use. \pre This object must be valid. Its validity is not checked.
	const NavTag&         GetNavTag()              const; ///< Returns the NavTag associated to this NavGraphVertex. \pre This object must be valid. Its validity is not checked.
	const NavGraphVertex& GetNavGraphVertex()      const; ///< Retrieves the NavGraph vertex at the start of the specified NavGraph edge. \pre This object must be valid. Its validity is not checked.
	const Vec3f&          GetGraphVertexPosition() const; ///< Retrieves the position of the NavGraph vertex . \pre This object must be valid. Its validity is not checked.

public:
	NavGraphRawPtr m_navGraphRawPtr;
	CompactNavGraphVertexIdx m_vertexIdx;
};

KY_INLINE NavGraphVertexRawPtr::NavGraphVertexRawPtr() : m_vertexIdx(CompactNavGraphVertexIdx_MAXVAL) {}

KY_INLINE NavGraphVertexRawPtr::NavGraphVertexRawPtr(const NavGraphRawPtr& navGraphRawPtr, NavGraphVertexIdx vertexIdx) :
	m_navGraphRawPtr(navGraphRawPtr), m_vertexIdx((CompactNavGraphVertexIdx)vertexIdx) {}

KY_INLINE bool NavGraphVertexRawPtr::IsValid() const { return m_navGraphRawPtr.IsValid() && m_vertexIdx != CompactNavGraphVertexIdx_MAXVAL; }
KY_INLINE void NavGraphVertexRawPtr::Invalidate() { m_navGraphRawPtr.Invalidate(); m_vertexIdx = CompactNavGraphVertexIdx_MAXVAL; }

KY_INLINE NavGraphVertexIdx NavGraphVertexRawPtr::GetNavGraphVertexIdx() const { return (NavGraphVertexIdx)m_vertexIdx; }

KY_INLINE bool NavGraphVertexRawPtr::operator ==(const NavGraphVertexRawPtr& rhs) const { return m_navGraphRawPtr == rhs.m_navGraphRawPtr && m_vertexIdx == rhs.m_vertexIdx; }
KY_INLINE bool NavGraphVertexRawPtr::operator !=(const NavGraphVertexRawPtr& rhs) const { return !(*this == rhs); }
KY_INLINE bool NavGraphVertexRawPtr::operator < (const NavGraphVertexRawPtr& rhs) const { return m_navGraphRawPtr < rhs.m_navGraphRawPtr || (m_navGraphRawPtr == rhs.m_navGraphRawPtr && m_vertexIdx < rhs.m_vertexIdx); }
KY_INLINE bool NavGraphVertexRawPtr::operator <=(const NavGraphVertexRawPtr& rhs) const { return !(rhs < *this); }

KY_INLINE const NavGraphBlob* NavGraphVertexRawPtr::GetNavGraphBlob() const { return m_navGraphRawPtr.GetNavGraphBlob(); }
KY_INLINE NavGraph*           NavGraphVertexRawPtr::GetNavGraph()     const { return m_navGraphRawPtr.GetNavGraph();     }

KY_INLINE const NavGraphVertex& NavGraphVertexRawPtr::GetNavGraphVertex()      const { return GetNavGraphBlob()->GetNavGraphVertex(m_vertexIdx); }
KY_INLINE const Vec3f&          NavGraphVertexRawPtr::GetGraphVertexPosition() const { return GetNavGraphVertex().GetPosition();                 }
KY_INLINE GraphVertexData&      NavGraphVertexRawPtr::GetGraphVertexData()     const { return GetNavGraph()->GetGraphVertexData(m_vertexIdx);    }
KY_INLINE KyUInt32              NavGraphVertexRawPtr::GetNeighborVertexCount() const { return GetNavGraphVertex().GetNeighborVertexCount();      }

KY_INLINE const NavTag& NavGraphVertexRawPtr::GetNavTag() const
{
	const NavGraphBlob* navGraphBlob = GetNavGraphBlob();
	const NavTagIdx navTagIdx =  navGraphBlob->GetNavGraphVertexNavTagIdx(m_vertexIdx);
	return navGraphBlob->GetNavTag(navTagIdx);
}

}

#endif //Navigation_NavGraphVertexRawPtr_H

