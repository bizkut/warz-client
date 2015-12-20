/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/




// primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_NavGraphLinker_H
#define Navigation_NavGraphLinker_H

#include "gwnavruntime/basesystem/basesystem.h"
#include "gwnavruntime/navgraph/navgraphtypes.h"

namespace Kaim
{

class NavGraph;
class Database;
class NavCellGrid;
class GraphVertexData;
class TriangleFromPosQuery;

///////////////////////////////////////////////////////////////////////////////////////////
// NavGraphLinker
///////////////////////////////////////////////////////////////////////////////////////////

/* Used internally by the GraphManager to stitch Graphs together at runtime. */
class NavGraphLinker
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(NavGraphLinker)

public:
	NavGraphLinker(Database* database) : m_database(database) {}
	~NavGraphLinker() { Clear(); }

	void Clear() {}

	KyResult LinkNavGraph(NavGraph* navGraph);
	KyResult UnLinkNavGraph(NavGraph* navGraph);

	KyResult LinkNavGraphVertex(NavGraph* navGraph, NavGraphVertexIdx vertexIdx, TriangleFromPosQuery& triangleFromPosQuery);
	KyResult UnLinkNavGraphVertex(GraphVertexData& vertexData);

public:
	Database* m_database;
};

} // namespace Kaim

#endif // Navigation_NavGraphLinker_H
