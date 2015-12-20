/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_IPathFinderQuery_H
#define Navigation_IPathFinderQuery_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/path/path.h"
#include "gwnavruntime/channel/channelcomputerconfig.h"
#include "gwnavruntime/kernel/SF_String.h"

namespace Kaim
{

class ScopedDisplayList;

/// This enum tells if the PathFinderQuery should compute Channels around path sections laying on the NavMesh.
/// By default it is disabled (PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
enum PathFinderQueryComputeChannelMode
{
	PATHFINDER_DO_NOT_COMPUTE_CHANNELS,
	PATHFINDER_DO_COMPUTE_CHANNELS,
};

/// Base class for all pathfinder queries.
/// Pathfinder queries takes all at least a start position as input and returns a
/// Path as an output.
class IPathFinderQuery : public ITimeSlicedQuery
{
public:
	/// Enumerates the possible status of a IPathFinderQuery.
	enum PathFinderResult
	{
		PathFinderNotInitialized, ///< The IPathFinderQuery is not yet initialized.
		PathFinderNotComputed,    ///< The IPathFinderQuery is initialized but has not been processed.
		PathFinderUnderProcess,   ///< The IPathFinderQuery is currently under process.
		PathFinderSuccess,        ///< The IPathFinderQuery has been completed and a Path has been found.
		PathFinderFailure         ///< The IPathFinderQuery has been completed but no Path has been found.
	};

	// ---------------------------------- Public Member Functions ----------------------------------

	IPathFinderQuery();
	virtual ~IPathFinderQuery() {}

	Path* GetPath() const;
	ChannelArray* GetChannelArray() const;
	const Vec3f& GetStartPos() const;

	void SetNavigationProfileId(KyUInt32 navigationProfileId);
	KyUInt32 GetNavigationProfileId() const;

	// ---------------------------------- Virtual Member Functions ----------------------------------

	/// This function is virtual because not all the PathFinders have a preset destination, it may return KY_NULL.
	virtual const Vec3f* GetDestination() const = 0;

	/// Fills the String with an explanation of the result. Use this to debug your pathfinder query.
	virtual void GetPathFinderTextResult(String&) const {}

	/// Fill the displayList with display info that may help to understand a PathFinderFailure (mainly propagation bounds)
	virtual void DisplayPropagationBounds(ScopedDisplayList&) const {}

	virtual PathFinderResult GetPathFinderResult() const = 0;
protected:
	void SetPath(Path* path) { m_path = path; }
public:
	// ---------------------------------- Public Data Members ----------------------------------

	Vec3f m_startPos3f;               ///< The starting position for the Path request.
	Ptr<Path> m_path;                 ///< The Path found by the query.
	
	KyUInt32 m_navigationProfileId;

	PathFinderQueryComputeChannelMode m_computeChannelMode; ///< The mode toggling Channels computation.
	ChannelComputerConfig m_channelComputerConfig; ///< The parameter set specific to Channels computation.

	KyUInt32 m_dataBaseChangeIdx;     ///< Stores the revision of the Database when the Path has been computed.
};

KY_INLINE IPathFinderQuery::IPathFinderQuery() :
	m_startPos3f(KyFloat32MAXVAL, KyFloat32MAXVAL, KyFloat32MAXVAL),
	m_path(KY_NULL),
	m_navigationProfileId(0),
	m_computeChannelMode(PATHFINDER_DO_NOT_COMPUTE_CHANNELS),
	m_dataBaseChangeIdx(0)
{}

KY_INLINE Path*         IPathFinderQuery::GetPath()         const { return m_path;         }
KY_INLINE const Vec3f&  IPathFinderQuery::GetStartPos()     const { return m_startPos3f;   }


KY_INLINE void     IPathFinderQuery::SetNavigationProfileId(KyUInt32 navigationProfileId) { m_navigationProfileId = navigationProfileId; }
KY_INLINE KyUInt32 IPathFinderQuery::GetNavigationProfileId() const { return m_navigationProfileId; }


/// utility class used to set to the SetNavigationProfileId of the path easily
/// created on the stack at the beginning of the PathFinder::Advance() implementations
class ScopedSetPathNavigationProfile
{
public:
	ScopedSetPathNavigationProfile(IPathFinderQuery* query) : m_query(query) {}
	~ScopedSetPathNavigationProfile()
	{
		if (m_query->m_path)
			m_query->m_path->SetNavigationProfileId(m_query->GetNavigationProfileId());
	}
	IPathFinderQuery* m_query;
};

}

#endif //Navigation_IPathFinderQuery_H

