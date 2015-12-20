/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_BaseFindFirstVisiblePositionOnPathQuery_H
#define Navigation_BaseFindFirstVisiblePositionOnPathQuery_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/queries/utils/queryutils.h"
#include "gwnavruntime/path/positiononpath.h"


namespace Kaim
{

/// Enumerates the possible results of a FindFirstVisiblePositionOnPathQuery.
enum FindFirstVisiblePositionOnPathQueryResult
{
	FINDFIRSTVISIBLEPOSITIONONPATH_NOT_INITIALIZED = QUERY_NOT_INITIALIZED,          ///< Indicates the query has not yet been initialized.
	FINDFIRSTVISIBLEPOSITIONONPATH_NOT_PROCESSED   = QUERY_NOT_PROCESSED,            ///< Indicates the query has not yet been launched.

	FINDFIRSTVISIBLEPOSITIONONPATH_DONE_VISIBILITYSTARTPOINT_OUTSIDE,                ///< Indicates that a NavMesh triangle could not be found for the visibilityStartPoint.
	FINDFIRSTVISIBLEPOSITIONONPATH_DONE_VISIBILITYSTARTPOINT_NAVTAG_FORBIDDEN,       ///< Indicates that a NavMesh triangle has been found for visibilityStartPoint, but its NavTag is not considered navigable.
	FINDFIRSTVISIBLEPOSITIONONPATH_DONE_NO_VISIBLE_POSITION_FOUND,                   ///< Indicates that none of the PositionOnPath locations sampled between startPositionOnPath and endPositionOnPath are visible from the  visibilityStartPoint.
	FINDFIRSTVISIBLEPOSITIONONPATH_DONE_LACK_OF_WORKING_MEMORY,                      ///< Indicates that insufficient memory was available to store crossed triangles or sub-sections, and the query failed as a result.
	FINDFIRSTVISIBLEPOSITIONONPATH_DONE_UNKNOWN_ERROR,                               ///< Indicates that an unknown error occurred during the query processing.

	FINDFIRSTVISIBLEPOSITIONONPATH_DONE_SUCCESS                                      ///< Indicates that the query can pass in a straight line from start to end point without a collision.
};

/// Base class for FindFirstVisiblePositionOnPathQuery.
class BaseFindFirstVisiblePositionOnPathQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeFindFirstVisiblePositionOnPath; }
	virtual QueryType GetType() const { return TypeFindFirstVisiblePositionOnPath; }
	virtual void BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler> CreateStaticQueryBlobHandler();


	// ---------------------------------- Main API Functions ----------------------------------

	FindFirstVisiblePositionOnPathQueryResult GetResult()   const;
	const PositionOnPath& GetStartPositionOnPath()          const;
	const PositionOnPath& GetEndPositionOnPath()            const;
	const Vec3f&          GetVisibilityStartPoint()         const;
	const NavTrianglePtr& GetVisibilityStartTrianglePtr()   const;
	KyFloat32             GetSamplingDistance()             const;
	RayCanGoMarginMode    GetRayCanGoMarginMode()           const;
	KyFloat32             GetCostToVisiblePositionOnPath() const;
	const PositionOnPath& GetFirstVisiblePositionOnPath()   const;

protected:
	enum CandidateValidationResult
	{
		ContinueSampling,
		StopSampling
	};

	BaseFindFirstVisiblePositionOnPathQuery();
	virtual ~BaseFindFirstVisiblePositionOnPathQuery() {}

	void BindToDatabase(Database* database);
	void Initialize(const PositionOnPath& startPositionOnPath, const PositionOnPath& endPositionOnPath, const Vec3f& visibilityStartPoint);

	void SetVisibilityStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);
	void SetSamplingDistance(KyFloat32 samplingDistance);
	void SetRayCanGoMarginMode(RayCanGoMarginMode rayCanGoMarginMode);
	void SetResult(FindFirstVisiblePositionOnPathQueryResult result);

protected:
	PositionOnPath m_startPositionOnPath; ///< The start point of the PositionOnPath query on the Path.
	PositionOnPath m_endPositionOnPath;   ///< The end point of the PositionOnPath query on the Path.

	Vec3f m_visibilityStartPos3f; ///< The visibility check starting position.
	NavTrianglePtr m_visibilityStartTrianglePtr; ///< The NavMesh triangle that corresponds to #m_visibilityStartPos3f. Can be set as an input, or retrieved as an output of the query. 

	RayCanGoMarginMode m_rayCanGoMarginMode; ///< The margin mode to be used in ray can go queries.

	KyFloat32 m_samplingDistance;

	FindFirstVisiblePositionOnPathQueryResult m_result; ///< Updated during processing to indicate the result of the query.
	PositionOnPath m_firstVisiblePositionOnPath;  ///< Updated during processing to indicate the first visible PositionOnPath found.
	KyFloat32 m_costToVisibleTargetOnPath;
};

} // namespace Kaim

#include "gwnavruntime/queries/utils/basefindfirstvisiblepositiononpathquery.inl"



#endif // Navigation_BaseFindFirstVisiblePositionOnPathQuery_H

