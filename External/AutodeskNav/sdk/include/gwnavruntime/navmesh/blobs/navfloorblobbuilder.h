/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavFloorBlobBuilder_H
#define Navigation_NavFloorBlobBuilder_H

#include "gwnavruntime/blob/baseblobbuilder.h"

namespace Kaim
{

class DynamicNavFloor;
class NavFloorBlob;
class NavFloor1To1StitchDataBlob;
class NavFloor1To1StitchDataBlobBuilder;
class Stitch1To1ToHalfEdgeInFloor;
class NavHalfEdge;
class DynamicNavVertex;
class NavVertex;

class NavFloorBlobBuilder : public BaseBlobBuilder<NavFloorBlob>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	NavFloorBlobBuilder(const DynamicNavFloor& dynamicNavFloor, const NavFloor1To1StitchDataBlob* floor1To1StitchData = KY_NULL) :
		m_dynamicNavFloor(&dynamicNavFloor), m_floor1To1StitchData(floor1To1StitchData) {}

private:
	virtual void DoBuild();

	void WriteNavHalfEdges(NavHalfEdge* edges, KyUInt32 numberOfTriangles) const;
	void ComputeNavFloorAABB(KyUInt32 numberOfVertices);
private:
	const DynamicNavFloor* m_dynamicNavFloor;
	const NavFloor1To1StitchDataBlob* m_floor1To1StitchData;

};

class NavFloor1To1StitchDataBlobBuilder : public BaseBlobBuilder<NavFloor1To1StitchDataBlob>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	NavFloor1To1StitchDataBlobBuilder(const DynamicNavFloor& dynamicNavFloor) :
	m_dynamicNavFloor(&dynamicNavFloor) {}

private:
	virtual void DoBuild();
	void PushVertex(const DynamicNavVertex& vertex, NavVertex* navVerticesBuffer, KyFloat32* navVertexAltitudesBuffer, KyUInt32 currentVertexIdx);
	void SetEdgeData(KyUInt32& edgeData, KyUInt32 startVertexIdx, KyUInt32 endVertexIdx, KyUInt32 edgeType)
	{
		KY_ASSERT(startVertexIdx != KyUInt32MAXVAL && ((startVertexIdx & 0xFFF00000) == 0));
		KY_ASSERT(endVertexIdx != KyUInt32MAXVAL && ((endVertexIdx & 0xFFF00000) == 0));
		edgeData = 0;
		edgeData |= startVertexIdx;
		edgeData |= (endVertexIdx << 12);
		edgeData |= (edgeType << 29);
	}
private:
	const DynamicNavFloor* m_dynamicNavFloor;
};


class Stitch1To1ToHalfEdgeInFloorBlobBuilder : public BaseBlobBuilder<Stitch1To1ToHalfEdgeInFloor>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	Stitch1To1ToHalfEdgeInFloorBlobBuilder(const DynamicNavFloor* dynamicNavFloor, const NavFloor1To1StitchDataBlob* floor1To1StitchData) :
	m_dynamicNavFloor(dynamicNavFloor), m_floor1To1StitchDataBlob(floor1To1StitchData) {}

private:
	virtual void DoBuild();

private:
	const DynamicNavFloor* m_dynamicNavFloor;
	const NavFloor1To1StitchDataBlob* m_floor1To1StitchDataBlob;
};


} // namespace Kaim

#endif // Navigation_NavFloorBlobBuilder_H
