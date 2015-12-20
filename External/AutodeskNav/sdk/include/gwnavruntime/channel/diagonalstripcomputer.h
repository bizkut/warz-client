/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_DiagonalStripComputer_H
#define Navigation_DiagonalStripComputer_H

#include "gwnavruntime/channel/diagonalstrip.h"
#include "gwnavruntime/path/path.h"

#include "gwnavruntime/channel/channelarraycomputerstats.h"


#include "gwnavruntime/queries/querydynamicoutput.h"
#include "gwnavruntime/queries/trianglefromposquery.h"
#include "gwnavruntime/queries/utils/bestfirstsearchtraversal.h"
#include "gwnavruntime/queries/utils/bestfirstsearch2dbordercollector.h"
#include "gwnavruntime/queries/utils/clearancepolygonintersector.h"
#include "gwnavruntime/queries/raycangoquery.h"

#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/base/timeprofiler.h"


namespace Kaim
{

class DiagonalStripComputerContext;
class Path;

class DiagonalStripComputerConfig
{
public:
	DiagonalStripComputerConfig() { SetDefaults(); }
	void SetDefaults() { m_idealCornerRadius = 2.5f; }

	KyFloat32 m_idealCornerRadius;
};

enum DiagonalStripComputerResult
{
	DiagonalStripResult_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	DiagonalStripResult_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	DiagonalStripResult_BuildDiagonalForPathEdge,
	DiagonalStripResult_ComputingMaxRadius_Init,
	DiagonalStripResult_ComputingMaxRadius,
	DiagonalStripResult_ComputingNeighborCompatibleRadius_Backward_Init,
	DiagonalStripResult_ComputingNeighborCompatibleRadius_Backward,
	DiagonalStripResult_ComputingNeighborCompatibleRadius_Forward_Init,
	DiagonalStripResult_ComputingNeighborCompatibleRadius_Forward,

	DiagonalStripResult_Done_DiagonalStripComputerError,
	DiagonalStripResult_Done_ComputeMaxRadiusError,
	DiagonalStripResult_Done_ComputeNeighborCompatibleRadiusError,

	DiagonalStripResult_Done
};

enum DiagonalStripJunctionType
{
	JunctionType_Slanting,
	JunctionType_AlmostOrtho
};

class DiagonalStripComputer
{
public:
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_QueryWorkingMem)
public:
	DiagonalStripComputer();
	~DiagonalStripComputer();

	void Clear();
	void Init(Database* database, const Path* path, KyUInt32 firstEdgeIdxOfSection, KyUInt32 lastEdgeIdxOfSection, DiagonalStrip* diagonalStrip);

	bool IsFinished() const { return m_result >= DiagonalStripResult_Done_DiagonalStripComputerError; }

	void* GetTraverseLogicUserData() const { return m_traverseLogicUserData; }
	void SetTraverseLogicUserData(void* traverseLogicUserData) { m_traverseLogicUserData = traverseLogicUserData; }

	DiagonalStripComputerResult GetResult() const { return m_result; }
	DiagonalStripComputerConfig& GetComputerConfig() { return m_computerConfig; }
	void SetComputerConfig(const DiagonalStripComputerConfig& computerConfig) { m_computerConfig = computerConfig; }

protected:
	KyResult ComputeCornerRadius(KyUInt32 cornerIdx);

	enum ComputeCompatibleRadiusResult { OutOfNeighborhood, Shrinked, Unchanged };

	KyResult ComputeForwardNeighborCompatibleCornerRadius(KyUInt32 cornerIdx);
	KyResult ComputeBackwardNeighborCompatibleCornerRadius(KyUInt32 cornerIdx);
	ComputeCompatibleRadiusResult ComputeCompatibleRadius(const Vec3f& cornerPos, CornerType cornerType, KyUInt32 otherIdx, KyFloat32& cornerRadius);

