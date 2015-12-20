/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavGraphEdgePtr_H
#define Navigation_NavGraphEdgePtr_H

#include "gwnavruntime/navgraph/identifiers/navgraphvertexptr.h"
#include "gwnavruntime/navgraph/identifiers/navgraphedgerawptr.h"

namespace Kaim
{

/// Each instance of this class uniquely identifies a single and mono-directionnal NavGraphEdge in a NavGraph.
/// Instances of this class can be safely stored and used in subsequent frames.
/// Within a NavGraph, a NavGraphEdge is identified by a NavGraphVertexIdx (corresponding to its starting vertex) and the
///  index of a Neighbor vertex of this starting vertex among all its neighbor vertices (This neighbor vertex corresponds 
///  to its starting vertex).
class NavGraphEdgePtr
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	NavGraphEdgePtr(); ///< Constructs a new instance of this class. Creates an invalid NavGraphEdgePtr. 
	NavGraphEdgePtr(const NavGraphPtr& navGraphPtr, const NavGraphEdgeSmartIdx& edgeSmartIdx);
	NavGraphEdgePtr(const NavGraphVertexPtr& navGraphVertexPtr, KyUInt32 neighborIndex);

	/// Returns true if this object refers to a valid NavGraphEdge: i.e. a NavGraphEdge in a valid NavGraph. see NavGraph::IsValid().
	bool IsValid() const;

	void Invalidate(); ///< Invalidates this object. 

	bool operator ==(const NavGraphEdgePtr& rhs) const; ///< Returns true if this object identifies the same edge as rhs, or if both are invalid. 
	bool operator !=(const NavGraphEdgePtr& rhs) const; ///< Returns true if this object identifies an edge different from the one identified by rhs, or if only one of them is invalid.
	bool operator < (const NavGraphEdgePtr& rhs) const;
	bool operator <=(const NavGraphEdgePtr& rhs) const;

	/// Returns a pointer to the NavGraph that handles the graph identified by this object.
	/// If this object is invalid, returns KY_NULL. 
	NavGraph* GetNavGraph() const;

	/// Returns a pointer to the NavGraphBlob identified by this object.
	/// If this object is invalid, returns KY_NULL. 
	const NavGraphBlob* GetNavGraphBlob() const;

	/// Returns the index of the edge around its starting vertex. It is the index of the ending vertex within the neighbor vertices of the starting vertex.
	KyUInt32 GetEdgeNumberAroundStartVertex() const;

	/// Returns the index of the starting NavGraphVertex of current instance within its NavGraph.
	NavGraphVertexIdx GetStartVertexIdx() const;

	// ---------------------------------- Member Functions for valid instance ----------------------------------
	/// All these function should be called only if IsValid() returns true

	/// Returns a pointer to the NavGraph that handles the graph identified by this object.
	/// \pre    This object must be valid. Its validity is not checked. 
	NavGraph* GetNavGraph_UnSafe() const;

	/// Returns a pointer to the NavGraphBlob identified by this object.
	/// \pre    This object must be valid. Its validity is not checked. 
	const NavGraphBlob* GetNavGraphBlob_UnSafe() const;

	/// Returns the index of the ending NavGraphVertex of current instance within its NavGraph.
	/// \pre    This object must be valid. Its validity is not checked. 
	NavGraphVertexIdx GetEndNavGraphVertexIdx() const;

	/// Returns a NavGraphVertexPtr that identifies the starting NavGraphVertex of current instance.
	/// \pre    This object must be valid. Its validity is not checked. 
	NavGraphVertexPtr GetStartNavGraphVertexPtr() const;

	/// Returns a NavGraphVertexPtr that identifies the ending NavGraphVertex of current instance.
	/// \pre    This object must be valid. Its validity is not checked. 
	NavGraphVertexPtr GetEndNavGraphVertexPtr() const;

	/// Returns the starting NavGraphVertex of current instance.
	/// \pre    This object must be valid. Its validity is not checked. 
	const NavGraphVertex& GetStartNavGraphVertex() const;

	/// Returns the ending NavGraphVertex of current instance.
	/// \pre    This object must be valid. Its validity is not checked. 
	const NavGraphVertex& GetEndNavGraphVertex() const;

	/// Returns the position of the starting NavGraphVertex of current instance.
	/// \pre    This object must be valid. Its validity is not checked. 
	const Vec3f& GetStartNavGraphVertexPosition() const;

	/// Returns the position of the ending NavGraphVertex of current instance.
	/// \pre    This object must be valid. Its validity is not checked. 
	const Vec3f& GetEndNavGraphVertexPosition() const;

	/// Returns the NavTag associated to this NavGraphEdge
	/// \pre    This object must be valid. Its validity is not checked. 
	const NavTag& GetNavTag() const;

	/// Constructs and returns a NavGraphEdgeRawPtr that refers to the same NavGraphVertex
	/// \pre    This object must be valid. Its validity is not checked. 
	NavGraphEdgeRawPtr GetRawPtr() const;

