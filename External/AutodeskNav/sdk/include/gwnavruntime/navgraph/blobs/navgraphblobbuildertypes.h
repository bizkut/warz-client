/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: LASI - secondary contact: NONE
#ifndef Navigation_NavGraphBlobBuilderTypes_H
#define Navigation_NavGraphBlobBuilderTypes_H

#include "gwnavruntime/navgraph/navgraphtypes.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/database/navtag.h"

namespace Kaim
{

class NavGraphBlobBuilder_Vertex
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	NavGraphBlobBuilder_Vertex() : m_navTagIdx(0), m_vertexType(NavGraphVertexLinkType_LinkToNavMesh), m_vertexIdx(KyUInt32MAXVAL) {}
	NavGraphBlobBuilder_Vertex(const Vec3f& position, KyUInt32 navTagIdx, NavGraphVertexLinkType vertexType, KyUInt32 vertexIdx) :
	m_position(position), m_navTagIdx(navTagIdx), m_vertexType(vertexType), m_vertexIdx(vertexIdx) {}

public:
	Vec3f m_position; //< Position of the vertex.
	KyUInt32 m_navTagIdx;
	NavGraphVertexLinkType m_vertexType; ///< Indicates the way this vertex should be stitched to the surrounding Graph at runtime. 
	KyUInt32 m_vertexIdx;
	KyArray<KyUInt32> m_outgoingEdgesIdx; //< outgoing edges.
};

// Contains all basic data of a graph edge.
class NavGraphBlobBuilder_Edge
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	NavGraphBlobBuilder_Edge(): m_startVertexIdx(KyUInt32MAXVAL), m_endVertexIdx(KyUInt32MAXVAL), m_navTagIdx(0) {}
	NavGraphBlobBuilder_Edge(KyUInt32 startVertexIdx, KyUInt32 endVertexIdx, KyUInt32 navTagIdx) :
	m_startVertexIdx(startVertexIdx), m_endVertexIdx(endVertexIdx), m_navTagIdx(navTagIdx)
	{
		KY_LOG_ERROR_IF(startVertexIdx == endVertexIdx, ("Creation of an illegal circular edge."));
	}

public:
	KyUInt32 m_startVertexIdx;   //< start vertex.
	KyUInt32 m_endVertexIdx;     //< end vertex.
	KyUInt32 m_navTagIdx;
};

}

#endif // #define Navigation_NavGraphBlobBuilderTypes_H
