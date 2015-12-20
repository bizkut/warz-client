/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavGraphVertexPtr_H
#define Navigation_NavGraphVertexPtr_H

#include "gwnavruntime/navgraph/identifiers/navgraphptr.h"
#include "gwnavruntime/navgraph/identifiers/navgraphvertexrawptr.h"


namespace Kaim
{

/// Each instance of this class uniquely identifies a single NavGraphVertex in a NavGraph.
/// Instances of this class can be safely stored and used in subsequent frames.
class NavGraphVertexPtr
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)

public:
	NavGraphVertexPtr(); ///< Constructs a new instance of this class. Creates an invalid NavGraphVertexPtr. 

	/// Construct a new instance of NavGraphVertexPtr, referring to the vertexIdx vertex of the NavGraph identified by navGraphPtr.
	NavGraphVertexPtr(const NavGraphPtr& navGraphPtr, NavGraphVertexIdx vertexIdx);

	/// Construct a new instance of NavGraphVertexPtr copying information from navGraphVertexRawPtr.
	explicit NavGraphVertexPtr(const NavGraphVertexRawPtr& navGraphVertexRawPtr);

	/// Returns true if this object refers to a valid NavGraphVertex: i.e. a NavGraphVertex in a validNavGraph. see NavGraph::IsValid().
	bool IsValid() const;

	void Invalidate(); ///< Invalidates this object. 

	bool operator ==(const NavGraphVertexPtr& rhs) const; ///< Returns true if this object identifies the same vertex as rhs, or if both are invalid.
	bool operator !=(const NavGraphVertexPtr& rhs) const; ///< Returns true if this object identifies an vertex different from the one identified by rhs, or if only one of them is invalid.
	bool operator < (const NavGraphVertexPtr& rhs) const;
	bool operator <=(const NavGraphVertexPtr& rhs) const;

	/// Returns the GraphVertexIdx that identifies this vertex within its Graph. 
	NavGraphVertexIdx GetNavGraphVertexIdx() const;

	/// Returns a pointer to the NavGraph that handles the graph identified by this object.
	/// If this object is invalid, returns KY_NULL. 
	NavGraph* GetNavGraph() const;

	/// Returns a pointer to the NavGraphBlob identified by this object.
	/// If this object is invalid, returns KY_NULL. 
	const NavGraphBlob* GetNavGraphBlob() const;

	// ---------------------------------- Member Functions for valid instance ----------------------------------
	/// All these function should be called only if IsValid() returns true
	/// \pre    This object must be valid. Its validity is not checked. 

	/// Returns a pointer to the NavGraph that handles the graph identified by this object.
	/// \pre    This object must be valid. Its validity is not checked. 
	NavGraph* GetNavGraph_UnSafe() const;

	/// Returns a pointer to the NavGraphBlob identified by this object.
	/// \pre    This object must be valid. Its validity is not checked. 
	const NavGraphBlob* GetNavGraphBlob_UnSafe() const;

	/// Returns the number of outgoing edges (equal to the number of neighbors) of this vertex.
	/// \pre    This object must be valid. Its validity is not checked. 
	KyUInt32 GetNeighborVertexCount() const;

	/// Returns the GraphVertexData associated to this NavGraphVertex. For Internal use.
	/// \pre    This object must be valid. Its validity is not checked. 
	GraphVertexData& GetGraphVertexData() const;

	/// Returns the NavTag associated to this NavGraphVertex.
	/// \pre    This object must be valid. Its validity is not checked. 
	const NavTag& GetNavTag() const;

	/// Retrieves the NavGraph vertex at the start of the specified NavGraph edge.
	/// \pre    This object must be valid. Its validity is not checked. 
	const NavGraphVertex& GetNavGraphVertex() const;

	/// Retrieves the position of the NavGraph vertex .
	/// \pre    This object must be valid. Its validity is not checked. 
	const Vec3f& GetGraphVertexPosition() const;

	/// Constructs and returns a NavGraphVertexRawPtr that refers to the same NavGraphVertex
	/// \pre    This object must be valid. Its validity is not checked. 
	NavGraphVertexRawPtr GetRawPtr() const;

