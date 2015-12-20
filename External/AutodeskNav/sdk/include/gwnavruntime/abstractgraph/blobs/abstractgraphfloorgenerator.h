/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphFloorGenerator_H
#define Navigation_AbstractGraphFloorGenerator_H

#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/abstractgraph/abstractgraphtypes.h"


namespace Kaim
{

class NavCellBlob;
class AbstractGraphGenerator;
class AbstractGraphCellFloorIndices;

class AbstractGraphFloorGenerator
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
public:
	AbstractGraphFloorGenerator();

	void Generate(AbstractGraphGenerator* generator, const NavCellBlob* navCellBlob, KyUInt32 navFloorIdx, KyUInt32 wantedBoundariesType, 
		AbstractGraphCellFloorIndices& graphCellFloorIndices);

	void ClearAndRelease();

public:
	void GenerateOnCellBoundary(CardinalDir cellBoundaryDir);

	void CreateNodes(CardinalDir cellBoundaryDir);

public:
	AbstractGraphGenerator* m_generator;
	const NavCellBlob* m_navCellBlob;
	KyUInt32 m_originalNavFloorIdx;
	KyUInt32 m_wantedCellBoundaries;

	CompactAbstractGraphNodeIdx m_abstractGraphNodeFirstIdx;
	KyUInt16 m_firstIdxForCellBoundaryDir[4];
	KyUInt16 m_countForCellBoundaryDir[4];
	KyArrayPOD<KyFloat32, MemStat_NavDataGen> m_altitudes;
	KyArrayPOD<AbstractGraphVertex, MemStat_NavDataGen> m_nodeVertices; // Note that nodes are sorted along the Y axis for EAST and WEST cell boundaries, or the X axis for NORTH and SOUTH cell boundaries
};

}

#endif
