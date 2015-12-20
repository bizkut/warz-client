/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NOBODY


namespace Kaim
{

KY_INLINE void Path::AddRef() { ++RefCount; }
KY_INLINE void Path::Release()
{
	if (RefCount-- == 1)
	{
		ClearAllBeforeDestruction();
		KY_FREE(this);
		return;
	}
}

KY_INLINE void     Path::SetNavigationProfileId(KyUInt32 navigationProfileId) { m_navigationProfileId = navigationProfileId; }
KY_INLINE KyUInt32 Path::GetNavigationProfileId() const { return m_navigationProfileId; }

KY_INLINE void Path::SetNodePosition         (KyUInt32 nodeIdx, const Vec3f& position                     ) { KY_ASSERT(nodeIdx < m_nodesCount); m_nodePositions[nodeIdx]        = position;          }
KY_INLINE void Path::SetNodeIntegerPosition  (KyUInt32 nodeIdx, const WorldIntegerPos& integerPos         ) { KY_ASSERT(nodeIdx < m_nodesCount); m_nodeIntegerPositions[nodeIdx] = integerPos;        }
KY_INLINE void Path::SetNodeNavTrianglePtr   (KyUInt32 nodeIdx, const NavTrianglePtr& navTrianglePtr      ) { KY_ASSERT(nodeIdx < m_nodesCount); m_nodeTrianglePtrs[nodeIdx]     = navTrianglePtr;    }
KY_INLINE void Path::SetNodeNavGraphVertexPtr(KyUInt32 nodeIdx, const NavGraphVertexPtr& navGraphVertexPtr) { KY_ASSERT(nodeIdx < m_nodesCount); m_nodeGraphVertexPtrs[nodeIdx]  = navGraphVertexPtr; }
KY_INLINE void Path::SetNodePosition3fAndInteger(KyUInt32 nodeIdx, const Vec3f& position, const WorldIntegerPos& integerPos)
{
	SetNodePosition(nodeIdx, position); SetNodeIntegerPosition(nodeIdx, integerPos);
}

KY_INLINE void Path::SetEdgeNavGraphEdgePtr(KyUInt32 pathEdgeIdx, const NavGraphEdgePtr& navGraphEdgePtr) { m_edgeNavGraphEdgePtr[pathEdgeIdx] = navGraphEdgePtr;       }
KY_INLINE void Path::SetPathEdgeType       (KyUInt32 pathEdgeIdx, PathEdgeType pathEdgeType             ) { KY_ASSERT(pathEdgeType < PathEdgeType_MaxCount); m_edgeTypes[pathEdgeIdx] = (KyUInt8)pathEdgeType; }
KY_INLINE void Path::SetPathCostAndDistance(KyFloat32 pathCost, KyFloat32 pathDistance) { m_pathCost = pathCost; m_pathDistance = pathDistance; }
KY_INLINE void Path::SetPathCellBox(const CellBox& cellBox) { m_pathCellBox = cellBox; }

KY_INLINE KyUInt32                 Path::GetByteSize()     const { return m_byteSize;     }
KY_INLINE KyUInt32                 Path::GetNodeCount()    const { return m_nodesCount;   }
KY_INLINE KyUInt32                 Path::GetEdgeCount()    const { return m_edgesCount;   }
KY_INLINE KyFloat32                Path::GetPathDistance() const { return m_pathDistance; }
KY_INLINE KyFloat32                Path::GetPathCost()     const { return m_pathCost;     }
KY_INLINE const CellBox&           Path::GetPathCellBox()  const { return m_pathCellBox;  }
KY_INLINE CellBox&                 Path::GetPathCellBox()        { return m_pathCellBox;  }

KY_INLINE const Vec3f&             Path::GetNodePosition(KyUInt32 nodeIdx)            const { KY_ASSERT(nodeIdx < m_nodesCount);     return m_nodePositions[nodeIdx];           }
KY_INLINE Vec3f&                   Path::GetNodePosition(KyUInt32 nodeIdx)                  { KY_ASSERT(nodeIdx < m_nodesCount);     return m_nodePositions[nodeIdx];           }
KY_INLINE const WorldIntegerPos&   Path::GetNodeIntegerPosition(KyUInt32 nodeIdx)     const { KY_ASSERT(nodeIdx < m_nodesCount);     return m_nodeIntegerPositions[nodeIdx];    }
KY_INLINE WorldIntegerPos&         Path::GetNodeIntegerPosition(KyUInt32 nodeIdx)           { KY_ASSERT(nodeIdx < m_nodesCount);     return m_nodeIntegerPositions[nodeIdx];    }
KY_INLINE const NavTrianglePtr&    Path::GetNodeNavTrianglePtr(KyUInt32 nodeIdx)      const { KY_ASSERT(nodeIdx < m_nodesCount);     return m_nodeTrianglePtrs[nodeIdx];        }
KY_INLINE NavTrianglePtr&          Path::GetNodeNavTrianglePtr(KyUInt32 nodeIdx)            { KY_ASSERT(nodeIdx < m_nodesCount);     return m_nodeTrianglePtrs[nodeIdx];        }
KY_INLINE const NavGraphVertexPtr& Path::GetNodeNavGraphVertexPtr(KyUInt32 nodeIdx)   const { KY_ASSERT(nodeIdx < m_nodesCount);     return m_nodeGraphVertexPtrs[nodeIdx];     }
KY_INLINE NavGraphVertexPtr&       Path::GetNodeNavGraphVertexPtr(KyUInt32 nodeIdx)         { KY_ASSERT(nodeIdx < m_nodesCount);     return m_nodeGraphVertexPtrs[nodeIdx];     }
KY_INLINE const NavGraphEdgePtr&   Path::GetEdgeNavGraphEdgePtr(KyUInt32 pathEdgeIdx) const { KY_ASSERT(pathEdgeIdx < m_edgesCount); return m_edgeNavGraphEdgePtr[pathEdgeIdx]; }
KY_INLINE NavGraphEdgePtr&         Path::GetEdgeNavGraphEdgePtr(KyUInt32 pathEdgeIdx)       { KY_ASSERT(pathEdgeIdx < m_edgesCount); return m_edgeNavGraphEdgePtr[pathEdgeIdx]; }

KY_INLINE PathEdgeType             Path::GetPathEdgeType(KyUInt32 pathEdgeIdx)            const { return (PathEdgeType)m_edgeTypes[pathEdgeIdx]; }
KY_INLINE const Vec3f&             Path::GetPathEdgeStartPosition(KyUInt32 edgeIdx)       const { return GetNodePosition(edgeIdx);               }
KY_INLINE const Vec3f&             Path::GetPathEdgeEndPosition(KyUInt32 edgeIdx)         const { return GetNodePosition(edgeIdx + 1);           }
KY_INLINE const NavTrianglePtr&    Path::GetPathEdgeStartNavTrianglePtr(KyUInt32 edgeIdx) const { return GetNodeNavTrianglePtr(edgeIdx);         }
KY_INLINE const NavTrianglePtr&    Path::GetPathEdgeEndNavTrianglePtr(KyUInt32 edgeIdx)   const { return GetNodeNavTrianglePtr(edgeIdx + 1);     }
KY_INLINE const Vec3f&             Path::GetPathStartPosition()                           const { return GetNodePosition(0);                     }
KY_INLINE const Vec3f&             Path::GetPathEndPosition()                             const { return GetNodePosition(GetNodeCount() - 1);    }
KY_INLINE const Vec3f*             Path::GetNodePositionBuffer()                          const { return m_nodePositions;                        }
KY_INLINE const WorldIntegerPos*   Path::GetNodeIntegerPositionBuffer()                   const { return m_nodeIntegerPositions;                 }

template <class T>
KY_INLINE void Path::ClearBufferCPP(T*& buffer, KyUInt32 count)
{
	ConstructorCPP<T>::DestructArray(buffer, count);
	buffer = KY_NULL;
}

template <class T>
KY_INLINE void Path::ClearBufferPOD(T*& buffer, KyUInt32 count)
{
	ConstructorPOD<T>::DestructArray(buffer, count);
	buffer = KY_NULL;

}

template <class T>
KY_INLINE void Path::InitBufferCPP(T*& buffer, KyUInt32 count, char*& memory)
{
	if (count != 0)
	{
		buffer = (T*)memory;
		memory += count * sizeof(T);
		ConstructorCPP<T>::ConstructArray(buffer, count);
	}
	else
	{
		buffer = KY_NULL;
	}
}

template <class T>
KY_INLINE void Path::InitBufferPOD(T*& buffer, KyUInt32 count, char*& memory)
{
	if (count != 0)
	{
		buffer = (T*)memory;
		memory += count * sizeof(T);
		ConstructorPOD<T>::ConstructArray(buffer, count);
	}
	else
	{
		buffer = KY_NULL;
	}
}

}