	void InitContext(DiagonalStripComputerContext& context, WorkingMemory* workingMemory);
	void FillDiagonalStrip(ClearancePolygonIntersector& clearancePolygonIntersector);
	bool IsCCWQuadConvex(const Vec3f& A, const Vec3f& B, const Vec3f& C, const Vec3f& D);
	void ComputeBissectriceEndPoints(DiagonalStripComputerContext& context, const Vec3f& nextPointOnPath, KyFloat32 nextEdgelength2D, KyFloat32 crossProdWithNextEdge);

public :
	template<class TraverseLogic>
	void Advance(WorkingMemory* workingMemory, ChannelArrayComputerStats& channelStats);

	template<class TraverseLogic>
	KyResult ComputeDiagonalsFromPath(WorkingMemory* workingMemory, ChannelArrayComputerStats& stats);

	template<class TraverseLogic>
	KyResult BuildDiagonalsForPathEdge(WorkingMemory* workingMemory);

	KyResult ComputeTriangleFromStartToEnd(WorkingMemory* workingMemory, const Vec3f& pathEdgeStart, const Vec3f& pathEdgeEnd, NavTrianglePtr& startTrianglePtr);

	template<class TraverseLogic>
	KyResult BuildDiagonalsInTrapeze(WorkingMemory* workingMemory, const Vec3f& pathEdgeStart, const Vec3f& pathEdgeEnd, NavTrianglePtr& inAndOutTrianglePtr,
		const Vec3f& pointOnStartSideLeft, const Vec3f& pointOnEndSideLeft);

	template<class TraverseLogic>
	KyResult BuildDiagonalsInTriangle(WorkingMemory* workingMemory, const Vec3f& pathEdgeStart, const Vec3f& pathEdgeEnd, NavTrianglePtr& inAndOutTrianglePtr,
		const Vec3f& pointOnStartSideLeft, const Vec3f& pointOnStartSideRight, const Vec3f& pointOnEndSideLeft, const Vec3f& pointOnEndSideRight);

	template<class TraverseLogic>
	KyResult ProcessTheTwoTriangleStartingThePolygon(WorkingMemory* workingMemory, DiagonalStripComputerContext& context);

	template<class TraverseLogic>
	KyResult ProcessPolygonWithTriangles(WorkingMemory* workingMemory, DiagonalStripComputerContext& context,
		Vec3f& bissectricIntersection, bool bissectriceIntersect, KyFloat32 maxDotProdStart, KyFloat32 minDotProdEnd);

	template<class TraverseLogic>
	KyResult ProcessTwoOppositeTriangles(WorkingMemory* workingMemory, DiagonalStripComputerContext& context,
		const Vec3f& endPointLeft, const Vec3f& endPointRight, const Vec3f& endPointOnPath);

	template<class TraverseLogic>
	KyResult ProcessTrianglesTurningAroundBissectriceIntersection(WorkingMemory* workingMemory, DiagonalStripComputerContext& context,
		const Vec3f& bissectricIntersection);

	template<class TraverseLogic>
	KyResult EndPolygonFromAlmostOrthoStart(WorkingMemory* workingMemory, DiagonalStripComputerContext& context,
		KyFloat32 maxDotProdStart, KyFloat32 minDotProdEnd);

public: // internal
	void ClearDisplayList();

protected:
	Database* m_database;
	DiagonalStripComputerConfig m_computerConfig;
	void* m_traverseLogicUserData;

	DiagonalStripComputerResult m_result;

	const Path* m_rawPath;

	DiagonalStrip* m_diagonalStrip;

	// diagonal strip temp
	KyUInt32 m_currentPathEdgeIdx;
	KyUInt32 m_firstEdgeIdxOfSection;
	KyUInt32 m_lastEdgeIdxOfSection;
	KyInt32 m_currentCornerIdx;

	Vec3f m_startPointOnLeft;
	Vec3f m_startPointOnRight;
	DiagonalStripJunctionType m_startJunctionType;
	Vec3f m_lastStripCornerLeft;
	Vec3f m_lastStripCornerRight;
	ScopedDisplayList* m_displayList;

	Ptr<QueryDynamicOutput> m_queryDynamicOutput;
	VisualColor m_currentColor;
};


} // namespace Kaim



#include "gwnavruntime/channel/diagonalstripcomputer.inl"


#endif // Navigation_DiagonalStripComputer_H
