/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_PathFinderQueryUtils_H
#define Navigation_PathFinderQueryUtils_H

#include "gwnavruntime/queries/utils/queryutils.h"
#include "gwnavruntime/queries/utils/pathrefinercontext.h"
#include "gwnavruntime/queries/utils/pathclampercontext.h"
#include "gwnavruntime/queries/utils/coneintersector.h"
#include "gwnavruntime/queries/utils/bestfirstsearchedgecollisionvisitor.h"
#include "gwnavruntime/queries/utils/bestfirstsearchtraversal.h"
#include "gwnavruntime/queries/utils/pathrefinerconfig.h"
#include "gwnavruntime/queries/insideposfromoutsideposquery.h"
#include "gwnavruntime/queries/raycangoquery.h"
#include "gwnavruntime/queries/monodirectionalraycangoquery.h"
#include "gwnavruntime/database/database.h"
#include "gwnavruntime/navgraph/navgraphlink.h"
#include "gwnavruntime/path/path.h"


namespace Kaim
{

/// This class is an helper used internaly by the PathFinder Queries to factorize Code
class PathFinderQueryUtils : public QueryUtils
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	PathFinderQueryUtils() {}
	PathFinderQueryUtils(Database* database, WorkingMemory* workingMemory, void* traverseLogicUserData)
	{
		Init(database, workingMemory, traverseLogicUserData);
	}

	void Init(Database* database, WorkingMemory* workingMemory, void* traverseLogicUserData)
	{
		QueryUtils::Init(database, workingMemory, traverseLogicUserData);
	}

public:
	template<class TraverseLogic>
	KyResult TryToHookOnNavMesh(const Vec3f& inputOutsidePos, const PositionSpatializationRange& positionSpatializationRange,
		KyFloat32 horizontalHookingMaxDist, KyFloat32 distFromObstacle, Vec3f& outputInsidePos, NavTrianglePtr& outputNavTrianglePtr);

	template <class TraverseLogic>
	KyResult RefineOneNode(const PositionSpatializationRange& positionSpatializationRange, const PathRefinerConfig& pathRefinerConfig, KyUInt32& cangoTestDone);

	template <class TraverseLogic>
	Ptr<Path> ComputePathFromPathClamperContext(const Vec3f& realStartPos, const Vec3f& realEndPos, const PositionSpatializationRange& positionSpatializationRange);

	void InitRayCanGoWithCost(BaseRayCanGoQuery& rayCanGo, const Vec3f& startPos, const NavTrianglePtr& startTrianglePtr, const Vec3f& destPos, const WorldIntegerPos& stratIntegerPos, const WorldIntegerPos& destIntegerPos);

	void ComputeRefinerNodeCost(RefinerNode* currentNode);
	KyResult UpdateNodeInBinaryHeap(RefinerNodeIndex nodeIdx, RefinerNode* currentNode);
	KyResult BuildRefinerBinaryHeap();


	KyUInt32 ClampOneEdge(KyUInt32& intersectionTestCount);

	void DisplayListRefining(const char* queryName);
	void DisplayListPropagation(const char* queryName);

private:
	enum NearestCornerType
	{
		NearestCornerType_Undefined,
		NearestCornerType_PrevNode,
		NearestCornerType_InnerCorner,
		NearestCornerType_NextNode
	};
};

}

#include "gwnavruntime/queries/utils/pathfinderqueryutils.inl"

#endif //Navigation_PathFinderQueryUtils_H

