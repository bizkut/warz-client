/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphFloorBlob_H
#define Navigation_AbstractGraphFloorBlob_H


#include "gwnavruntime/abstractgraph/abstractgraphtypes.h"
#include "gwnavruntime/navdata/navdatablobcategory.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/base/endianness.h"


namespace Kaim
{

class AbstractGraphFloorBlob
{
	KY_ROOT_BLOB_CLASS(NavData, AbstractGraphFloorBlob, 0)
public:
	AbstractGraphFloorBlob();

public:
	KyUInt32 GetLocalNodeIdx(AbstractGraphNodeIdx graphNodeIdx) const;
	KyUInt32 GetNodeIdx(KyUInt32 localNodeIdx) const;

	const AbstractGraphVertex& GetAbstractGraphVertex(AbstractGraphNodeIdx graphNodeIdx) const;	
	KyFloat32 GetAltitude(AbstractGraphNodeIdx graphNodeIdx) const;
	CardinalDir GetCellBoundaryDir(AbstractGraphNodeIdx graphNodeIdx) const;
		
	const AbstractGraphVertex& GetAbstractGraphVertexFromLocalIdx(KyUInt32 localIdx) const;	
	KyFloat32 GetAltitudeFromLocalIdx(KyUInt32 localIdx) const;
	CardinalDir GetCellBoundaryDirFromLocalIdx(KyUInt32 localNodeIdx) const;


public:
	NavFloorIdx m_originalNavFloorIdx;

	AbstractGraphNodeIdx m_abstractGraphNodeFirstIdx;
	KyUInt16 m_firstIdxForCellBoundaryDir[4];
	KyUInt16 m_countForCellBoundaryDir[4];
	BlobArray<KyFloat32> m_altitudes;
	BlobArray<AbstractGraphVertex> m_nodeVertices; // Note that nodes are sorted along the Y axis for EAST and WEST cell boundaries, or the X axis for NORTH and SOUTH cell boundaries
};

inline void SwapEndianness(Endianness::Target e, AbstractGraphFloorBlob& self)
{
	SwapEndianness(e, self.m_originalNavFloorIdx);
	SwapEndianness(e, self.m_abstractGraphNodeFirstIdx);
	SwapEndianness(e, self.m_firstIdxForCellBoundaryDir[0]);
	SwapEndianness(e, self.m_firstIdxForCellBoundaryDir[1]);
	SwapEndianness(e, self.m_firstIdxForCellBoundaryDir[2]);
	SwapEndianness(e, self.m_firstIdxForCellBoundaryDir[3]);
	SwapEndianness(e, self.m_countForCellBoundaryDir[0]);
	SwapEndianness(e, self.m_countForCellBoundaryDir[1]);
	SwapEndianness(e, self.m_countForCellBoundaryDir[2]);
	SwapEndianness(e, self.m_countForCellBoundaryDir[3]);
	SwapEndianness(e, self.m_altitudes);
	SwapEndianness(e, self.m_nodeVertices);
}


KY_INLINE KyUInt32 AbstractGraphFloorBlob::GetNodeIdx(KyUInt32 localNodeIdx) const
{
	return localNodeIdx + m_abstractGraphNodeFirstIdx;
}

KY_INLINE KyUInt32 AbstractGraphFloorBlob::GetLocalNodeIdx(AbstractGraphNodeIdx graphNodeIdx) const
{
	KY_ASSERT(graphNodeIdx >= m_abstractGraphNodeFirstIdx);
	KY_ASSERT(graphNodeIdx < (m_abstractGraphNodeFirstIdx + m_countForCellBoundaryDir[0] + m_countForCellBoundaryDir[1] + m_countForCellBoundaryDir[2] + m_countForCellBoundaryDir[3]));
	return graphNodeIdx - m_abstractGraphNodeFirstIdx;
}

KY_INLINE const AbstractGraphVertex& AbstractGraphFloorBlob::GetAbstractGraphVertex(AbstractGraphNodeIdx graphNodeIdx) const
{
	return GetAbstractGraphVertexFromLocalIdx(GetLocalNodeIdx(graphNodeIdx));
}

KY_INLINE KyFloat32 AbstractGraphFloorBlob::GetAltitude(AbstractGraphNodeIdx graphNodeIdx) const
{
	return GetAltitudeFromLocalIdx(GetLocalNodeIdx(graphNodeIdx));
}

KY_INLINE const AbstractGraphVertex& AbstractGraphFloorBlob::GetAbstractGraphVertexFromLocalIdx(KyUInt32 localIdx) const
{
	return m_nodeVertices.GetValues()[localIdx];
}

KY_INLINE KyFloat32 AbstractGraphFloorBlob::GetAltitudeFromLocalIdx(KyUInt32 localIdx) const
{
	return m_altitudes.GetValues()[localIdx];
}

KY_INLINE CardinalDir AbstractGraphFloorBlob::GetCellBoundaryDir(AbstractGraphNodeIdx graphNodeIdx) const
{
	return GetCellBoundaryDirFromLocalIdx(GetLocalNodeIdx(graphNodeIdx));
}

}


#endif
