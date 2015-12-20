/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraph_H
#define Navigation_AbstractGraph_H

#include "gwnavruntime/abstractgraph/blobs/abstractgraphblob.h"
#include "gwnavruntime/abstractgraph/identifiers/loadedabstractgraphnodeidx.h"
#include "gwnavruntime/abstractgraph/identifiers/loadedabstractgraphcellidx.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/containers/collection.h"
#include "gwnavruntime/kernel/SF_RefCount.h"




namespace Kaim
{

class AbstractGraphCellGrid;
class AbstractGraphBlob;
class AbstractGraph;
class Database;
class ScopedDisplayList;
class VisualColor;
class MemoryHeap;


class AbstractGraphNodeLink
{
public:
	enum NavMeshLinkStatus
	{
		NavMeshLink_NoNavMesh = 0,   // No Navmesh is loaded underneath, i.e. no ActiveCell at the CellPos of the node
		NavMeshLink_Inside    = 1,   // A NavMesh is loaded, but the Node is on the NavMesh		

		NavMeshLink_Undefined = KyUInt32MAXVAL,   // Link Status is undefined yet
	};

public:
	AbstractGraphNodeLink()
		: m_navMeshLinkStatus(NavMeshLink_Undefined)
	{}

	bool CanTraverse() const { return m_pairedNodeIdx.IsValid() && (m_navMeshLinkStatus == NavMeshLink_NoNavMesh || m_navMeshLinkStatus == NavMeshLink_Inside); }

public:
	LoadedAbstractGraphNodeIdx m_pairedNodeIdx; //< gives the node in a neighbor graph, this node is paired to
	NavMeshLinkStatus m_navMeshLinkStatus;
};


class AbstractGraph
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
	KY_CLASS_WITHOUT_COPY(AbstractGraph)
	KY_REFCOUNT_MALLOC_FREE(AbstractGraph)

public:
	static Ptr<AbstractGraph> Create(AbstractGraphBlob* abstractGraphBlob, Database* database, MemoryHeap* pheap = KY_NULL);

	KyUInt32 GetNodeCount() const { return m_blob->GetNodeCount(); }

	Vec3f GetNodePosition(const LoadedAbstractGraphNodeIdx& node) const;  ///<  It must be a node of this AbstractGraph
	Vec3f GetNodePosition(AbstractGraphNodeIdx nodeIdx) const;

	// -------------------- Access to Neighbors Nodes ------------------

	KyFloat32 GetNeighborCost(AbstractGraphNodeIdx fromGraphNodeIdx, AbstractGraphNodeIdx toGraphNodeIdx) const;

	const AbstractGraphCellBlob* GetAbstractGraphCellBlob(const LoadedAbstractGraphCellIdx cellIdx) const;
	AbstractGraphCellGrid* GetAbstractGraphCellGrid() const;
	Database* GetDatabase() const;

public: // internal
	KyUInt32 GetIndexInCollection() const { return m_abstractGraphIdx; }
	void SetIndexInCollection(KyUInt32 indexInCollection);

	bool IsNodePaired(const LoadedAbstractGraphNodeIdx& node) const;  ///<  It must be a node of this AbstractGraph
	bool IsNodePaired(AbstractGraphNodeIdx nodeIdx) const;

	// Debug-only, used in unittests, not needed otherwise, the VisualDebug uses Blobs instead of ScopedDisplayLists
	enum DebugDisplay
	{
		DebugDisplay_AbstractEdges    = 1 << 0,
		DebugDisplay_CellBoxCoverage  = 1 << 1,
		DebugDisplay_All = KyUInt32MAXVAL
	};
	void Display(ScopedDisplayList& displayList, const VisualColor& color, DebugDisplay debugDisplay = DebugDisplay_All);

public:
	Database* m_database;
	const AbstractGraphBlob* m_blob;
	AbstractGraphNodeLink* m_links; // for each graph nodes gives link information about the node in the neighbor AbstractGraph, and the Navmesh
	AbstractGraphIdx m_abstractGraphIdx;      ///< For TrackedCollection
};

}

#endif
