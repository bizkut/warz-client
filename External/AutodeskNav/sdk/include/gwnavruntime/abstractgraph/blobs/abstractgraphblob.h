/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphBlob_H
#define Navigation_AbstractGraphBlob_H

#include "gwnavruntime/navmesh/blobs/navmeshelementblob.h"
#include "gwnavruntime/containers/bitfieldblob.h"
#include "gwnavruntime/abstractgraph/blobs/abstractgraphcellblob.h"

namespace Kaim
{


class AbstractGraphCellFloorIndices
{
public:
	AbstractGraphCellFloorIndices() {}
	AbstractGraphCellFloorIndices(CompactAbstractGraphCellIdx graphCellIdx, CompactAbstractGraphFloorIdx graphFloorIdx)
		: m_abstractCellIdx(graphCellIdx),  m_abstractFloorIdx(graphFloorIdx)	{}

	CompactAbstractGraphCellIdx m_abstractCellIdx;
	CompactAbstractGraphFloorIdx m_abstractFloorIdx;
};

inline void SwapEndianness(Endianness::Target e, AbstractGraphCellFloorIndices& self)
{
	SwapEndianness(e, self.m_abstractCellIdx);
	SwapEndianness(e, self.m_abstractFloorIdx);
}


class AbstractGraphBlob
{
	KY_ROOT_BLOB_CLASS(NavData, AbstractGraphBlob, 0)
public:

	bool IsCompatibleWith(const NavMeshGenParameters& genParameters) const;

	KyUInt32 GetNodeCount() const;

	Vec3f GetNodePosition(AbstractGraphNodeIdx graphNodeIdx, const DatabaseGenMetrics& genMetrics) const;

	KyFloat32 GetNeighborCost(AbstractGraphNodeIdx fromGraphNodeIdx, AbstractGraphNodeIdx toGraphNodeIdx) const;

	bool IsCellCovered(AbstractGraphNodeIdx graphNodeIdx, const DatabaseGenMetrics& genMetrics) const;

public: // internal
	void GetVertexAndAltitude(AbstractGraphNodeIdx graphNodeIdx, NavVertex& graphNodeVertex, KyFloat32& altitude) const;

public:
	NavMeshGenParameters m_navMeshGenParameters;
	GuidCompound m_navMeshGuidCompound;
	CellBox m_cellBox;
	BitFieldBlob m_cellBoxCoverage;
	BlobArray<AbstractGraphCellFloorIndices> m_graphNodeIdxToGraphCellFloorIndices;

	BlobArray<KyFloat32> m_neighborCostsHalfMatrixBuffer;

	BlobArray< BlobRef<AbstractGraphCellBlob> > m_abstractCells;
};

inline void SwapEndianness(Endianness::Target e, AbstractGraphBlob& self)
{
	SwapEndianness(e, self.m_navMeshGenParameters);
	SwapEndianness(e, self.m_navMeshGuidCompound);
	SwapEndianness(e, self.m_cellBox);
	SwapEndianness(e, self.m_graphNodeIdxToGraphCellFloorIndices);
	SwapEndianness(e, self.m_neighborCostsHalfMatrixBuffer);
	SwapEndianness(e, self.m_abstractCells);
}



KY_INLINE bool AbstractGraphBlob::IsCompatibleWith(const NavMeshGenParameters& genParameters) const
{
	return m_navMeshGenParameters == genParameters;
}

KY_INLINE KyUInt32 AbstractGraphBlob::GetNodeCount() const
{
	return m_graphNodeIdxToGraphCellFloorIndices.GetCount();
}

KY_INLINE Vec3f AbstractGraphBlob::GetNodePosition(AbstractGraphNodeIdx graphNodeIdx, const DatabaseGenMetrics& genMetrics) const
{
	const AbstractGraphCellFloorIndices& cellFloorIndices = m_graphNodeIdxToGraphCellFloorIndices.GetValues()[graphNodeIdx];
	const AbstractGraphCellBlob* graphCellBlob = (m_abstractCells.GetValues()[cellFloorIndices.m_abstractCellIdx]).Ptr();
	return graphCellBlob->GetPosition(graphNodeIdx, cellFloorIndices.m_abstractFloorIdx, genMetrics);
}


}


#endif
