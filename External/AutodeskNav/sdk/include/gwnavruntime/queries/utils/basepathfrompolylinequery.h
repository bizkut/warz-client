/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BasePathFromPolylineQuery_H
#define Navigation_BasePathFromPolylineQuery_H

#include "gwnavruntime/queries/ipathfinderquery.h"
#include "gwnavruntime/querysystem/workingmemcontainers/navdatachangeindexingrid.h"
#include "gwnavruntime/queries/utils/pathfinderqueryutils.h"
#include "gwnavruntime/queries/utils/pathrefinercontext.h"
#include "gwnavruntime/queries/utils/pathclampercontext.h"

namespace Kaim
{

class Path;


/// Enumerates the possible results of an PathFromPolylineQuery.
enum PathFromPolylineQueryResult
{
	PATHFROMPOLYLINE_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	PATHFROMPOLYLINE_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	PATHFROMPOLYLINE_PROCESSING_PATHCLAMPING_INIT,      ///< Indicates that the clamper is beeing initialized. 
	PATHFROMPOLYLINE_PROCESSING_PATHCLAMPING,           ///< Indicates that the path is being clamped to navMesh according to the altitude tolerance.
	PATHFROMPOLYLINE_PROCESSING_PATHBUILDING,           ///< Indicates that the clamping algorithm has completed, but the path has not yet been built. 
	PATHFROMPOLYLINE_PROCESSING_CHANNEL_INIT,           ///< Unused. Kept here for backward compatibility
	PATHFROMPOLYLINE_PROCESSING_CHANNEL_COMPUTE,        ///< Indicates that the channel is initialized, the channel is being computed

	PATHFROMPOLYLINE_DONE_DATABASE_EMPTY,               ///< Indicates that the provided database is empty (no NavData !)
	PATHFROMPOLYLINE_DONE_END_OUTSIDE,                  ///< Indicates that the last point of the polyline is outside the NavMesh. 
	PATHFROMPOLYLINE_DONE_POLYLINE_INVALID,             ///< Indicates that one of the rayCast along polyline edge failed.
	PATHFROMPOLYLINE_DONE_PATH_NOT_FOUND,               ///< Indicates that a path could not be found between the start and destination points.
	PATHFROMPOLYLINE_DONE_NAVDATA_CHANGED,              ///< Indicates that the query has stopped and must be relaunched because of a change in the NavData.
	PATHFROMPOLYLINE_DONE_ERROR_LACK_OF_WORKING_MEMORY, ///< Indicates that insufficient working memory caused the query to stop. 
	PATHFROMPOLYLINE_DONE_COMPUTATION_ERROR,            ///< Indicates that a computation error caused the query to stop.
	PATHFROMPOLYLINE_DONE_CHANNELCOMPUTATION_ERROR,     ///< Indicates that a computation error caused the query to stop.
	PATHFROMPOLYLINE_DONE_COMPUTATION_CANCELED,         ///< Indicates that the computation has been canceled.
	PATHFROMPOLYLINE_DONE_ERROR_POLYLINE_EMPTY,         ///< Indicates that the input polyline in empty

	PATHFROMPOLYLINE_DONE_DEST_IS_START_NO_PATH,        ///< Indicates that query starting location and ending location are equals. computing a Path would have lead to a degenerated Path with only 1 node
	PATHFROMPOLYLINE_DONE_PATH_FOUND,                   ///< Indicates that a path has been found between the start and destination. 
};


/// Base class for AstarQuery.
class BasePathFromPolylineQuery : public IPathFinderQuery
{
public:
	static  QueryType            GetStaticType() { return TypePathFromPolyline; }
	virtual QueryType            GetType() const { return TypePathFromPolyline; }

	// ---------------------------------- Main API functions ----------------------------------

	virtual void                      GetPathFinderTextResult(String&)        const;
	virtual PathFinderResult          GetPathFinderResult()                   const;
	virtual const Vec3f*              GetDestination()                        const;

	PathFromPolylineQueryResult       GetResult()                             const;
	const Vec3f&                      GetStartPos()                           const;
	const KyArrayPOD<Vec3f>&          GetPolyline()                           const;
	PathFinderQueryComputeChannelMode GetComputeChannelMode()                 const;
	const ChannelComputerConfig&      GetChannelComputerConfig()              const;
	Path*                             GetPath()                               const;
	ChannelArray*                     GetChannelArray()                       const;
	KyUInt32                          GetNumberOfProcessedNodePerFrame()      const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

public: // Internal
	void BindToDatabase(Database* database);
	void Initialize(const Vec3f* vertexBuffer, KyUInt32 vertexCount);

	void SetComputeChannelMode(PathFinderQueryComputeChannelMode computeChannelMode);
	void SetChannelComputerConfig(const ChannelComputerConfig& channelComputerConfig);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);

protected :
	BasePathFromPolylineQuery();
	virtual ~BasePathFromPolylineQuery() {}
	void SetResult(PathFromPolylineQueryResult result);

protected :
	KyArrayPOD<Vec3f> m_polylinePositions;

	/// The altitude range used to find NavMesh triangles that correspond to the start and destination points.
	PositionSpatializationRange m_positionSpatializationRange;

	PathFromPolylineQueryResult m_result; ///< Updated during processing to indicate the result of the PathFinderQueryResult query.

public : //internal
	/// The number of CanGo tests that will be processed each frame during the refiner part of the query.
	KyUInt32 m_numberOfRayCastPerAdavance;

	/// The number of intersections that will be processed each frame during the clamper part of the query.
	KyUInt32 m_numberOfIntersectionTestPerAdvance;
};


KY_INLINE void BasePathFromPolylineQuery::SetResult(PathFromPolylineQueryResult result)                 { m_result                = result; }
KY_INLINE void BasePathFromPolylineQuery::SetComputeChannelMode(PathFinderQueryComputeChannelMode mode) { m_computeChannelMode    = mode;   }
KY_INLINE void BasePathFromPolylineQuery::SetChannelComputerConfig(const ChannelComputerConfig& config) { m_channelComputerConfig = config; }

KY_INLINE PathFromPolylineQueryResult       BasePathFromPolylineQuery::GetResult()                const { return m_result;                        }
KY_INLINE Path*                             BasePathFromPolylineQuery::GetPath()                  const { return m_path;                          }
KY_INLINE PathFinderQueryComputeChannelMode BasePathFromPolylineQuery::GetComputeChannelMode()    const { return m_computeChannelMode;            }
KY_INLINE const ChannelComputerConfig&      BasePathFromPolylineQuery::GetChannelComputerConfig() const { return m_channelComputerConfig;         }
KY_INLINE const Vec3f&                      BasePathFromPolylineQuery::GetStartPos()              const { return IPathFinderQuery::GetStartPos(); }
KY_INLINE const KyArrayPOD<Vec3f>&          BasePathFromPolylineQuery::GetPolyline()              const { return m_polylinePositions;             }
KY_INLINE const Vec3f* BasePathFromPolylineQuery::GetDestination() const
{
	if (m_polylinePositions.GetCount() == 0)
		return KY_NULL;

	return &m_polylinePositions.Back();
}

KY_INLINE const PositionSpatializationRange& BasePathFromPolylineQuery::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }
KY_INLINE void BasePathFromPolylineQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_positionSpatializationRange = positionSpatializationRange;
}

}

#endif //Navigation_AstarQuery_H