public:
	NavGraphPtr m_navGraphPtr;
	NavGraphVertexIdx m_vertexIdx;
};


KY_INLINE NavGraphVertexPtr::NavGraphVertexPtr() :
	m_navGraphPtr(), m_vertexIdx(NavGraphVertexIdx_Invalid) {}
KY_INLINE NavGraphVertexPtr::NavGraphVertexPtr(const NavGraphPtr& navGraphPtr, NavGraphVertexIdx vertexIdx) :
	m_navGraphPtr(navGraphPtr), m_vertexIdx(vertexIdx) {}
KY_INLINE NavGraphVertexPtr::NavGraphVertexPtr(const NavGraphVertexRawPtr& navGraphVertexRawPtr) :
	m_navGraphPtr(navGraphVertexRawPtr.GetNavGraph()), m_vertexIdx(navGraphVertexRawPtr.GetNavGraphVertexIdx()) {}

KY_INLINE bool NavGraphVertexPtr::IsValid() const { return m_navGraphPtr.IsValid() && m_vertexIdx != NavGraphVertexIdx_Invalid; }
KY_INLINE void NavGraphVertexPtr::Invalidate() { m_navGraphPtr.Invalidate(); m_vertexIdx = NavGraphVertexIdx_Invalid; }

KY_INLINE bool NavGraphVertexPtr::operator ==(const NavGraphVertexPtr& rhs) const { return m_navGraphPtr == rhs.m_navGraphPtr && m_vertexIdx == rhs.m_vertexIdx; }
KY_INLINE bool NavGraphVertexPtr::operator !=(const NavGraphVertexPtr& rhs) const { return !(*this == rhs); }
KY_INLINE bool NavGraphVertexPtr::operator < (const NavGraphVertexPtr& rhs) const { return m_navGraphPtr < rhs.m_navGraphPtr || (m_navGraphPtr == rhs.m_navGraphPtr && m_vertexIdx < rhs.m_vertexIdx); }
KY_INLINE bool NavGraphVertexPtr::operator <=(const NavGraphVertexPtr& rhs) const { return !(rhs < *this); }

KY_INLINE NavGraphVertexIdx     NavGraphVertexPtr::GetNavGraphVertexIdx()   const { return m_vertexIdx;                                                 }
KY_INLINE NavGraph*             NavGraphVertexPtr::GetNavGraph()            const { return m_navGraphPtr.GetNavGraph();                                 }
KY_INLINE NavGraph*             NavGraphVertexPtr::GetNavGraph_UnSafe()     const { return m_navGraphPtr.GetNavGraph_UnSafe();                          }
KY_INLINE const NavGraphBlob*   NavGraphVertexPtr::GetNavGraphBlob()        const { return m_navGraphPtr.GetNavGraphBlob();                             }
KY_INLINE const NavGraphBlob*   NavGraphVertexPtr::GetNavGraphBlob_UnSafe() const { return m_navGraphPtr.GetNavGraphBlob_UnSafe();                      }
KY_INLINE NavGraphVertexRawPtr  NavGraphVertexPtr::GetRawPtr()              const { return NavGraphVertexRawPtr(GetNavGraph(), GetNavGraphVertexIdx()); }
KY_INLINE KyUInt32              NavGraphVertexPtr::GetNeighborVertexCount() const { return GetRawPtr().GetNeighborVertexCount();                        }
KY_INLINE GraphVertexData&      NavGraphVertexPtr::GetGraphVertexData()     const { return GetRawPtr().GetGraphVertexData();                            }
KY_INLINE const NavTag&         NavGraphVertexPtr::GetNavTag()              const { return GetRawPtr().GetNavTag();                                     }
KY_INLINE const NavGraphVertex& NavGraphVertexPtr::GetNavGraphVertex()      const { return GetRawPtr().GetNavGraphVertex();                             }
KY_INLINE const Vec3f&          NavGraphVertexPtr::GetGraphVertexPosition() const { return GetRawPtr().GetGraphVertexPosition();                        }

}

#endif //Navigation_NavGraphVertexPtr_H

