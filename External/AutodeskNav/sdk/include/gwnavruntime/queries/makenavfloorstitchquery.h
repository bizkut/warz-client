/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LASI - secondary contact: NOBODY
#ifndef Navigation_MakeNavFloorStitchQuery_H
#define Navigation_MakeNavFloorStitchQuery_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/navmesh/blobs/navfloorblob.h"
#include "gwnavruntime/blob/blobhandler.h"
#include "gwnavruntime/navmesh/navfloor.h"

namespace Kaim
{
class NavCell;
class NavFloorBlob;
class DynamicTriangulation;
class DynamicNavFloor;
class NavFloorStitcherData;

/// Enumerates the possible results of a MakeNavFloorStitchQuery.
enum MakeNavFloorStitchQueryResult
{
	RUNTIMESTITCH_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	RUNTIMESTITCH_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	RUNTIMESTITCH_PROCESSING,

	RUNTIMESTITCH_LACK_OF_WORKING_MEMORY,                  ///< Indicates that insufficient working memory caused the query to stop.
	RUNTIMESTITCH_UNKNOWN_ERROR,                           ///< Indicates an unknown error occurred during the query processing. 

	RUNTIMESTITCH_DONE_SUCCESS,                            ///< Indicates the query was successfully processed.
};

class MakeNavFloorStitchQuery : public ITimeSlicedQuery
{

public:
	static  QueryType GetStaticType() { return TypeDynamicNavMesh; }
	virtual QueryType GetType() const { return TypeDynamicNavMesh; }

	MakeNavFloorStitchQuery();
	virtual ~MakeNavFloorStitchQuery();

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);
	void Initialize(NavCell* navCell);

	virtual void Advance(WorkingMemory* workingMemory);
	virtual void ReleaseWorkingMemoryOnCancelDuringProcess(WorkingMemory* workingMemory = KY_NULL);
	/// Read accessor for #m_result. 
	MakeNavFloorStitchQueryResult GetResult() const { return m_result; };

	// ---------------------------------- Accessors ----------------------------------
	void SetResult(MakeNavFloorStitchQueryResult result) { m_result = result; };
	static void FillEdgeExtremities(const NavFloorBlob* navFloorBlob, NavHalfEdgeIdx halfEdgeIdx, Vec2i& start, Vec2i& end);

private:
	void ProcessNavFloor();

	class NewCutAlongStaticEdge
	{
	public:
		NewCutAlongStaticEdge() {}
		NewCutAlongStaticEdge(const Vec2i& pos, KyFloat32 alt) : m_pos(pos), m_altitude(alt) {}
		bool operator==(const NewCutAlongStaticEdge& other) const { return m_pos == other.m_pos; }
		bool operator!=(const NewCutAlongStaticEdge& other) const { return m_pos != other.m_pos; }

	public:
		Vec2i m_pos;
		KyFloat32 m_altitude;
	};

	class RunTimeStitchCutSorter
	{
	public:
		RunTimeStitchCutSorter(const Vec2i& stitch1To1EndVertex, KyInt32 currentEdgeDir) :
		m_stitch1To1EndVertex(stitch1To1EndVertex), m_coordIdx(currentEdgeDir) {}

		bool operator() (const NewCutAlongStaticEdge& cut1, const NewCutAlongStaticEdge& cut2) const
		{
			KY_ASSERT(cut1.m_pos[m_coordIdx] != m_stitch1To1EndVertex[m_coordIdx]);
			KY_ASSERT(cut2.m_pos[m_coordIdx] != m_stitch1To1EndVertex[m_coordIdx]);
			const KyInt32 diff1 = cut1.m_pos[m_coordIdx] - m_stitch1To1EndVertex[m_coordIdx];
			const KyInt32 diff2 = cut2.m_pos[m_coordIdx] - m_stitch1To1EndVertex[m_coordIdx];
			const KyInt32 squareDist1 = diff1*diff1;
			const KyInt32 squareDist2 = diff2*diff2;
			return squareDist1 < squareDist2;
		}

		Vec2i m_stitch1To1EndVertex;
		KyInt32 m_coordIdx;
	};


	void ComputeAndInsertAllCuts(NavFloorStitcherData& currentData, KyUInt32 stitch1To1EdgeIdx, const Vec2i& stitch1To1EndVertex,
		NavFloorStitcherData& neighborData, KyUInt32 neighborStitch1To1EdgeIdx, KyArray<NewCutAlongStaticEdge>& newCuts, DynamicTriangulation& dynTri);
	void InsertexVertexInTriangulationOnEdge(DynamicTriangulation& dynTri, KyUInt32 halfEdgeIdx, NewCutAlongStaticEdge& newCut);
	void BuildDynamicTriangulationFromStaticNavFloorBlob(const NavFloorBlob* floorBlob, DynamicTriangulation& dynTri);
	void BuildDynamicNavFloorFromDynamicTriangulation(const NavFloorBlob* floorBlob, const NavFloor1To1StitchDataBlob& floor1To1StitchDataBlob,
		const DynamicTriangulation& dynTri, DynamicNavFloor& dynaFloor);

	void SetFinish(WorkingMemory* workingMemory);
	void ReleaseWorkingMemory() {}


public:
	/// Input Data
	NavCell* m_navCell;
	KyArray<Ptr<BlobHandler<NavFloorBlob> > > m_resultNavfloorHandlers;
	MakeNavFloorStitchQueryResult m_result; ///< Updated during processing to indicate the result of the query. 
	KyInt32 m_coordIdx; // 0 for x, 1 for y
	NavFloorIdx m_currentFloorIdx;
	KyArray<KyUInt16> m_navHalfEdgeToTriangulationEdge;
	bool m_dynamicTriangulationFromFloorIsBuilt;
};

KY_INLINE void MakeNavFloorStitchQuery::SetFinish(WorkingMemory* /*workingMemory*/)
{
	m_processStatus = QueryDone; // TODO - CHANGE THIS!
	ReleaseWorkingMemory();
}



}





#endif //Navigation_MakeNavFloorStitchQuery_H

