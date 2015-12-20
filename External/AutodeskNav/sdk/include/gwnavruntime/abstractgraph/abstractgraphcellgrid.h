/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphCellGrid_H
#define Navigation_AbstractGraphCellGrid_H

#include "gwnavruntime/abstractgraph/abstractgraph.h"

#include "gwnavruntime/abstractgraph/identifiers/loadedabstractgraphnodeidx.h"
#include "gwnavruntime/abstractgraph/identifiers/loadedabstractgraphcellidx.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/base/memory.h"



namespace Kaim
{

class Database;
class AbstractGraph;
class AbstractGraphBlob;
class NavMeshGenParameters;
class ScopedDisplayList;


class AbstractGraphCell
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	LoadedAbstractGraphCellIdx m_graphCellIdx;
	// KY_TODO maintain association AbstractGraphCell with NavCellPosInfo ?
};


class AbstractGraphCellGrid
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)

public:
	AbstractGraphCellGrid(Database* database)
		: m_database(database)
		, m_abstractGraphCellBuffer(KY_NULL)
		, m_abstractGraphCellBufferSize(0)
		, m_altitudeTolerance(0.5f)
		, m_abstractGraphChangeIdx(0)
	{}

	~AbstractGraphCellGrid() { Clear(); }

	void Clear();

	void EnlargeGrid(const CellBox& cellBox);

	AbstractGraph* InsertAbstractGraph(AbstractGraphBlob* abstractGraphBlob);
	void RemoveAbstractGraph(AbstractGraph* abstractGraph);

	void UpdateNavMeshLinkStatus(CellBox cellBox);
	
private:
	KyResult CheckGenerationParameters(const NavMeshGenParameters& navMeshGenParameters);

public: //internal
	LoadedAbstractGraphCellIdx GetAbstractGraphCellIdx(const CellPos& cellPos) const;
	AbstractGraph* GetAbstractGraph(const LoadedAbstractGraphNodeIdx& node) const;
	AbstractGraph* GetAbstractGraph(const LoadedAbstractGraphCellIdx& cell) const;
	AbstractGraph* GetAbstractGraph(const CellPos& cellPos) const;

	// used only in unittests while debugging, not needed otherwise, the VisualDebug uses Blobs instead of ScopedDisplayLists
	void Display(ScopedDisplayList& displayList);

public:
	Database* m_database;
	AbstractGraphCell* m_abstractGraphCellBuffer;
	KyInt32 m_abstractGraphCellBufferSize;
	CellBox m_cellBox;
	KyFloat32 m_altitudeTolerance;
	TrackedCollection< Ptr<AbstractGraph>, MemStat_NavData> m_abstractGraphs;
	KyUInt32 m_abstractGraphChangeIdx;
};



}

#endif
