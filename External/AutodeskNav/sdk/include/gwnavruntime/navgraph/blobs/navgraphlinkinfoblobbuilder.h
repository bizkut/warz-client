/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_NavGraph_DisplayListBuilder_H
#define Navigation_NavGraph_DisplayListBuilder_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/navgraph/blobs/navgraphlinkinfoblob.h"

namespace Kaim
{

class NavGraph;
class NavGraphVertexLinkInfoBlob;
class NavGraphVertex;
class GraphVertexData;
class NavGraphVertexSpatializationBlob;
class NavGraphLink;
class NavGraphArrayLinkInfoBlob;
class NavData;

class NavGraphLinkInfoBlobBuilder : public BaseBlobBuilder<NavGraphLinkInfoBlob>
{
public:
	NavGraphLinkInfoBlobBuilder(NavGraph* navGraph, KyUInt32 graphIndex) : m_navGraph(navGraph), m_graphIndex(graphIndex) {}

private:
	virtual void DoBuild();

	NavGraph* m_navGraph;
	KyUInt32 m_graphIndex;
};

class NavGraphVertexLinkInfoBlobBuilder : public BaseBlobBuilder<NavGraphVertexLinkInfoBlob>
{
public:
	NavGraphVertexLinkInfoBlobBuilder(const NavGraphVertex* vertex, const GraphVertexData* vertexData) : m_vertex(vertex), m_vertexData(vertexData) {}

private:
	virtual void DoBuild();

	const NavGraphVertex* m_vertex;
	const GraphVertexData* m_vertexData;
};

class NavGraphVertexSpatializationBlobBuilder : public BaseBlobBuilder<NavGraphVertexSpatializationBlob>
{
public:
	NavGraphVertexSpatializationBlobBuilder(NavGraphLink* link) : m_link(link) {}

private:
	virtual void DoBuild();

	NavGraphLink* m_link;
};

class NavGraphArrayLinkInfoBlobBuilder : public BaseBlobBuilder<NavGraphArrayLinkInfoBlob>
{
public:
	NavGraphArrayLinkInfoBlobBuilder(NavData* navData) : m_navData(navData) {}

private:
	virtual void DoBuild();

	NavData* m_navData;
};

}

#endif