	/// Return the opposite NavGraphEdgePtr, if any exists.
	/// If no opposite NavGraphEdgePtr exists (the edge is not "bi-directional"), an invalid NavGraphEdgePtr is returned
	/// \pre    This object must be valid. Its validity is not checked. 
	NavGraphEdgePtr GetOppositeNavGraphEdgePtr() const;

public:
	NavGraphPtr m_navGraphPtr;
	NavGraphEdgeSmartIdx m_edgeSmartIdx;
};

KY_INLINE NavGraphEdgePtr::NavGraphEdgePtr() : m_navGraphPtr(), m_edgeSmartIdx() {}
KY_INLINE NavGraphEdgePtr::NavGraphEdgePtr(const NavGraphPtr& navGraphPtr, const NavGraphEdgeSmartIdx& edgeSmartIdx) :
	m_navGraphPtr(navGraphPtr), m_edgeSmartIdx(edgeSmartIdx) {}
KY_INLINE NavGraphEdgePtr::NavGraphEdgePtr(const NavGraphVertexPtr& navGraphVertexPtr, KyUInt32 neighborIndex) :
	m_navGraphPtr(navGraphVertexPtr.m_navGraphPtr), m_edgeSmartIdx(navGraphVertexPtr.GetNavGraphVertexIdx(), neighborIndex) {}

KY_INLINE bool NavGraphEdgePtr::IsValid() const { return m_navGraphPtr.IsValid() && m_edgeSmartIdx.IsValid(); }
KY_INLINE void NavGraphEdgePtr::Invalidate() { m_navGraphPtr.Invalidate(); m_edgeSmartIdx.Invalidate(); }

KY_INLINE NavGraphEdgeRawPtr    NavGraphEdgePtr::GetRawPtr()                      const { return NavGraphEdgeRawPtr(GetNavGraph(), m_edgeSmartIdx);             }
KY_INLINE NavGraph*             NavGraphEdgePtr::GetNavGraph()                    const { return m_navGraphPtr.GetNavGraph();                                   }
KY_INLINE const NavGraphBlob*   NavGraphEdgePtr::GetNavGraphBlob()                const { return m_navGraphPtr.GetNavGraphBlob();                               }
KY_INLINE NavGraph*             NavGraphEdgePtr::GetNavGraph_UnSafe()             const { return m_navGraphPtr.GetNavGraph_UnSafe();                            }
KY_INLINE const NavGraphBlob*   NavGraphEdgePtr::GetNavGraphBlob_UnSafe()         const { return m_navGraphPtr.GetNavGraphBlob_UnSafe();                        }
KY_INLINE NavGraphVertexIdx     NavGraphEdgePtr::GetStartVertexIdx()              const { return m_edgeSmartIdx.GetStartVertexIdx();                            }
KY_INLINE KyUInt32              NavGraphEdgePtr::GetEdgeNumberAroundStartVertex() const { return m_edgeSmartIdx.GetNeighborVertexIdx();                        }
KY_INLINE NavGraphVertexIdx     NavGraphEdgePtr::GetEndNavGraphVertexIdx()        const { return GetRawPtr().GetEndNavGraphVertexIdx();                         }
KY_INLINE const NavGraphVertex& NavGraphEdgePtr::GetStartNavGraphVertex()         const { return GetRawPtr().GetStartNavGraphVertex();                          }
KY_INLINE const NavGraphVertex& NavGraphEdgePtr::GetEndNavGraphVertex()           const { return GetRawPtr().GetEndNavGraphVertex();                            }
KY_INLINE const Vec3f&          NavGraphEdgePtr::GetStartNavGraphVertexPosition() const { return GetRawPtr().GetStartNavGraphVertexPosition();                  }
KY_INLINE const Vec3f&          NavGraphEdgePtr::GetEndNavGraphVertexPosition()   const { return GetRawPtr().GetEndNavGraphVertexPosition();                    }
KY_INLINE const NavTag&         NavGraphEdgePtr::GetNavTag()                      const { return GetRawPtr().GetNavTag();                                       }

KY_INLINE bool NavGraphEdgePtr::operator ==(const NavGraphEdgePtr& rhs) const { return m_navGraphPtr == rhs.m_navGraphPtr && m_edgeSmartIdx == rhs.m_edgeSmartIdx; }
KY_INLINE bool NavGraphEdgePtr::operator < (const NavGraphEdgePtr& rhs) const { return m_navGraphPtr < rhs.m_navGraphPtr || (m_navGraphPtr == rhs.m_navGraphPtr && m_edgeSmartIdx < rhs.m_edgeSmartIdx); }
KY_INLINE bool NavGraphEdgePtr::operator !=(const NavGraphEdgePtr& rhs) const { return !(*this == rhs); }
KY_INLINE bool NavGraphEdgePtr::operator <=(const NavGraphEdgePtr& rhs) const { return !(rhs < *this);  }
// avoid warning C471 : marked as __forceinline not inlined
inline NavGraphVertexPtr     NavGraphEdgePtr::GetStartNavGraphVertexPtr()      const { return NavGraphVertexPtr(GetRawPtr().GetStartNavGraphVertexRawPtr()); }
inline NavGraphVertexPtr     NavGraphEdgePtr::GetEndNavGraphVertexPtr()        const { return NavGraphVertexPtr(GetRawPtr().GetEndNavGraphVertexRawPtr());   }

}

#endif //Navigation_NavGraphEdgePtr_H

