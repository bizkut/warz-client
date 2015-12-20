/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphGenerator_H
#define Navigation_AbstractGraphGenerator_H

#include "gwnavruntime/abstractgraph/blobs/abstractgraphfloorgenerator.h"
#include "gwnavruntime/abstractgraph/blobs/abstractgraphblob.h"
#include "gwnavruntime/navmesh/traverselogic.h"
#include "gwnavruntime/queries/utils/cellfilter.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_RefCount.h"



namespace Kaim
{

class AbstractGraphGenerationReport;
class AbstractGraphDataGenerator;
class NavData;
class NavMeshElementBlob;

// Used internally by AbstractGraphDataGenerator
class AbstractGraphGenerator
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
public:
	AbstractGraphGenerator()
		: m_report(KY_NULL)
		, m_navData(KY_NULL)
		, m_navMeshElementBlob(KY_NULL)
		, m_neighborhoodDataCount(0)
		, m_graphCellCount(0)
	{}

	KyResult Init(AbstractGraphDataGenerator* abstractGraphGenerator);
	KyResult Generate(Ptr< BlobHandler<AbstractGraphBlob> >& abstractGraphBlobHandler, Ptr<CellFilter> cellFilter, KyArrayPOD<CellPos>& cellPosBorder, KyArrayPOD<KyUInt8>& wantedBoundariesPerCells);

	void ClearAndRelease()
	{
		m_graphNodeTriangles.ClearAndRelease();
		m_graphNodeIdxToGraphCellFloorIndices.ClearAndRelease();
		m_graphNodeNeighborCosts.ClearAndRelease();
		m_graphNodeNeighbors.ClearAndRelease();
		m_tmpGraphFloorBlobHandlers.ClearAndRelease();
		m_tmpGraphCellBlobHandlers.ClearAndRelease();
		m_graphCellCount = 0;
		m_neighborhoodDataCount = 0;
		m_tmpGraphFloorGenerator.ClearAndRelease();
	}

private:
	KyResult CreateNodes(KyArrayPOD<CellPos>& cellPosBorder, KyArrayPOD<KyUInt8>& wantedBoundariesPerCells);

	void ComputeAllCosts();
	Vec3f GetNodePosition(AbstractGraphNodeIdx nodeIdx);
	
public:
	AbstractGraphGenerationReport* m_report;

	NavData* m_navData;
	NavMeshElementBlob* m_navMeshElementBlob;
	
	KyArrayPOD<NavTriangleRawPtr> m_graphNodeTriangles; // used to store the triangles on which an abstract graph node lies on
	KyArrayPOD<AbstractGraphCellFloorIndices> m_graphNodeIdxToGraphCellFloorIndices; // associates an abstract graph node to the indices of its abstract graph cell and floor
	KyUInt32 m_neighborhoodDataCount;
	KyArray< KyArrayPOD<KyFloat32, MemStat_NavDataGen>, MemStat_NavDataGen > m_graphNodeNeighborCosts;
	KyArray< KyArrayPOD<CompactAbstractGraphNodeIdx, MemStat_NavDataGen>, MemStat_NavDataGen > m_graphNodeNeighbors;
	
	AbstractGraphFloorGenerator m_tmpGraphFloorGenerator;
	KyArray< Ptr< BlobHandler<AbstractGraphFloorBlob> >, MemStat_NavDataGen > m_tmpGraphFloorBlobHandlers;
	KyUInt32 m_graphCellCount;
	KyArray< Ptr< BlobHandler<AbstractGraphCellBlob> >, MemStat_NavDataGen > m_tmpGraphCellBlobHandlers;
	Ptr<CellFilter> m_cellFilter;
};


}

#endif
